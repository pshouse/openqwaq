/****************************************************************************
*   PROJECT: Unix (pthread but /not/ linux) heartbeat logic for Stack/Cog VM
*   FILE:    sqUnixHeartbeat.c
*   CONTENT: 
*
*   AUTHOR:  Eliot Miranda
*   ADDRESS: 
*   EMAIL:   eliot@qwaq.com
*   RCSID:   $Id$
*
*   NOTES: 
*  July 31st, 2008, EEM added heart-beat thread.
*  Aug  20th, 2009, EEM added 64-bit microsecond clock support code
*  Sep  17th, 2010, EEM sp(l)it itimer heartbeat into sqLinuxHeartbeat.c
*
*****************************************************************************/

#include "sq.h"
#include "sqAssert.h"
#include "sqMemoryFence.h"
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#define SecondsFrom1901To1970      2177452800ULL
#define MicrosecondsFrom1901To1970 2177452800000000ULL

#define MicrosecondsPerSecond 1000000ULL
#define MillisecondsPerSecond 1000ULL

#define MicrosecondsPerMillisecond 1000ULL

static unsigned volatile long long utcMicrosecondClock;
static unsigned volatile long long localMicrosecondClock;
static unsigned volatile long millisecondClock; /* for the ioMSecs clock. */
static unsigned long long utcStartMicroseconds; /* for the ioMSecs clock. */
static long long vmGMTOffset = 0;
static unsigned long long frequencyMeasureStart = 0;
static unsigned long heartbeats;

#define microToMilliseconds(usecs) ((((usecs) - utcStartMicroseconds) \
									/ MicrosecondsPerMillisecond) \
									& MillisecondClockMask)

#define LOG_CLOCK 1

#if LOG_CLOCK
# define LOGSIZE 1024
static unsigned long long useclog[LOGSIZE];
static unsigned long mseclog[LOGSIZE];
static int logClock = 0;
static unsigned int ulogidx = (unsigned int)-1;
static unsigned int mlogidx = (unsigned int)-1;
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

/* Compute the current VM time basis, the number of microseconds from 1901. */

static unsigned long long
currentUTCMicroseconds()
{
	struct timeval utcNow;

	gettimeofday(&utcNow,0);
	return ((utcNow.tv_sec * MicrosecondsPerSecond) + utcNow.tv_usec)
			+ MicrosecondsFrom1901To1970;
}

/* Update the utc and local microsecond clocks, and the millisecond clock.
 * Since this is invoked from interupt code, and since the clocks are 64-bit
 * values that are read concurrently by the VM, care must be taken to access
 * these values atomically on 32-bit systems.  If they are not accessed atomic-
 * ally there is a possibility of fetching the two halves of the clock from
 * different ticks which would cause a jump in the clock of 2^32 microseconds
 * (1 hr, 11 mins, 34 secs).
 *
 * Since an interrupt could occur between any two instructions the clock must be
 * read atomically as well as written atomically.  If possible this can be
 * implemented without locks using atomic 64-bit reads and writes.
 */

#include "sqAtomicOps.h"

static void
updateMicrosecondClock()
{
	unsigned long long newUtcMicrosecondClock;
	unsigned long long newLocalMicrosecondClock;

	newUtcMicrosecondClock = currentUTCMicroseconds();

	/* The native clock may go backwards, e.g. due to NTP adjustments, although
	 * why it can't avoid small backward steps itself, I don't know.  Simply
	 * ignore backward steps and wait until the clock catches up again.  Of
	 * course this will cause problems if the clock is manually adjusted.  To
	 * which the doctor says, "don't do that".
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
	time_t utctt;
	updateMicrosecondClock();
	utctt = (get64(utcMicrosecondClock) - MicrosecondsFrom1901To1970)
				/ MicrosecondsPerSecond;
	vmGMTOffset = localtime(&utctt)->tm_gmtoff * MicrosecondsPerSecond;
}

sqLong
ioHighResClock(void)
{
  /* return the value of the high performance counter */
  sqLong value = 0;
#if defined(__GNUC__) && ( defined(i386) || defined(__i386) || defined(__i386__)  \
			|| defined(i486) || defined(__i486) || defined (__i486__) \
			|| defined(intel) || defined(x86) || defined(i86pc) )
    __asm__ __volatile__ ("rdtsc" : "=A"(value));
