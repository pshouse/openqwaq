/****************************************************************************
*   PROJECT: Win32 time functions and heartbeat logic for Stack VM
*   FILE:    sqWin32Heartbeat.c
*   CONTENT: 
*
*   AUTHOR:  Eliot Miranda
*   ADDRESS: 
*   EMAIL:   eliot@teleplace.com
*   RCSID:   $Id$
*
*   NOTES: 
*  August 3rd, 2008, EEM added heart-beat thread.
*
*****************************************************************************/

#include <windows.h>
#include <mmsystem.h>

#include "sq.h"
#include "sqAssert.h"
#include "sqMemoryFence.h"

/*
 * Win32 FILETIMEs are 10th's of microseconds since 1601.  Smalltalk times
 * are seconds from 1901.  Let's call a 10th of a microsecond a "tock".
 */

#if _MSC_VER
# define SecondsFrom1601To1901 9467020800i64 /*See PRINT_TIME_CONSTANTS below*/
# define MicrosecondsFrom1601To1901 9467020800000000i64

# define MicrosecondsPerSecond 1000000i64
# define MillisecondsPerSecond 1000i64

# define MicrosecondsPerMillisecond 1000000i64

# define TocksPerSecond      10000000i64
# define TocksPerMillisecond 10000i64
# define TocksPerMicrosecond 10i64
# define LLFMT "I64"
#else
# define SecondsFrom1601To1901 9467020800LL /*See PRINT_TIME_CONSTANTS below*/
# define MicrosecondsFrom1601To1901 9467020800000000LL

# define MicrosecondsPerSecond 1000000LL
# define MillisecondsPerSecond 1000LL

# define MicrosecondsPerMillisecond 1000LL

# define TocksPerSecond      10000000LL
# define TocksPerMillisecond 10000LL
# define TocksPerMicrosecond 10LL
# if defined(__MINGW32__)
#	define LLFMT "I64"
# else
#	define LLFMT "ll"
# endif
#endif

static DWORD dwTimerPeriod = 0;
static DWORD timerID = 0;

int ioOldMSecs()
{
  /* Make sure the value fits into Squeak SmallIntegers */
#ifndef _WIN32_WCE
  return timeGetTime() & MillisecondClockMask;
#else
  return GetTickCount() & MillisecondClockMask;
#endif
}

sqLong ioHighResClock(void) {
  sqLong value = 0;
#ifdef __GNUC__
  __asm__ __volatile__ ("rdtsc" : "=A" (value));
#endif
  return value;
}

/* Compute the current VM time basis, the number of microseconds from 1901.
 *
 * Alas Windows' system time functions GetSystemTime et al have low resolution;
 * 15 ms.  So we use timeGetTime for higher resolution and use it as an offset to
 * the system time, resetting when timeGetTime wraps.  Since timeGetTime wraps we
 * need some basis information which is passed in as pointers to provide us with
 * both the heartbeat clock and an instantaneous clock for the VM thread.
 */

static unsigned __int64
currentUTCMicroseconds(unsigned __int64 *utcTickBaseUsecsp, DWORD *lastTickp, DWORD *baseTickp)
{
	FILETIME utcNow;
	DWORD currentTick = timeGetTime();
	DWORD prevTick = *lastTickp;

	*lastTickp = currentTick;

	/* If the timeGetTime millisecond clock wraps (as it will every 49.71 days)
	 * resync to the system time.  
	 */
	if (currentTick < prevTick) {

		*baseTickp = currentTick;
		GetSystemTimeAsFileTime(&utcNow);
		*utcTickBaseUsecsp = *(unsigned __int64 *)&utcNow
							/ TocksPerMicrosecond
							- MicrosecondsFrom1601To1901;
		return *utcTickBaseUsecsp;
	}
	return *utcTickBaseUsecsp
		  + (currentTick - *baseTickp) * MicrosecondsPerMillisecond;
}


/* The 64-bit clocks.  utcMicrosecondClock is utc microseconds from 1901.
 * localMicrosecondClock is local microseconds from 1901.  The 32-bit clock
 * millisecondClock is milliseconds since system start.  These are updated
 * by the heartbeat thread at up to 1KHz.
 */
static unsigned volatile __int64 utcMicrosecondClock;
static unsigned volatile __int64 localMicrosecondClock;
static unsigned volatile long millisecondClock; /* for the ioMSecs clock. */
static unsigned __int64 utcStartMicroseconds;
static   signed __int64 vmGMTOffset = 0;

/* The bases that relate timeGetTime's 32-bit wrapping millisecond clock to the
 * non-wrapping 64-bit microsecond clocks.
 */
