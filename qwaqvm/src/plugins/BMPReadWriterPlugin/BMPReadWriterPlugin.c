/* Automatically generated by
	VMPluginCodeGenerator VMMaker-eem.524 uuid: 9b748596-0986-4ca7-ac5b-b7a050a08431
   from
	BMPReadWriterPlugin VMMaker-eem.524 uuid: 9b748596-0986-4ca7-ac5b-b7a050a08431
 */
static char __buildInfo[] = "BMPReadWriterPlugin VMMaker-eem.524 uuid: 9b748596-0986-4ca7-ac5b-b7a050a08431 " __DATE__ ;




#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Default EXPORT macro that does nothing (see comment in sq.h): */
#define EXPORT(returnType) returnType

/* Do not include the entire sq.h file but just those parts needed. */
/*  The virtual machine proxy definition */
#include "sqVirtualMachine.h"
/* Configuration options */
#include "sqConfig.h"
/* Platform specific definitions */
#include "sqPlatformSpecific.h"

#define true 1
#define false 0
#define null 0  /* using 'null' because nil is predefined in Think C */
#ifdef SQUEAK_BUILTIN_PLUGIN
#undef EXPORT
// was #undef EXPORT(returnType) but screws NorCroft cc
#define EXPORT(returnType) static returnType
#endif

#include "sqMemoryAccess.h"



/*** Constants ***/


/*** Function Prototypes ***/
static VirtualMachine * getInterpreter(void);
EXPORT(const char*) getModuleName(void);
static sqInt halt(void);
static sqInt msg(char *s);
EXPORT(sqInt) primitiveRead24BmpLine(void);
EXPORT(sqInt) primitiveWrite24BmpLine(void);
EXPORT(sqInt) setInterpreter(struct VirtualMachine*anInterpreter);


/*** Variables ***/

#ifdef SQUEAK_BUILTIN_PLUGIN
extern
#endif
struct VirtualMachine* interpreterProxy;
static const char *moduleName =
#ifdef SQUEAK_BUILTIN_PLUGIN
	"BMPReadWriterPlugin VMMaker-eem.524 (i)"
#else
	"BMPReadWriterPlugin VMMaker-eem.524 (e)"
#endif
;



/*	Note: This is coded so that plugins can be run from Squeak. */

static VirtualMachine *
getInterpreter(void) {
	return interpreterProxy;
}


/*	Note: This is hardcoded so it can be run from Squeak.
	The module name is used for validating a module *after*
	it is loaded to check if it does really contain the module
	we're thinking it contains. This is important! */

EXPORT(const char*)
getModuleName(void) {
	return moduleName;
}

static sqInt
halt(void) {
	;
	return 0;
}

static sqInt
msg(char *s) {
	fprintf(stderr, "\n%s: %s", moduleName, s);
	return 0;
}

EXPORT(sqInt)
primitiveRead24BmpLine(void) {
    unsigned int *formBits;
    sqInt formBitsIndex;
    sqInt formBitsOop;
    sqInt formBitsSize;
    unsigned char *pixelLine;
    sqInt pixelLineOop;
    sqInt pixelLineSize;
    sqInt width;

	if (!((interpreterProxy->methodArgumentCount()) == 4)) {
		return interpreterProxy->primitiveFail();
	}
	width = interpreterProxy->stackIntegerValue(0);
	formBitsIndex = interpreterProxy->stackIntegerValue(1);
	formBitsOop = interpreterProxy->stackObjectValue(2);
	pixelLineOop = interpreterProxy->stackObjectValue(3);
	if (interpreterProxy->failed()) {
		return null;
	}
	if (!(interpreterProxy->isWords(formBitsOop))) {
		return interpreterProxy->primitiveFail();
	}
	if (!(interpreterProxy->isBytes(pixelLineOop))) {
		return interpreterProxy->primitiveFail();
	}
	formBitsSize = interpreterProxy->slotSizeOf(formBitsOop);
	formBits = interpreterProxy->firstIndexableField(formBitsOop);
	pixelLineSize = interpreterProxy->slotSizeOf(pixelLineOop);
	pixelLine = interpreterProxy->firstIndexableField(pixelLineOop);
	if (!(((formBitsIndex + width) <= formBitsSize)
		 && ((width * 3) <= pixelLineSize))) {
		return interpreterProxy->primitiveFail();
	}
	
	formBits += formBitsIndex-1;
	while(width--) {
		unsigned int rgb;
		rgb = (*pixelLine++);
		rgb += (*pixelLine++) << 8;
		rgb += (*pixelLine++) << 16;
		if(rgb) rgb |= 0xFF000000; else rgb |= 0xFF000001;
		*formBits++ = rgb;
	}
	;
	interpreterProxy->pop(4);
}

