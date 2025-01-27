#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>

#include "vmCallback.h"
#include "sqVirtualMachine.h"


/*** Function prototypes ***/

/* InterpreterProxy methodsFor: 'stack access' */
sqInt  pop(sqInt nItems);
sqInt  popthenPush(sqInt nItems, sqInt oop);
sqInt  push(sqInt object);
sqInt  pushBool(sqInt trueOrFalse);
sqInt  pushFloat(double f);
sqInt  pushInteger(sqInt integerValue);
double stackFloatValue(sqInt offset);
sqInt  stackIntegerValue(sqInt offset);
sqInt  stackObjectValue(sqInt offset);
sqInt  stackValue(sqInt offset);

/*** variables ***/

/* InterpreterProxy methodsFor: 'object access' */
sqInt  argumentCountOf(sqInt methodPointer);
void  *arrayValueOf(sqInt oop);
sqInt  byteSizeOf(sqInt oop);
void  *fetchArrayofObject(sqInt fieldIndex, sqInt objectPointer);
sqInt  fetchClassOf(sqInt oop);
double fetchFloatofObject(sqInt fieldIndex, sqInt objectPointer);
sqInt  fetchIntegerofObject(sqInt fieldIndex, sqInt objectPointer);
sqInt  fetchPointerofObject(sqInt index, sqInt oop);
/* sqInt  fetchWordofObject(sqInt fieldIndex, sqInt oop);     *
 * has been rescinded as of VMMaker 3.8 and the 64bitclean VM *
 * work. To support old plugins we keep a valid function in   *
 * the same location in the VM struct but rename it to        *
 * something utterly horrible to scare off the natives. A new *
 * equivalent but 64 bit valid function is added as           *
 * 'fetchLong32OfObject'                                      */
sqInt  obsoleteDontUseThisFetchWordofObject(sqInt index, sqInt oop);
sqInt  fetchLong32ofObject(sqInt index, sqInt oop); 
void  *firstFixedField(sqInt oop);
void  *firstIndexableField(sqInt oop);
sqInt  literalofMethod(sqInt offset, sqInt methodPointer);
sqInt  literalCountOf(sqInt methodPointer);
sqInt  methodArgumentCount(void);
sqInt  methodPrimitiveIndex(void);
sqInt  primitiveMethod(void);
sqInt  primitiveIndexOf(sqInt methodPointer);
sqInt  sizeOfSTArrayFromCPrimitive(void *cPtr);
sqInt  slotSizeOf(sqInt oop);
sqInt  stObjectat(sqInt array, sqInt index);
sqInt  stObjectatput(sqInt array, sqInt index, sqInt value);
sqInt  stSizeOf(sqInt oop);
sqInt  storeIntegerofObjectwithValue(sqInt index, sqInt oop, sqInt integer);
sqInt  storePointerofObjectwithValue(sqInt index, sqInt oop, sqInt valuePointer);


/* InterpreterProxy methodsFor: 'testing' */
sqInt isKindOf(sqInt oop, char *aString);
sqInt isMemberOf(sqInt oop, char *aString);
sqInt isBytes(sqInt oop);
sqInt isFloatObject(sqInt oop);
sqInt isIndexable(sqInt oop);
sqInt isIntegerObject(sqInt objectPointer);
sqInt isIntegerValue(sqInt intValue);
sqInt isPointers(sqInt oop);
sqInt isWeak(sqInt oop);
sqInt isWords(sqInt oop);
sqInt isWordsOrBytes(sqInt oop);
sqInt includesBehaviorThatOf(sqInt aClass, sqInt aSuperClass);
#if VM_PROXY_MINOR > 10
sqInt isKindOfClass(sqInt oop, sqInt aClass);
sqInt primitiveErrorTable(void);
sqInt primitiveFailureCode(void);
sqInt instanceSizeOf(sqInt aClass);
sqInt tenuringIncrementalGC(void);
#endif
sqInt isArray(sqInt oop);
#if IMMUTABILITY
sqInt internalIsMutable(sqInt oop);
sqInt internalIsImmutable(sqInt oop);
#endif

