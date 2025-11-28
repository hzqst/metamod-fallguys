#pragma once

const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

const int ASFlag_MapScript = 1;
const int ASFlag_Plugin = 2;

const int ASFileAccessControl_Read = 1;
const int ASFileAccessControl_Write = 2;

class aslScriptFunction;
class CASFunction;
class CASModule;
class CASServerManager;
class CASDocumentation;
class CASDirectoryList;
class CScriptArray;
class CScriptBuilder;

typedef void(*fnASDocInitCallback)(CASDocumentation *pASDoc);
typedef void(*fnASDirInitCallback)(CASDirectoryList *pASDir);
typedef void(*fnScriptBuilderDefineCallback)(CScriptBuilder* pScriptBuilder);

/*
	Callbacks must be registered before AngelScript initialization
*/
C_DLLEXPORT bool ASEXT_RegisterDocInitCallback(fnASDocInitCallback callback);

/*
	Callbacks must be registered before AngelScript initialization
*/
C_DLLEXPORT bool ASEXT_RegisterDirInitCallback(fnASDirInitCallback callback);

/*
	Callbacks must be registered before module start
*/
C_DLLEXPORT bool ASEXT_RegisterScriptBuilderDefineCallback(fnScriptBuilderDefineCallback callback);

C_DLLEXPORT bool ASEXT_UnregisterScriptBuilderDefineCallback(fnScriptBuilderDefineCallback callback);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectMethod(CASDocumentation *pASDoc, const char *docs, const char *name, const char *func, void *pfn, int type);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectMethodEx(CASDocumentation* pASDoc, const char* docs, const char* name, const char* func, void* funcptr, int type);


/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectBehaviour(CASDocumentation *pASDoc, const char *docs, const char *name, int behaviour, const char *func, void *pfn, int type);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectBehaviourEx(CASDocumentation* pthis, const char* docs, const char* name, int behaviour, const char* func, void* funcptr, int type);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectType(CASDocumentation *pASDoc, const char *docs, const char *name, int size, unsigned int flags);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterObjectProperty(CASDocumentation *pASDoc, const char *docs, const char *name, const char *prop, int offset);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterGlobalProperty(CASDocumentation *pASDoc, const char *docs, const char *name, void *ptr);

/*
	Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_RegisterFuncDef(CASDocumentation *pASDoc, const char *docs, const char *funcdef);

/* Create directory in the virtual file system */
C_DLLEXPORT void ASEXT_CreateDirectory(void *pASDir, const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk);

C_DLLEXPORT void ASEXT_CStringAssign(void *pthis, const char *src, size_t len);

C_DLLEXPORT void ASEXT_CStringdtor(void *pthis);

C_DLLEXPORT CASFunction *ASEXT_CreateCASFunction(aslScriptFunction *aslfn, CASModule *asmodule, int unk);

/*
	Must be registered before call hook
*/
C_DLLEXPORT void *ASEXT_RegisterHook(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args);

typedef void(*fnASEXT_CallHook)(void *hook, int dummy, ...);

C_DLLEXPORT fnASEXT_CallHook ASEXT_CallHook;

typedef int(*fnASEXT_CallCASBaseCallable)(void *callable, int dummy, ...);

C_DLLEXPORT fnASEXT_CallCASBaseCallable ASEXT_CallCASBaseCallable;

C_DLLEXPORT CASServerManager *ASEXT_GetServerManager();

C_DLLEXPORT bool ASEXT_CASRefCountedBaseClass_InternalRelease(void *ref);

C_DLLEXPORT void ASEXT_CScriptAny_Release(void *anywhat);

C_DLLEXPORT void ASEXT_CScriptArray_Release(void* anywhat);

C_DLLEXPORT void* ASEXT_GetCurrentContext();

/*
	Can only be called in ScriptBuilder_DefineCallback
*/
C_DLLEXPORT void ASEXT_ScriptBuilder_DefineWord(CScriptBuilder* pthis, const char* word);