static unsigned __int64 utcTickBaseMicroseconds;
static DWORD lastTick = (DWORD)-1;
static DWORD baseTick;
static unsigned __int64 vmThreadUtcTickBaseMicroseconds;
static DWORD vmThreadLastTick = (DWORD)-1;
static DWORD vmThreadBaseTick;

#define microToMilliseconds(usecs) ((((usecs) - utcStartMicroseconds) \
									/ MicrosecondsPerMillisecond) \
									& MillisecondClockMask)

#define LOG_CLOCK 1

#if LOG_CLOCK
# define LOGSIZE 1024
static unsigned __int64 useclog[LOGSIZE];
static unsigned long mseclog[LOGSIZE];
static int logClock = 0;
static unsigned int ulogidx = (unsigned)-1;
static unsigned int mlogidx = (unsigned)-1;
# define logusecs(usecs) do { sqLowLevelMFence(); \
							if (logClock) useclog[++ulogidx % LOGSIZE] = (usecs); \
						} while (0)
# define logmsecs(msecs) do { sqLowLevelMFence(); \
							if (logClock) mseclog[++mlogidx % LOGSIZE] = (msecs); \
						} while (0)
void
ioGetClockLogSizeUsecsIdxMsecsIdx(sqInt *runInNOutp, void **usecsp, sqInt *uip, void **msecsp, sqInt *mip)
{
	logClock = *runInNOutp;
	sqLowLevelMFence();
	*runInNOutp = LOGSIZE;
	*usecsp = useclog;
	*uip = ulogidx % LOGSIZE;
	*msecsp = mseclog;
	*mip = mlogidx % LOGSIZE;
}
#else /* LOG_CLOCK */
# define logusecs(usecs) 0
# define logmsecs(msecs) 0
void
ioGetClockLogSizeUsecsIdxMsecsIdx(sqInt *np, void **usecsp, sqInt *uip, void **msecsp, sqInt *mip)
{
	*np = *uip = *mip = 0;
	*usecsp = *msecsp = 0;
}
#endif /* LOG_CLOCK */

/*
 * Update the utc and local microsecond clocks.  Since this is invoked from
 * a high-priority thread, and since the clocks are 64-bit values that are read
 * concurrently by the VM, care must be taken to access these values atomically
 * on 32-bit systems.  If they are not accessed atomically there is a possibility
 * of fetching the two halves of the clock from different ticks which would
 * cause a jump in the clock of 2^32 microseconds (1 hr, 11 mins, 34 secs).
 *
 * Since an interrupt could occur between any two instructions the clock must be
 * read atomically as well as written atomically.  If possible this can be
 * implemented without locks using atomic 64-bit reads and writes.
 */

#include "sqAtomicOps.h"

static void
updateMicrosecondClock()
{
	unsigned __int64 newUtcMicrosecondClock, newLocalMicrosecondClock;

	newUtcMicrosecondClock = currentUTCMicroseconds(&utcTickBaseMicroseconds,
													&lastTick,
													&baseTick);
	/* The native clock may go backwards, e.g. when timeGetTime wraps and we sync
	 * with the system clock, which, having only 15ms resolution, could cause a
	 * backward step.  Of course this will cause problems if the clock is manually
	 * adjusted.  To which the doctor says, "don't do that".
	 */
	if (!asserta(newUtcMicrosecondClock >= utcMicrosecondClock)) {
		logusecs(0); /* if logging log a backward step as 0 */
		return;
	}
	newLocalMicrosecondClock = newUtcMicrosecondClock + vmGMTOffset;

	set64(utcMicrosecondClock,newUtcMicrosecondClock);
	set64(localMicrosecondClock,newLocalMicrosecondClock);
	millisecondClock = microToMilliseconds(newUtcMicrosecondClock);

	logusecs(newUtcMicrosecondClock);
	logmsecs(millisecondClock);
}

void
ioUpdateVMTimezone()
{
	__int64 utcNow, localNow;

	GetSystemTimeAsFileTime((FILETIME *)&utcNow);
	FileTimeToLocalFileTime((FILETIME *)&utcNow,(FILETIME *)&localNow);
	vmGMTOffset = (localNow - utcNow) / (__int64)TocksPerMicrosecond;
}

void
ioInitTime(void)
{
	ioUpdateVMTimezone();
	updateMicrosecondClock(); /* this can now compute localUTCMicroseconds */
	utcStartMicroseconds = utcMicrosecondClock;
}

usqLong
ioUTCMicroseconds() { return get64(utcMicrosecondClock); }

usqLong
ioLocalMicroseconds() { return get64(localMicrosecondClock); }