/* InterpreterProxy methodsFor: 'converting' */
sqInt  booleanValueOf(sqInt obj);
sqInt  checkedIntegerValueOf(sqInt intOop);
sqInt  floatObjectOf(double aFloat);
double floatValueOf(sqInt oop);
sqInt  integerObjectOf(sqInt value);
sqInt  integerValueOf(sqInt oop);
sqInt  positive32BitIntegerFor(sqInt integerValue);
sqInt  positive32BitValueOf(sqInt oop);
sqInt  signed32BitIntegerFor(sqInt integerValue);
sqInt  signed32BitValueOf(sqInt oop);
sqInt  positive64BitIntegerFor(sqLong integerValue);
sqLong positive64BitValueOf(sqInt oop);
sqInt  signed64BitIntegerFor(sqLong integerValue);
sqLong signed64BitValueOf(sqInt oop);
long  signedMachineIntegerValueOf(sqInt);
long  stackSignedMachineIntegerValue(sqInt);
unsigned long  positiveMachineIntegerValueOf(sqInt);
unsigned long  stackPositiveMachineIntegerValue(sqInt);

/* InterpreterProxy methodsFor: 'special objects' */
sqInt characterTable(void);
sqInt displayObject(void);
sqInt falseObject(void);
sqInt nilObject(void);
sqInt trueObject(void);


/* InterpreterProxy methodsFor: 'special classes' */
sqInt classArray(void);
sqInt classBitmap(void);
sqInt classByteArray(void);
sqInt classCharacter(void);
sqInt classFloat(void);
sqInt classLargePositiveInteger(void);
sqInt classLargeNegativeInteger(void);
sqInt classPoint(void);
sqInt classSemaphore(void);
sqInt classSmallInteger(void);
sqInt classString(void);


/* InterpreterProxy methodsFor: 'instance creation' */
sqInt clone(sqInt oop);
sqInt instantiateClassindexableSize(sqInt classPointer, sqInt size);
sqInt makePointwithxValueyValue(sqInt xValue, sqInt yValue);
sqInt popRemappableOop(void);
sqInt pushRemappableOop(sqInt oop);


/* InterpreterProxy methodsFor: 'other' */
sqInt becomewith(sqInt array1, sqInt array2);
sqInt byteSwapped(sqInt w);
sqInt failed(void);
sqInt fullDisplayUpdate(void);
sqInt fullGC(void);
sqInt incrementalGC(void);
sqInt primitiveFail(void);
sqInt primitiveFailFor(sqInt reasonCode);
sqInt showDisplayBitsLeftTopRightBottom(sqInt aForm, sqInt l, sqInt t, sqInt r, sqInt b);
sqInt signalSemaphoreWithIndex(sqInt semaIndex);
sqInt success(sqInt aBoolean);
sqInt superclassOf(sqInt classPointer);
sqInt ioMicroMSecs(void);
usqLong ioUTCMicroseconds(void);
sqInt forceInterruptCheck(void);
sqInt getThisSessionID(void);
sqInt ioFilenamefromStringofLengthresolveAliases(char* aCharBuffer, char* filenameIndex, sqInt filenameLength, sqInt resolveFlag);
sqInt vmEndianness(void);	
sqInt getInterruptPending(void);

/* InterpreterProxy methodsFor: 'BitBlt support' */
sqInt loadBitBltFrom(sqInt bbOop);
sqInt copyBits(void);
sqInt copyBitsFromtoat(sqInt leftX, sqInt rightX, sqInt yValue);