#else
# error "no high res clock defined"
#endif
  return value;
}

#if !macintoshSqueak
static unsigned int   lowResMSecs= 0;
static struct timeval startUpTime;

/*
 * Answer the millisecond clock as computed on Unix prior to the 64-bit
 * microsecond clock.  This is to help verify that the new clock is correct.
 */
sqInt
ioOldMSecs(void)
{
  struct timeval now;
  unsigned int nowMSecs;

#if 1 /* HAVE_HIGHRES_COUNTER */

  /* if we have a cheap, high-res counter use that to limit
     the frequency of calls to gettimeofday to something reasonable. */
  static unsigned int baseMSecs = 0;      /* msecs when we took base tick */
  static sqLong baseTicks = 0;/* base tick for adjustment */
  static sqLong tickDelta = 0;/* ticks / msec */
  static sqLong nextTick = 0; /* next tick to check gettimeofday */

  sqLong thisTick = ioHighResClock();

  if(thisTick < nextTick) return lowResMSecs;

#endif

  gettimeofday(&now, 0);
  if ((now.tv_usec-= startUpTime.tv_usec) < 0)
    {
      now.tv_usec+= 1000000;
      now.tv_sec-= 1;
    }
  now.tv_sec-= startUpTime.tv_sec;
  nowMSecs = (now.tv_usec / 1000 + now.tv_sec * 1000);

#if 1 /* HAVE_HIGHRES_COUNTER */
  {
    unsigned int msecsDelta;
    /* Adjust our rdtsc rate every 10...100 msecs as needed.
       This also covers msecs clock-wraparound. */
    msecsDelta = nowMSecs - baseMSecs;
    if(msecsDelta < 0 || msecsDelta > 100) {
      /* Either we've hit a clock-wraparound or we are being
	 sampled in intervals larger than 100msecs.
	 Don't try any fancy adjustments */
      baseMSecs = nowMSecs;
      baseTicks = thisTick;
      nextTick = 0;
      tickDelta = 0;
    } else if(msecsDelta >= 10) {
      /* limit the rate of adjustments to 10msecs */
      baseMSecs = nowMSecs;
      tickDelta = (thisTick - baseTicks) / msecsDelta;
      nextTick = baseTicks = thisTick;
    }
    nextTick += tickDelta;
  }
#endif
  return lowResMSecs= nowMSecs;
}
#endif /* !macintoshSqueak */

usqLong
ioUTCMicroseconds() { return get64(utcMicrosecondClock); }

usqLong
ioLocalMicroseconds() { return get64(localMicrosecondClock); }

/* This is an expensive interface for use by profiling code that wants the time
 * now rather than as of the last heartbeat.
 */
usqLong
ioUTCMicrosecondsNow() { return currentUTCMicroseconds(); }

int
ioMSecs() { return millisecondClock; }

/* Note: ioMicroMSecs returns *milli*seconds */
int ioMicroMSecs(void) { return microToMilliseconds(currentUTCMicroseconds()); }

/* returns the local wall clock time */
int
ioSeconds(void) { return get64(localMicrosecondClock) / MicrosecondsPerSecond; }

int
ioUTCSeconds(void) { return get64(utcMicrosecondClock) / MicrosecondsPerSecond; }

/*
 * On Mac OS X use the following.
 * On Unix use dpy->ioRelinquishProcessorForMicroseconds
 */
#if macintoshSqueak
int
ioRelinquishProcessorForMicroseconds(int microSeconds)
{
    long	realTimeToWait;
	extern usqLong getNextWakeupUsecs();
	usqLong nextWakeupUsecs = getNextWakeupUsecs();
	usqLong utcNow = get64(utcMicrosecondClock);

    if (nextWakeupUsecs <= utcNow) {
		/* if nextWakeupUsecs is non-zero the next wakeup time has already
		 * passed and we should not wait.
		 */
        if (nextWakeupUsecs != 0)
			return 0;
		realTimeToWait = microSeconds;
    }
    else {
        realTimeToWait = nextWakeupUsecs - utcNow;
		if (realTimeToWait > microSeconds)
			realTimeToWait = microSeconds;
	}

	aioSleepForUsecs(realTimeToWait);

	return 0;
}
#endif /* !macintoshSqueak */