/* This is an expensive interface for use by profiling code that wants the time
 * now rather than as of the last heartbeat.
 */
usqLong
ioUTCMicrosecondsNow()
{
	return currentUTCMicroseconds(&vmThreadUtcTickBaseMicroseconds,
								  &vmThreadLastTick,
								  &vmThreadBaseTick);
}

int
ioMSecs() { return millisecondClock; }

/* Note: ioMicroMSecs returns *milli*seconds */
int
ioMicroMSecs(void) { return microToMilliseconds(ioUTCMicrosecondsNow()); }

/* returns the local wall clock time */
int
ioSeconds(void) { return get64(localMicrosecondClock) / MicrosecondsPerSecond; }

int
ioUTCSeconds(void) { return get64(utcMicrosecondClock) / MicrosecondsPerSecond; }


typedef enum { dead, condemned, nascent, quiescent, active } machine_state;

static machine_state beatThreadState = nascent;

#if !defined(DEFAULT_BEAT_MS)
# define DEFAULT_BEAT_MS 1
#endif
static long beatMilliseconds = DEFAULT_BEAT_MS;
static HANDLE beatSemaphore;
static HANDLE heartbeatThread;
static unsigned __int64 frequencyMeasureStart;
static unsigned long heartbeats;
static usqLong deadline;

static usqLong
heartbeat()
{
	usqLong nextDeadline;
	updateMicrosecondClock();
	if (get64(frequencyMeasureStart) == 0) {
		set64(frequencyMeasureStart,utcMicrosecondClock);
		heartbeats = 0;
	}
	else
		heartbeats += 1;
	nextDeadline = checkHighPriorityTickees(utcMicrosecondClock);
	forceInterruptCheckFromHeartbeat();
	/* If the next deadline is not infinitely far in the future then a tickee
	 * actually ran and used some time and we should update the clock.
	 */
	if (nextDeadline != -1ULL)
		updateMicrosecondClock();
	return nextDeadline;
}

static DWORD WINAPI
beatThreadStateMachine(void *careLess)
{
	usqLong deadline;
	beatThreadState = active;
	updateMicrosecondClock();
	deadline = utcMicrosecondClock;  
	while (beatThreadState != condemned) {
		if (deadline <= utcMicrosecondClock && beatThreadState == active) {
			/* No need to wait; we're already at or past the deadline. */
			deadline = heartbeat();
		}
		else {
			usqLong waitTime = (deadline - utcMicrosecondClock) / 1000LL;
			/* Need to wait. */
			if (beatThreadState != active) waitTime = INFINITE;
			else if (waitTime > beatMilliseconds) waitTime = beatMilliseconds;
			DWORD res = WaitForSingleObject(beatSemaphore, (DWORD)waitTime);	
			if (res == WAIT_TIMEOUT
#if !defined(_WIN32_WCE) // for pulsing by timeSetEvent below
				|| res == WAIT_OBJECT_0
#endif
			)
				deadline = heartbeat();
			else if (res == WAIT_FAILED)
				abortMessage(TEXT("Fatal: WaitForSingleObject(beatSemaphore) %ld"),
							GetLastError());
			else
				printLastError(TEXT("beatThreadStateMachine WaitForSingleObject"));
		}
	}
	beatThreadState = dead;
	return 0;
}

/* Answer the average heartbeats per second since the stats were last reset.
 */
unsigned long
ioHeartbeatFrequency(int reset)
{
	unsigned long duration = (ioUTCMicroseconds() - get64(frequencyMeasureStart))
							/ MicrosecondsPerSecond;
	unsigned frequency = duration ? heartbeats / duration : 0;

	if (reset) {
		unsigned __int64 zero = 0;
		set64(frequencyMeasureStart,zero);
	}
	return frequency;
}

/*
 * If we're using just a poll thread (current default) then it also decides
 * the delay resolution and so we choose THREAD_PRIORITY_TIME_CRITICAL.
 */
void
ioInitHeartbeat()
{
	DWORD uselessThreadId;
extern sqInt suppressHeartbeatFlag;

	if (suppressHeartbeatFlag) return;

	beatSemaphore = CreateSemaphore(NULL,	/*no security*/
									0,		/*no initial signals*/
									65535,  /*no limit on num signals*/
									NULL	/*anonymous*/);
	heartbeatThread = CreateThread(0,	/* default security attributes (none) */
							  2048, /* thread stack bytes (ignored!! read it and weep) */
							  beatThreadStateMachine,
							  0,	/* beatThreadStateMachine argument */
							  0 | STACK_SIZE_PARAM_IS_A_RESERVATION,	/* creation flags, 0 => run immediately, SSPIAR implies don't commit memory to stack */
							  &uselessThreadId);
	if (!heartbeatThread
	 || !SetThreadPriority(heartbeatThread, POLL_THREAD_PRIORITY))
		abortMessage(TEXT("Fatal error: poll thread init failure %ld"), GetLastError());

	while (beatThreadState == nascent)
		if (WaitForSingleObject(beatSemaphore, 1) == WAIT_FAILED)
			abortMessage(TEXT("Fatal: WaitForSingleObject(beatSemaphore) %ld"),
						 GetLastError());

	ioSetHeartbeatMilliseconds(beatMilliseconds);
}