/* InterpreterProxy methodsFor: 'FFI support' */
sqInt classExternalAddress(void);
sqInt classExternalData(void);
sqInt classExternalFunction(void);
sqInt classExternalLibrary(void);
sqInt classExternalStructure(void);
sqInt ioLoadModuleOfLength(sqInt moduleNameIndex, sqInt moduleNameLength);
sqInt ioLoadSymbolOfLengthFromModule(sqInt functionNameIndex, sqInt functionNameLength, sqInt moduleHandle);
sqInt isInMemory(sqInt address);
sqInt classAlien(void); /* Alien FFI */
sqInt classUnsafeAlien(void); /* Alien FFI */
void *startOfAlienData(sqInt);
usqInt sizeOfAlienData(sqInt);
sqInt getStackPointer(void);  /* Alien FFI */
sqInt sendInvokeCallbackStackRegistersJmpbuf(sqInt thunkPtrAsInt, sqInt stackPtrAsInt, sqInt regsPtrAsInt, sqInt jmpBufPtrAsInt); /* Alien FFI */
sqInt reestablishContextPriorToCallback(sqInt callbackContext); /* Alien FFI */
sqInt sendInvokeCallbackContext(VMCallbackContext *);
sqInt returnAsThroughCallbackContext(int, VMCallbackContext *, sqInt);
char *cStringOrNullFor(sqInt);

void *ioLoadFunctionFrom(char *fnName, char *modName);


/* Proxy declarations for v1.8 */
sqInt callbackEnter(sqInt *callbackID);
sqInt callbackLeave(sqInt  callbackID);
sqInt addGCRoot(sqInt *varLoc);
sqInt removeGCRoot(sqInt *varLoc);

/* Proxy declarations for v1.10 */
sqInt methodArg(sqInt index);
sqInt objectArg(sqInt index);
sqInt integerArg(sqInt index);
double floatArg(sqInt index);
sqInt methodReturnValue(sqInt oop);
sqInt topRemappableOop(void);

struct VirtualMachine *VM = NULL;

static sqInt majorVersion(void) {
	return VM_PROXY_MAJOR;
}

static sqInt minorVersion(void) {
	return VM_PROXY_MINOR;
}

#if !IMMUTABILITY
static sqInt isNonIntegerObject(sqInt objectPointer)
{
	return !isIntegerObject(objectPointer);
}
#endif

#if STACKVM
void (*setInterruptCheckChain(void (*aFunction)(void)))();
#else
void (*setInterruptCheckChain(void (*aFunction)(void)))() { return 0; }
#endif

#if COGMTVM
sqInt disownVM(sqInt flags);
sqInt ownVM(sqInt threadIdAndFlags);
#else
sqInt disownVM(sqInt flags) { return 1; }
sqInt ownVM(sqInt threadIdAndFlags)
{
	extern sqInt amInVMThread(void);
	return amInVMThread() ? 0 : -1;
}
#endif
extern sqInt isYoung(sqInt);

/* High-priority and synchronous ticker function support. */
void addHighPriorityTickee(void (*ticker)(void), unsigned periodms);
void addSynchronousTickee(void (*ticker)(void), unsigned periodms, unsigned roundms);

struct VirtualMachine* sqGetInterpreterProxy(void)
{
	if(VM) return VM;
	VM = (struct VirtualMachine *)calloc(1, sizeof(VirtualMachine));
	/* Initialize Function pointers */
	VM->majorVersion = majorVersion;
	VM->minorVersion = minorVersion;

	/* InterpreterProxy methodsFor: 'stack access' */
	VM->pop = pop;
	VM->popthenPush = popthenPush;
	VM->push = push;
	VM->pushBool = pushBool;
	VM->pushFloat = pushFloat;
	VM->pushInteger = pushInteger;
	VM->stackFloatValue = stackFloatValue;
	VM->stackIntegerValue = stackIntegerValue;
	VM->stackObjectValue = stackObjectValue;
	VM->stackValue = stackValue;

