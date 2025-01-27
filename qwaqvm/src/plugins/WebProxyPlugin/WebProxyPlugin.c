/* Automatically generated by
	VMPluginCodeGenerator VMMaker-eem.524 uuid: 9b748596-0986-4ca7-ac5b-b7a050a08431
   from
	WebProxyPlugin Qwaq-Plugins-bgf.97 uuid: 15a615ba-3290-40a1-877d-f025b25fd3af
 */
static char __buildInfo[] = "WebProxyPlugin Qwaq-Plugins-bgf.97 uuid: 15a615ba-3290-40a1-877d-f025b25fd3af " __DATE__ ;




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
#include "WebProxyPlugin.h"

#include "sqMemoryAccess.h"



/*** Constants ***/


/*** Function Prototypes ***/
static VirtualMachine * getInterpreter(void);
EXPORT(const char*) getModuleName(void);
static sqInt halt(void);
static sqInt msg(char *s);
static sqInt oopFromString(char*cString);
EXPORT(sqInt) primitiveDefaultProxyServer(void);
EXPORT(sqInt) primitiveDynamicAutoProxyUrl(void);
EXPORT(sqInt) primitiveProxyForUrl(void);
EXPORT(sqInt) primitiveStaticAutoProxyUrl(void);
EXPORT(sqInt) setInterpreter(struct VirtualMachine*anInterpreter);
static void * stackStringValue(sqInt index);


/*** Variables ***/

#ifdef SQUEAK_BUILTIN_PLUGIN
extern
#endif
struct VirtualMachine* interpreterProxy;
static const char *moduleName =
#ifdef SQUEAK_BUILTIN_PLUGIN
	"WebProxyPlugin Qwaq-Plugins-bgf.97 (i)"
#else
	"WebProxyPlugin Qwaq-Plugins-bgf.97 (e)"
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

static sqInt
oopFromString(char*cString) {
    char*dstPtr;
    sqInt i;
    sqInt result;
    sqInt sz;

	if (cString == null) {
		return interpreterProxy->nilObject();
	}
	sz = strlen(cString);
	result = interpreterProxy->instantiateClassindexableSize(interpreterProxy->classString(), sz);
	dstPtr = interpreterProxy->firstIndexableField(result);
	for (i = 0; i <= (sz - 1); i += 1) {
		dstPtr[i] = (cString[i]);
	}
	return result;
}


/*	Primitive. Answer the default proxy server(s) for this box. */

EXPORT(sqInt)
primitiveDefaultProxyServer(void) {
    sqInt result;
    char*urlString;

	if (!((interpreterProxy->methodArgumentCount()) == 0)) {
		return interpreterProxy->primitiveFail();
	}
	urlString = defaultProxyServer();
	if (urlString == null) {
		return interpreterProxy->primitiveFail();
	}
	result = oopFromString(urlString);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->push(result);
}


/*	Primitive. Detect the URL for autoproxy (PAC) configuration */

EXPORT(sqInt)
primitiveDynamicAutoProxyUrl(void) {
    sqInt result;
    char*urlString;

	if (!((interpreterProxy->methodArgumentCount()) == 0)) {
		return interpreterProxy->primitiveFail();
	}
	urlString = dynamicAutoProxyUrl();
	if (urlString == null) {
		return interpreterProxy->primitiveFail();
	}
	result = oopFromString(urlString);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->push(result);
}


/*	Primitive. Find a proxy for the given url. */

EXPORT(sqInt)
primitiveProxyForUrl(void) {
    char*pacString;
    char*proxy;
    sqInt result;
    char*urlString;

	if (!((interpreterProxy->methodArgumentCount()) == 2)) {
		return interpreterProxy->primitiveFail();
	}
	pacString = stackStringValue(0);
	urlString = stackStringValue(1);
	if (!(interpreterProxy->failed())) {
		proxy = proxyServerForUrl(urlString, pacString);
	}
	if (pacString == null) {
		null;
	}
	else {
		free(pacString);
	}
	if (urlString == null) {
		null;
	}
	else {
		free(urlString);
	}
	if (proxy == null) {
		return interpreterProxy->primitiveFail();
	}
	result = oopFromString(proxy);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->push(result);
}


/*	Primitive. Answer the default URL for autoproxy (PAC) configuration */

EXPORT(sqInt)
primitiveStaticAutoProxyUrl(void) {
    sqInt result;
    char*urlString;

	if (!((interpreterProxy->methodArgumentCount()) == 0)) {
		return interpreterProxy->primitiveFail();
	}
	urlString = staticAutoProxyUrl();
	if (urlString == null) {
		return interpreterProxy->primitiveFail();
	}
	result = oopFromString(urlString);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->push(result);
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

static void *
stackStringValue(sqInt index) {
    char*dstPtr;
    sqInt i;
    sqInt obj;
    char*srcPtr;
    sqInt sz;

	obj = interpreterProxy->stackValue(index);
	if (!(interpreterProxy->isBytes(obj))) {
		interpreterProxy->primitiveFail();
		return null;
	}
	sz = interpreterProxy->byteSizeOf(obj);
	dstPtr = malloc(sz + 1);
	srcPtr = interpreterProxy->firstIndexableField(obj);
	for (i = 0; i <= (sz - 1); i += 1) {
		dstPtr[i] = (srcPtr[i]);
	}
	dstPtr[sz] = 0;
	return dstPtr;
}


#ifdef SQUEAK_BUILTIN_PLUGIN

void* WebProxyPlugin_exports[][3] = {
	{"WebProxyPlugin", "getModuleName", (void*)getModuleName},
	{"WebProxyPlugin", "primitiveDefaultProxyServer", (void*)primitiveDefaultProxyServer},
	{"WebProxyPlugin", "primitiveDynamicAutoProxyUrl", (void*)primitiveDynamicAutoProxyUrl},
	{"WebProxyPlugin", "primitiveProxyForUrl", (void*)primitiveProxyForUrl},
	{"WebProxyPlugin", "primitiveStaticAutoProxyUrl", (void*)primitiveStaticAutoProxyUrl},
	{"WebProxyPlugin", "setInterpreter", (void*)setInterpreter},
	{NULL, NULL, NULL}
};

#endif /* ifdef SQ_BUILTIN_PLUGIN */
