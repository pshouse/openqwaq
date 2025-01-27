/* Automatically generated by
	VMPluginCodeGenerator VMMaker-eem.666 uuid: 4edd7d00-3db5-48ef-b579-5d95fae49120
   from
	WebAuthPlugin VMMaker-eem.666 uuid: 4edd7d00-3db5-48ef-b579-5d95fae49120
 */
static char __buildInfo[] = "WebAuthPlugin VMMaker-eem.666 uuid: 4edd7d00-3db5-48ef-b579-5d95fae49120 " __DATE__ ;




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
#include "WebAuthPlugin.h"

#include "sqMemoryAccess.h"


/*** Constants ***/


/*** Function Prototypes ***/
static VirtualMachine * getInterpreter(void);
EXPORT(const char*) getModuleName(void);
static sqInt halt(void);
EXPORT(sqInt) initialiseModule(void);
static sqInt msg(char *s);
EXPORT(sqInt) primitiveAuthContinue(void);
EXPORT(sqInt) primitiveAuthDestroy(void);
EXPORT(sqInt) primitiveAuthInitiate(void);
EXPORT(sqInt) primitiveAuthToken(void);
EXPORT(sqInt) setInterpreter(struct VirtualMachine*anInterpreter);
EXPORT(sqInt) shutdownModule(void);
static void * stackStringValue(sqInt index);


/*** Variables ***/

#ifdef SQUEAK_BUILTIN_PLUGIN
extern
#endif
struct VirtualMachine* interpreterProxy;
static const char *moduleName =
#ifdef SQUEAK_BUILTIN_PLUGIN
	"WebAuthPlugin VMMaker-eem.666 (i)"
#else
	"WebAuthPlugin VMMaker-eem.666 (e)"
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

EXPORT(sqInt)
initialiseModule(void) {
	return sqAuthInitialize();
}

static sqInt
msg(char *s) {
	fprintf(stderr, "\n%s: %s", moduleName, s);
	return 0;
}


/*	Primitive. Continue an authentication session.
	Arguments:
	session - the auth session identifier.
	token - the token received from the server
	Returns:
	<0 - Error. The error code is platform specific.
	0 - OK. The session is authenticated.
	1	- Another server-roundtrip is needed.
	 */

EXPORT(sqInt)
primitiveAuthContinue(void) {
    sqInt handle;
    sqInt oop;
    sqInt status;
    char*token;
    sqInt tokensize;

	if (!((interpreterProxy->methodArgumentCount()) == 2)) {
		return interpreterProxy->primitiveFail();
	}
	oop = interpreterProxy->stackValue(0);
	if (!(interpreterProxy->isBytes(oop))) {
		return interpreterProxy->primitiveFail();
	}
	token = interpreterProxy->firstIndexableField(oop);
	tokensize = interpreterProxy->byteSizeOf(oop);
	handle = interpreterProxy->positive32BitValueOf(interpreterProxy->stackValue(1));
	if (interpreterProxy->failed()) {
		return null;
	}
	status = sqAuthContinue(handle, token, tokensize);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->pushInteger(status);
}


/*	Primitive. Destroy an authentication session.
	Arguments:
	session - the auth session identifier.
	Returns:
	<0 - Error. The error code is platform specific.
	0 - OK.
	 */

EXPORT(sqInt)
primitiveAuthDestroy(void) {
    sqInt handle;
    sqInt status;

	if (!((interpreterProxy->methodArgumentCount()) == 1)) {
		return interpreterProxy->primitiveFail();
	}
	handle = interpreterProxy->positive32BitValueOf(interpreterProxy->stackValue(0));
	if (interpreterProxy->failed()) {
		return null;
	}
	status = sqAuthDestroy(handle);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->pushInteger(status);
}