	/* InterpreterProxy methodsFor: 'object access' */
	VM->argumentCountOf = argumentCountOf;
	VM->arrayValueOf = arrayValueOf;
	VM->byteSizeOf = byteSizeOf;
	VM->fetchArrayofObject = fetchArrayofObject;
	VM->fetchClassOf = fetchClassOf;
	VM->fetchFloatofObject = fetchFloatofObject;
	VM->fetchIntegerofObject = fetchIntegerofObject;
	VM->fetchPointerofObject = fetchPointerofObject;
	VM->obsoleteDontUseThisFetchWordofObject = obsoleteDontUseThisFetchWordofObject;
	VM->firstFixedField = firstFixedField;
	VM->firstIndexableField = firstIndexableField;
	VM->literalofMethod = literalofMethod;
	VM->literalCountOf = literalCountOf;
	VM->methodArgumentCount = methodArgumentCount;
	VM->methodPrimitiveIndex = methodPrimitiveIndex;
	VM->primitiveIndexOf = primitiveIndexOf;
	VM->primitiveMethod = primitiveMethod;
	VM->sizeOfSTArrayFromCPrimitive = sizeOfSTArrayFromCPrimitive;
	VM->slotSizeOf = slotSizeOf;
	VM->stObjectat = stObjectat;
	VM->stObjectatput = stObjectatput;
	VM->stSizeOf = stSizeOf;
	VM->storeIntegerofObjectwithValue = storeIntegerofObjectwithValue;
	VM->storePointerofObjectwithValue = storePointerofObjectwithValue;

	/* InterpreterProxy methodsFor: 'testing' */
	VM->isKindOf = isKindOf;
	VM->isMemberOf = isMemberOf;
	VM->isBytes = isBytes;
	VM->isFloatObject = isFloatObject;
	VM->isIndexable = isIndexable;
	VM->isIntegerObject = isIntegerObject;
	VM->isIntegerValue = isIntegerValue;
	VM->isPointers = isPointers;
	VM->isWeak = isWeak;
	VM->isWords = isWords;
	VM->isWordsOrBytes = isWordsOrBytes;

	/* InterpreterProxy methodsFor: 'converting' */
	VM->booleanValueOf = booleanValueOf;
	VM->checkedIntegerValueOf = checkedIntegerValueOf;
	VM->floatObjectOf = floatObjectOf;
	VM->floatValueOf = floatValueOf;
	VM->integerObjectOf = integerObjectOf;
	VM->integerValueOf = integerValueOf;
	VM->positive32BitIntegerFor = positive32BitIntegerFor;
	VM->positive32BitValueOf = positive32BitValueOf;

	/* InterpreterProxy methodsFor: 'special objects' */
	VM->characterTable = characterTable;
	VM->displayObject = displayObject;
	VM->falseObject = falseObject;
	VM->nilObject = nilObject;
	VM->trueObject = trueObject;
	
	/* InterpreterProxy methodsFor: 'special classes' */
	VM->classArray = classArray;
	VM->classBitmap = classBitmap;
	VM->classByteArray = classByteArray;
	VM->classCharacter = classCharacter;
	VM->classFloat = classFloat;
	VM->classLargePositiveInteger = classLargePositiveInteger;
	VM->classPoint = classPoint;
	VM->classSemaphore = classSemaphore;
	VM->classSmallInteger = classSmallInteger;
	VM->classString = classString;

	/* InterpreterProxy methodsFor: 'instance creation' */
	VM->clone = clone;
	VM->instantiateClassindexableSize = instantiateClassindexableSize;
	VM->makePointwithxValueyValue = makePointwithxValueyValue;
	VM->popRemappableOop = popRemappableOop;
	VM->pushRemappableOop = pushRemappableOop;

	/* InterpreterProxy methodsFor: 'other' */
	VM->becomewith = becomewith;
	VM->byteSwapped = byteSwapped;
	VM->failed = failed;
	VM->fullDisplayUpdate = fullDisplayUpdate;
	VM->fullGC = fullGC;
	VM->incrementalGC = incrementalGC;
	VM->primitiveFail = primitiveFail;
	VM->showDisplayBitsLeftTopRightBottom = showDisplayBitsLeftTopRightBottom;
	VM->signalSemaphoreWithIndex = signalSemaphoreWithIndex;
	VM->success = success;
	VM->superclassOf = superclassOf;

