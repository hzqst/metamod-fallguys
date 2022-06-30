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
C_DLLEXPORT bool ASEXT_RegisterDocInitCallback(fnASDocInitCallback callback);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectMethod(void *pASDoc, const char *docs, const char *name, const char *func, void *pfn, int type);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectType(void *pASDoc, const char *docs, const char *name, int unk, unsigned int flags);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectProperty(void *pASDoc, const char *docs, const char *name, const char *prop, int offset);

/*
	Must be registered before call hook
*/
C_DLLEXPORT void *ASEXT_RegisterHook(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args);

typedef void(*fnASEXT_CallHook)(void *hook, int unk, ...);

C_DLLEXPORT fnASEXT_CallHook ASEXT_CallHook;