void
ioInitTime(void)
{
	ioUpdateVMTimezone(); /* does updateMicrosecondClock as a side-effect */
	updateMicrosecondClock(); /* this can now compute localUTCMicroseconds */
	utcStartMicroseconds = utcMicrosecondClock;
#if !macintoshSqueak
	/* This is only needed for ioOldMSecs */
	gettimeofday(&startUpTime, 0);
#endif
}

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

typedef enum { dead, condemned, nascent, quiescent, active } machine_state;

static int					stateMachinePolicy;
static struct sched_param	stateMachinePriority;

static machine_state beatState = nascent;

#if !defined(DEFAULT_BEAT_MS)
# define DEFAULT_BEAT_MS 2
#endif
static int beatMilliseconds = DEFAULT_BEAT_MS;
static struct timespec beatperiod = { 0, DEFAULT_BEAT_MS * 1000 * 1000 };

static void *
beatThreadStateMachine(void *careLess)
{
	usqLong deadline;
	int er;
	if ((er = pthread_setschedparam(pthread_self(),
									stateMachinePolicy,
									&stateMachinePriority))) {
		errno = er;
		perror("pthread_setschedparam failed; consider using ITIMER_HEARTBEAT");
		exit(errno);
	}
	beatState = active;
	updateMicrosecondClock();
	deadline = utcMicrosecondClock;
	while (beatState != condemned) {
		if (deadline > utcMicrosecondClock) {
			struct timespec naptime;
			assert(!beatperiod.tv_sec);
			naptime.tv_sec = 0;
			if (deadline == -1ULL)
				naptime.tv_nsec = beatperiod.tv_nsec;
			else {
				usqLong nsToDeadline = (deadline - utcMicrosecondClock) * 1000;
				naptime.tv_nsec = min(beatperiod.tv_nsec, nsToDeadline);
			}
#	define MINSLEEPNS 2000 /* don't bother sleeping for short times */
			while (nanosleep(&naptime, &naptime) == -1
				&& naptime.tv_sec >= 0 /* oversleeps can return tv_sec < 0 */
				&& (naptime.tv_sec > 0 || naptime.tv_nsec > MINSLEEPNS))
				if (errno != EINTR) {
					perror("nanosleep");
					exit(1);
				}
		}
		deadline = heartbeat();
	}
	beatState = dead;
	return 0;
}

void
ioInitHeartbeat()
{
	int er;
	struct timespec halfAMo;
	pthread_t careLess;
extern sqInt suppressHeartbeatFlag;

	if (suppressHeartbeatFlag) return;

	if ((er = pthread_getschedparam(pthread_self(),
									&stateMachinePolicy,
									&stateMachinePriority))) {
		errno = er;
		perror("pthread_getschedparam failed");
		exit(errno);
	}
	++stateMachinePriority.sched_priority;
	halfAMo.tv_sec  = 0;
	halfAMo.tv_nsec = 1000 * 100;
	if ((er= pthread_create(&careLess,
							(const pthread_attr_t *)0,
							beatThreadStateMachine,
							0))) {
		errno = er;
		perror("beat thread creation failed");
		exit(errno);
	}
	while (beatState == nascent)
		nanosleep(&halfAMo, 0);
}

void
ioSetHeartbeatMilliseconds(int ms)
{
	beatMilliseconds = ms;
	beatperiod.tv_sec = beatMilliseconds / 1000;
	beatperiod.tv_nsec = (beatMilliseconds % 1000) * 1000 * 1000;
}

int
ioHeartbeatMilliseconds() { return beatMilliseconds; }


/* Answer the average heartbeats per second since the stats were last reset.
 */
unsigned long
ioHeartbeatFrequency(int resetStats)
{
	unsigned duration = (ioUTCMicroseconds() - get64(frequencyMeasureStart))
						/ MicrosecondsPerSecond;
	unsigned frequency = duration ? heartbeats / duration : 0;

	if (resetStats) {
		unsigned long long zero = 0;
		set64(frequencyMeasureStart,zero);
	}
	return frequency;
}