	VM->compilerHookVector= 0;
	VM->setCompilerInitialized= 0;

#if VM_PROXY_MINOR > 1

	/* InterpreterProxy methodsFor: 'BitBlt support' */
	VM->loadBitBltFrom = loadBitBltFrom;
	VM->copyBits = copyBits;
	VM->copyBitsFromtoat = copyBitsFromtoat;

#endif

#if VM_PROXY_MINOR > 2

	/* InterpreterProxy methodsFor: 'FFI support' */
	VM->classExternalAddress = classExternalAddress;
	VM->classExternalData = classExternalData;
	VM->classExternalFunction = classExternalFunction;
	VM->classExternalLibrary = classExternalLibrary;
	VM->classExternalStructure = classExternalStructure;
	VM->ioLoadModuleOfLength = ioLoadModuleOfLength;
	VM->ioLoadSymbolOfLengthFromModule = ioLoadSymbolOfLengthFromModule;
	VM->isInMemory = isInMemory;
	VM->signed32BitIntegerFor = signed32BitIntegerFor;
	VM->signed32BitValueOf = signed32BitValueOf;
	VM->includesBehaviorThatOf = includesBehaviorThatOf;
	VM->classLargeNegativeInteger = classLargeNegativeInteger;

#endif

#if VM_PROXY_MINOR > 3

	VM->ioLoadFunctionFrom = ioLoadFunctionFrom;
	VM->ioMicroMSecs = ioMicroMSecs;

#endif

#if VM_PROXY_MINOR > 4

	VM->positive64BitIntegerFor = positive64BitIntegerFor;
	VM->positive64BitValueOf = positive64BitValueOf;
	VM->signed64BitIntegerFor = signed64BitIntegerFor;
	VM->signed64BitValueOf = signed64BitValueOf;

#endif

#if VM_PROXY_MINOR > 5
	VM->isArray = isArray;
	VM->forceInterruptCheck = forceInterruptCheck;
#endif

#if VM_PROXY_MINOR > 6
	VM->fetchLong32ofObject = fetchLong32ofObject;
	VM->getThisSessionID = getThisSessionID;
	VM->ioFilenamefromStringofLengthresolveAliases = ioFilenamefromStringofLengthresolveAliases;
	VM->vmEndianness = vmEndianness;
#endif

#if VM_PROXY_MINOR > 7
	VM->callbackEnter = callbackEnter;
	VM->callbackLeave = callbackLeave;
	VM->addGCRoot = addGCRoot;
	VM->removeGCRoot = removeGCRoot;
#endif

#if VM_PROXY_MINOR > 8
	VM->primitiveFailFor    = primitiveFailFor;
	VM->setInterruptCheckChain = setInterruptCheckChain;
	VM->classAlien          = classAlien;
	VM->classUnsafeAlien    = classUnsafeAlien;
	VM->sendInvokeCallbackStackRegistersJmpbuf = sendInvokeCallbackStackRegistersJmpbuf;
	VM->reestablishContextPriorToCallback = reestablishContextPriorToCallback;
# if ALIEN_FFI
	VM->getStackPointer     = 0;
# endif
# if IMMUTABILITY
	VM->internalIsImmutable = internalIsImmutable;
	VM->internalIsMutable   = internalIsMutable;
# else
	VM->internalIsImmutable = isIntegerObject;
	VM->internalIsMutable   = isNonIntegerObject;
# endif
#endif

#if VM_PROXY_MINOR > 9
	VM->methodArg = methodArg;
	VM->objectArg = objectArg;
	VM->integerArg = integerArg;
	VM->floatArg = floatArg;
	VM->methodReturnValue = methodReturnValue;
	VM->topRemappableOop = topRemappableOop;
#endif

#if VM_PROXY_MINOR > 10
	VM->disownVM = disownVM;
	VM->ownVM = ownVM;
	VM->addHighPriorityTickee = addHighPriorityTickee;
	VM->addSynchronousTickee = addSynchronousTickee;
	VM->utcMicroseconds = ioUTCMicroseconds;
	VM->tenuringIncrementalGC = tenuringIncrementalGC;
	VM->isYoung = isYoung;
	VM->isKindOfClass = isKindOfClass;
	VM->primitiveErrorTable = primitiveErrorTable;
	VM->primitiveFailureCode = primitiveFailureCode;
	VM->instanceSizeOf = instanceSizeOf;
#endif

#if VM_PROXY_MINOR > 11
	VM->sendInvokeCallbackContext = sendInvokeCallbackContext;
	VM->returnAsThroughCallbackContext = returnAsThroughCallbackContext;
	VM->signedMachineIntegerValueOf = signedMachineIntegerValueOf;
	VM->stackSignedMachineIntegerValue = stackSignedMachineIntegerValue;
	VM->positiveMachineIntegerValueOf = positiveMachineIntegerValueOf;
	VM->stackPositiveMachineIntegerValue = stackPositiveMachineIntegerValue;
	VM->getInterruptPending = getInterruptPending;
	VM->cStringOrNullFor = cStringOrNullFor;
	VM->startOfAlienData = startOfAlienData;
	VM->sizeOfAlienData = sizeOfAlienData;
#endif