EXPORT(sqInt)
primitiveWrite24BmpLine(void) {
    unsigned int *formBits;
    sqInt formBitsIndex;
    sqInt formBitsOop;
    sqInt formBitsSize;
    unsigned char *pixelLine;
    sqInt pixelLineOop;
    sqInt pixelLineSize;
    sqInt width;

	if (!((interpreterProxy->methodArgumentCount()) == 4)) {
		return interpreterProxy->primitiveFail();
	}
	width = interpreterProxy->stackIntegerValue(0);
	formBitsIndex = interpreterProxy->stackIntegerValue(1);
	formBitsOop = interpreterProxy->stackObjectValue(2);
	pixelLineOop = interpreterProxy->stackObjectValue(3);
	if (interpreterProxy->failed()) {
		return null;
	}
	if (!(interpreterProxy->isWords(formBitsOop))) {
		return interpreterProxy->primitiveFail();
	}
	if (!(interpreterProxy->isBytes(pixelLineOop))) {
		return interpreterProxy->primitiveFail();
	}
	formBitsSize = interpreterProxy->slotSizeOf(formBitsOop);
	formBits = interpreterProxy->firstIndexableField(formBitsOop);
	pixelLineSize = interpreterProxy->slotSizeOf(pixelLineOop);
	pixelLine = interpreterProxy->firstIndexableField(pixelLineOop);
	if (!(((formBitsIndex + width) <= formBitsSize)
		 && ((width * 3) <= pixelLineSize))) {
		return interpreterProxy->primitiveFail();
	}
	
	formBits += formBitsIndex-1;

	while(width--) {
		unsigned int rgb;
		rgb = *formBits++;
		(*pixelLine++) = (rgb      ) & 0xFF;
		(*pixelLine++) = (rgb >> 8 ) & 0xFF;
		(*pixelLine++) = (rgb >> 16) & 0xFF;
	}

	;
	interpreterProxy->pop(4);
}


/*	Note: This is coded so that is can be run from Squeak. */

EXPORT(sqInt)
setInterpreter(struct VirtualMachine*anInterpreter) {
    sqInt ok;

	interpreterProxy = anInterpreter;
	ok = interpreterProxy->majorVersion() == VM_PROXY_MAJOR;
	if (ok == 0) {
		return 0;
	}
	ok = interpreterProxy->minorVersion() >= VM_PROXY_MINOR;
	return ok;
}


#ifdef SQUEAK_BUILTIN_PLUGIN

void* BMPReadWriterPlugin_exports[][3] = {
	{"BMPReadWriterPlugin", "getModuleName", (void*)getModuleName},
	{"BMPReadWriterPlugin", "primitiveRead24BmpLine", (void*)primitiveRead24BmpLine},
	{"BMPReadWriterPlugin", "primitiveWrite24BmpLine", (void*)primitiveWrite24BmpLine},
	{"BMPReadWriterPlugin", "setInterpreter", (void*)setInterpreter},
	{NULL, NULL, NULL}
};

#endif /* ifdef SQ_BUILTIN_PLUGIN */