int
ioHeartbeatMilliseconds() { return beatMilliseconds; }

void
ioSetHeartbeatMilliseconds(int ms)
{
	if (ms != beatMilliseconds) {
		beatMilliseconds = ms;
		/*
		 * We only break the poll state machine out of its loop if it is
		 * inactive.  Otherwise the I/O poll frequency will only change when
		 * the current wait finishes.  This is consistent with the Unix/Mac OS
		 * platform which cannot break out of its nanosleep.  If you change
		 * this you should change sqUnixHeartbeat.c to match.
		 */
		if (beatThreadState != active) {
			beatThreadState = active;
			if (!ReleaseSemaphore(beatSemaphore,1 /* 1 signal */,NULL))
				abortMessage(TEXT("Fatal: ReleaseSemaphore(beatSemaphore) %ld"),
							 GetLastError());
		}
	}
#if !defined(_WIN32_WCE)
	/* Belt and braces.  Use timeSetEvent to signal beatSemaphore periodically
	 * to avoid cases where Windows doesn't honour the timeout in a timely
	 * fashion in the above WaitForSingleObject.
	 */
	if (dwTimerPeriod != ms) {
		void ioReleaseTime(void);
		TIMECAPS tCaps;

		ioReleaseTime();
		dwTimerPeriod = 0;
		if(timeGetDevCaps(&tCaps,sizeof(tCaps)) != 0)
			return;
		dwTimerPeriod = max(tCaps.wPeriodMin,beatMilliseconds);
		if(timeBeginPeriod(dwTimerPeriod) != 0)
			return;
		timerID = timeSetEvent(	dwTimerPeriod,
								0,
								(LPTIMECALLBACK)beatSemaphore,
								0,
								TIME_PERIODIC |
								TIME_CALLBACK_EVENT_PULSE);
	}
#endif /* defined(_WIN32_WCE) */
}

void
ioReleaseTime(void)
{
#if !defined(_WIN32_WCE)
	if (timerID) {
		timeKillEvent(timerID);
		timeEndPeriod(dwTimerPeriod);
		timerID = 0;
	}
#endif /* !defined(_WIN32_WCE) */
}


#if PRINT_TIME_CONSTANTS
SYSTEMTIME StartOfThe20thCentury
        = { 1901,	/* Blame Aloysius Lilius? */
            1,      /* January */
            2,      /* (Date fromSeconds: 0) dayOfWeekName => #Tuesday */
            1,      /* 1st of January */
            0,      /* 0th Hour */
            0,      /* 0th minute */
            0,      /* 0th second */
            0 };    /* 0th millisecond */
int
main()
{
	FILETIME startOfThe20thCentury;
	TCHAR buffer[1024];
	FILETIME utcNow, localNow;

	GetDateFormat(LOCALE_SYSTEM_DEFAULT,
				  DATE_LONGDATE,
				  &StartOfThe20thCentury,
				  0,
				  buffer,
				  1024);
	printf("StartOfThe20thCentury is %s\n\n", buffer);

	SystemTimeToFileTime(&StartOfThe20thCentury, &startOfThe20thCentury);
	printf("#define MicrosecondsFrom1601To1901 %"LLFMT"d\n",
			*(unsigned __int64 *)&startOfThe20thCentury / TocksPerMicrosecond);
	printf("#define SecondsFrom1601To1901 %"LLFMT"d\n\n",
			*(unsigned __int64 *)&startOfThe20thCentury / TocksPerSecond);
	GetSystemTimeAsFileTime(&utcNow);
	FileTimeToLocalFileTime(&utcNow,&localNow);
	printf("Now Smalltalk seconds is therefore %"LLFMT"d\n",
			(*(unsigned __int64 *)&localNow / TocksPerSecond)
			- (*(unsigned __int64 *)&startOfThe20thCentury / TocksPerSecond));
}

sqInt
forceInterruptCheckFromHeartbeat()
{
}

int __cdecl
abortMessage(const TCHAR *fmt,...)
{
}
#endif /* PRINT_TIME_CONSTANTS */