	return VM;
}


/* This lives here for now but belongs somewhere else.
 * platforms/Cross/vm/sqStuff.c??
 */
#define STDOUT_STACK_SZ 5
static int stdoutStackIdx = -1;
static FILE stdoutStack[STDOUT_STACK_SZ];

/* N.B. As of cygwin 1.5.25 fopen("crash.dmp","a") DOES NOT WORK!  crash.dmp
 * contains garbled output as if the file pointer gets set to the start of the
 * file, not the end.  So we synthesize our own append mode.
 */
#if __MINGW32__
# include <io.h>
static FILE *
fopen_for_append(char *filename)
{
	FILE *f = !access(filename, F_OK) /* access is bass ackwards */
		? fopen(filename,"r+")
		: fopen(filename,"w+");
	if (f)
		fseek(f,0,SEEK_END);
	return f;
}
#elif defined(WIN32)
# define fopen_for_append(filename) fopen(filename,"a+t")
#else
# define fopen_for_append(filename) fopen(filename,"a+")
#endif

void
pushOutputFile(char *filenameOrStdioIndex)
{
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
# define STDERR_FILENO 2
#endif

	FILE *output;

	if (stdoutStackIdx + 2 >= STDOUT_STACK_SZ) {
		fprintf(stderr,"output file stack is full.\n");
		return;
	}
	switch ((unsigned)filenameOrStdioIndex) {
	case STDOUT_FILENO: output = stdout; break;
	case STDERR_FILENO: output = stderr; break;
	default:
		if (!(output = fopen_for_append(filenameOrStdioIndex))) {
			fprintf(stderr,
					"could not open \"%s\" for writing.\n",
					filenameOrStdioIndex);
			return;
		}
	}
	stdoutStack[++stdoutStackIdx] = *stdout;
	*stdout = *output;
}

void
popOutputFile()
{
	if (stdoutStackIdx < 0) {
		fprintf(stderr,"output file stack is empty.\n");
		return;
	}
	fflush(stdout);
	if (fileno(stdout) > STDERR_FILENO) {
		/* as of Feb 2011 with fclose@@GLIBC_2.1 under e.g. CentOS 5.3, fclose
		 * hangs in _IO_un_link_internal.  This hack avoids that.
		 */
#if __linux__
		close(fileno(stdout));
#else
		fclose(stdout);
#endif
	}
	*stdout = stdoutStack[stdoutStackIdx--];
}
