#pragma once

typedef void(*fnASDocInitCallback)(void *pASDoc);

const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

const int ASHookFlag_MapScript = 1;
const int ASHookFlag_Plugin = 2;

/*
	Callbacks must be registered before AngelScript initialization
*/
typedef bool (*fnASEXT_RegisterDocInitCallback)(fnASDocInitCallback callback);

extern fnASEXT_RegisterDocInitCallback ASEXT_RegisterDocInitCallback;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectMethod)(void *pASDoc, const char *docs, const char *name, const char *func, void *pfn, int type);

extern fnASEXT_RegisterObjectMethod ASEXT_RegisterObjectMethod;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectType)(void *pASDoc, const char *docs, const char *name, int unk, unsigned int flags);

extern fnASEXT_RegisterObjectType ASEXT_RegisterObjectType;

/*
	Must be called inside DocInitCallback
*/
typedef void (*fnASEXT_RegisterObjectProperty)(void *pASDoc, const char *docs, const char *name, const char *prop, int offset);

extern fnASEXT_RegisterObjectProperty ASEXT_RegisterObjectProperty;

/*
	Must be registered before call hook
*/
typedef void *(*fnASEXT_RegisterHook)(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args);

extern fnASEXT_RegisterHook ASEXT_RegisterHook;

typedef void(*fnASEXT_CallHook)(void *hook, int unk, ...);

extern fnASEXT_CallHook *ASEXT_CallHook;

#ifdef _WIN32

#define SC_SERVER_DECL __fastcall
#define SC_SERVER_DUMMYARG_NOCOMMA , int dummy
#define SC_SERVER_DUMMYARG int dummy, 
#define SC_SERVER_PASS_DUMMYARG dummy, 
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA dummy

#else

#define SC_SERVER_DECL 
#define SC_SERVER_DUMMYARG_NOCOMMA
#define SC_SERVER_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG
#define SC_SERVER_PASS_DUMMYARG_NOCOMMA

#endif