/*	Primitive. Initiate an authentication method.
	Arguments:
	method - the name of the authentication method
	user - the user name (optional)
	pass - the password (optional)
	domain - the authentication domain (optional)
	principal - the kerberos service principal name
	Returns:
	session - the auth session identifier.
	If any of the arguments are missing, the currently logged on
	user will be used. The names of the authentication methods can be:
	NTLM - for NTLM authentication.
	Negotiate - For Microsoft Negotiate
	See also RFC 4559 for details. */

EXPORT(sqInt)
primitiveAuthInitiate(void) {
    char*domain;
    sqInt handle;
    char*method;
    char*password;
    char*principal;
    char*username;

	if (!((interpreterProxy->methodArgumentCount()) == 5)) {
		return interpreterProxy->primitiveFail();
	}
	principal = stackStringValue(0);
	domain = stackStringValue(1);
	password = stackStringValue(2);
	username = stackStringValue(3);
	method = stackStringValue(4);
	if (!(interpreterProxy->failed())) {
		handle = sqAuthInitiate(method, username, password, domain, principal);
		if (handle == 0) {
			interpreterProxy->primitiveFail();
		}
	}
	if (username == null) {
		null;
	}
	else {
		free(username);
	}
	if (password == null) {
		null;
	}
	else {
		free(password);
	}
	if (domain == null) {
		null;
	}
	else {
		free(domain);
	}
	if (principal == null) {
		null;
	}
	else {
		free(principal);
	}
	if (!(interpreterProxy->failed())) {
		interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
		interpreterProxy->push(interpreterProxy->positive32BitIntegerFor(handle));
	}
}


/*	Primitive. Answer the next token for the authentication.
	Arguments:
	session - the ntml session identifier
	Returns:
	token - the token to continue the authentication with. */

EXPORT(sqInt)
primitiveAuthToken(void) {
    char*byteptr;
    sqInt handle;
    sqInt oop;
    char*token;
    sqInt tokensize;

	if (!((interpreterProxy->methodArgumentCount()) == 1)) {
		return interpreterProxy->primitiveFail();
	}
	handle = interpreterProxy->positive32BitValueOf(interpreterProxy->stackValue(0));
	if (interpreterProxy->failed()) {
		return null;
	}
	token = sqAuthToken(handle, &tokensize);
	if (token == null) {
		return interpreterProxy->primitiveFail();
	}
	oop = interpreterProxy->instantiateClassindexableSize(interpreterProxy->classByteArray(), tokensize);
	byteptr = interpreterProxy->firstIndexableField(oop);
	memcpy(byteptr, token, tokensize);
	interpreterProxy->pop((interpreterProxy->methodArgumentCount()) + 1);
	interpreterProxy->push(oop);
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

EXPORT(sqInt)
shutdownModule(void) {
	return sqAuthShutdown();
}

static void *
stackStringValue(sqInt index) {
    char*dstPtr;
    sqInt i;
    sqInt obj;
    char*srcPtr;
    sqInt sz;

	obj = interpreterProxy->stackValue(index);
	if (obj == (interpreterProxy->nilObject())) {
		return null;
	}
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

void* WebAuthPlugin_exports[][3] = {
	{"WebAuthPlugin", "getModuleName", (void*)getModuleName},
	{"WebAuthPlugin", "initialiseModule", (void*)initialiseModule},
	{"WebAuthPlugin", "primitiveAuthContinue", (void*)primitiveAuthContinue},
	{"WebAuthPlugin", "primitiveAuthDestroy", (void*)primitiveAuthDestroy},
	{"WebAuthPlugin", "primitiveAuthInitiate", (void*)primitiveAuthInitiate},
	{"WebAuthPlugin", "primitiveAuthToken", (void*)primitiveAuthToken},
	{"WebAuthPlugin", "setInterpreter", (void*)setInterpreter},
	{"WebAuthPlugin", "shutdownModule", (void*)shutdownModule},
	{NULL, NULL, NULL}
};

#endif /* ifdef SQ_BUILTIN_PLUGIN */
