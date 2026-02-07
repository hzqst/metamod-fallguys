#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include <vector>
#include <algorithm>

#include "asext.h"

#include "serverdef.h"

PRIVATE_FUNCTION_DEFINE(CASHook_Call);
PRIVATE_FUNCTION_DEFINE(CASHook_CASHook);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectType);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectProperty);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterGlobalProperty);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectMethod);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectBehaviour);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterFuncDef);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterEnum);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterEnumValue);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_SetDefaultNamespace);
PRIVATE_FUNCTION_DEFINE(CASDirectoryList_CreateDirectory);
PRIVATE_FUNCTION_DEFINE(CASFunction_Create);
PRIVATE_FUNCTION_DEFINE(CASBaseCallable_Call);
PRIVATE_FUNCTION_DEFINE(CASRefCountedBaseClass_InternalRelease);
PRIVATE_FUNCTION_DEFINE(CScriptAny_Release);
PRIVATE_FUNCTION_DEFINE(CScriptArray_Release);
PRIVATE_FUNCTION_DEFINE(CString_Assign);
PRIVATE_FUNCTION_DEFINE(CString_dtor);
PRIVATE_FUNCTION_DEFINE(asGetActiveContext);
PRIVATE_FUNCTION_DEFINE(CScriptBuilder_DefineWord);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Create);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_AddRef);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Release);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Set);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Get);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Exists);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_IsEmpty);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_GetSize);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_Delete);
PRIVATE_FUNCTION_DEFINE(CScriptDictionary_DeleteAll);

CASServerManager **g_pServerManager = NULL;

fnASEXT_CallHook ASEXT_CallHook = NULL;

fnASEXT_CallCASBaseCallable ASEXT_CallCASBaseCallable = NULL;

bool g_ASDocInit = false;
std::vector<fnASDocInitCallback> g_ASDocInitCallbacks;

bool g_ASDirInit = false;
std::vector<fnASDirInitCallback> g_ASDirInitCallbacks;

std::vector<fnScriptBuilderDefineCallback> g_ScriptBuilderDefineCallbacks;

std::vector<CASHook *> g_ASHooks;

C_DLLEXPORT void ASEXT_RegisterObjectMethod(CASDocumentation *pthis, const char *docs, const char *name, const char *func, void *pfn, int type)
{
	SC_SERVER_DUMMYVAR;

	asSFuncPtr reg;
	reg.pfn = pfn;
	reg.flag = 3;//3 = method

	g_call_original_CASDocumentation_RegisterObjectMethod(pthis, SC_SERVER_PASS_DUMMYARG docs, name, func, &reg, type);
}

C_DLLEXPORT void ASEXT_RegisterObjectMethodEx(CASDocumentation* pthis, const char* docs, const char* name, const char* func, void* funcptr, int type)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDocumentation_RegisterObjectMethod(pthis, SC_SERVER_PASS_DUMMYARG docs, name, func, (asSFuncPtr*)funcptr, type);
}

C_DLLEXPORT void ASEXT_RegisterObjectBehaviour(CASDocumentation *pthis, const char *docs, const char *name, int behaviour, const char *func, void *pfn, int type)
{
	SC_SERVER_DUMMYVAR;

	asSFuncPtr reg;
	reg.pfn = pfn;
	reg.flag = 2;//2 = global func

	g_call_original_CASDocumentation_RegisterObjectBehaviour(pthis, SC_SERVER_PASS_DUMMYARG docs, name, behaviour, func, &reg, type, 0);
}

C_DLLEXPORT void ASEXT_RegisterObjectBehaviourEx(CASDocumentation* pthis, const char* docs, const char* name, int behaviour, const char* func, void* funcptr, int type)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDocumentation_RegisterObjectBehaviour(pthis, SC_SERVER_PASS_DUMMYARG docs, name, behaviour, func, (asSFuncPtr*)funcptr, type, 0);
}

C_DLLEXPORT void ASEXT_RegisterObjectType(CASDocumentation *pthis, const char *docs, const char *name, int size, unsigned int flags)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectType(pthis, SC_SERVER_PASS_DUMMYARG docs, name, size, flags);
}

C_DLLEXPORT void ASEXT_RegisterObjectProperty(CASDocumentation *pthis, const char *docs, const char *name, const char *prop, int offset)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG docs, name, prop, offset);
}

C_DLLEXPORT void ASEXT_RegisterGlobalProperty(CASDocumentation *pthis, const char *docs, const char *name, void *ptr)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterGlobalProperty(pthis, SC_SERVER_PASS_DUMMYARG docs, name, ptr);
}

C_DLLEXPORT void ASEXT_RegisterFuncDef(CASDocumentation *pthis, const char *docs, const char *funcdef)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterFuncDef(pthis, SC_SERVER_PASS_DUMMYARG docs, funcdef);
}

C_DLLEXPORT void ASEXT_RegisterEnum(CASDocumentation* pthis, const char* docs, const char* enums, int enumtype)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDocumentation_RegisterEnum(pthis, SC_SERVER_PASS_DUMMYARG docs, enums, enumtype);
}

C_DLLEXPORT void ASEXT_RegisterEnumValue(CASDocumentation* pthis, const char* docs, const char* enums, const char* name, int value)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDocumentation_RegisterEnumValue(pthis, SC_SERVER_PASS_DUMMYARG docs, enums, name, value);
}

C_DLLEXPORT void ASEXT_SetDefaultNamespace(CASDocumentation* pthis, const char* ns)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDocumentation_SetDefaultNamespace(pthis, SC_SERVER_PASS_DUMMYARG ns);
}

int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int size, unsigned int flags)
{
	if (name && docs && !strcmp(name, "CSurvivalMode") && !strcmp(docs, "Survival Mode handler") && flags == 0x40001u)
	{
		for (size_t i = 0; i < g_ASDocInitCallbacks.size(); ++i)
		{
			g_ASDocInitCallbacks[i](pthis);
		}

		g_ASDocInitCallbacks.clear();
		g_ASDocInit = true;
	} 

	return g_call_original_CASDocumentation_RegisterObjectType(pthis, SC_SERVER_PASS_DUMMYARG docs, name, size, flags);
}

void SC_SERVER_DECL NewCASDirectoryList_CreateDirectory(CASDirectoryList *pthis, SC_SERVER_DUMMYARG const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk)
{
	if (!strcmp(path, "scripts/plugins/store") && flags == ASFlag_Plugin && access_control == (ASFileAccessControl_Read | ASFileAccessControl_Write) && permanent == 1)
	{
		for (size_t i = 0; i < g_ASDirInitCallbacks.size(); ++i)
		{
			g_ASDirInitCallbacks[i](pthis);
		}

		g_ASDirInitCallbacks.clear();
		g_ASDirInit = true;
	}

	g_call_original_CASDirectoryList_CreateDirectory(pthis, SC_SERVER_PASS_DUMMYARG path, flags, access_control, permanent, unk);
}

C_DLLEXPORT void ASEXT_CreateDirectory(void *pthis, const char *path, unsigned char flags, unsigned char access_control, unsigned char permanent, unsigned char unk)
{
	SC_SERVER_DUMMYVAR;
	g_call_original_CASDirectoryList_CreateDirectory((CASDirectoryList *)pthis, SC_SERVER_PASS_DUMMYARG path, flags, access_control, permanent, unk);
}

C_DLLEXPORT bool ASEXT_RegisterDocInitCallback(fnASDocInitCallback callback)
{
	if (g_ASDocInit)
		return false;

	g_ASDocInitCallbacks.emplace_back(callback);

	return true;
}

C_DLLEXPORT bool ASEXT_RegisterDirInitCallback(fnASDirInitCallback callback)
{
	if (g_ASDirInit)
		return false;

	g_ASDirInitCallbacks.emplace_back(callback);

	return true;
}

C_DLLEXPORT bool ASEXT_RegisterScriptBuilderDefineCallback(fnScriptBuilderDefineCallback callback)
{
	g_ScriptBuilderDefineCallbacks.emplace_back(callback);

	return true;
}

C_DLLEXPORT bool ASEXT_UnregisterScriptBuilderDefineCallback(fnScriptBuilderDefineCallback callback)
{
	auto it = std::remove_if(g_ScriptBuilderDefineCallbacks.begin(), g_ScriptBuilderDefineCallbacks.end(), [callback](fnScriptBuilderDefineCallback cb) {
		return cb == callback;
	});
	bool found = it != g_ScriptBuilderDefineCallbacks.end();
	g_ScriptBuilderDefineCallbacks.erase(it, g_ScriptBuilderDefineCallbacks.end());
	return found;
}

C_DLLEXPORT void *ASEXT_RegisterHook(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args)
{
	SC_SERVER_DUMMYVAR;

	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = stopMode;
	reg.docs = docs;

	CASHook *hook = new CASHook;
	g_ASHooks.emplace_back(hook);

	g_pfn_CASHook_CASHook(hook, SC_SERVER_PASS_DUMMYARG type, flags, domain, func, args, &reg);

	return hook;
}

C_DLLEXPORT void ASEXT_CStringAssign(void *pthis, const char *src, size_t len)
{
	SC_SERVER_DUMMYVAR;

	g_pfn_CString_Assign((CString *)pthis, SC_SERVER_PASS_DUMMYARG src, len);
}

C_DLLEXPORT void ASEXT_CStringdtor(void *pthis)
{
	SC_SERVER_DUMMYVAR;

	g_pfn_CString_dtor((CString *)pthis SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT CASFunction *ASEXT_CreateCASFunction(aslScriptFunction *aslfn, CASModule *asmodule, int unk)
{
	return g_pfn_CASFunction_Create(aslfn, asmodule, unk);
}

C_DLLEXPORT CASServerManager *ASEXT_GetServerManager()
{
	return (*g_pServerManager);
}

C_DLLEXPORT bool ASEXT_CASRefCountedBaseClass_InternalRelease(void *ref)
{
	return g_pfn_CASRefCountedBaseClass_InternalRelease(ref);
}

C_DLLEXPORT void ASEXT_CScriptAny_Release(void *anywhat)
{
	g_pfn_CScriptAny_Release(anywhat);
}

C_DLLEXPORT void ASEXT_CScriptArray_Release(void* anywhat)
{
	g_pfn_CScriptArray_Release(anywhat);
}

C_DLLEXPORT void* ASEXT_GetCurrentContext()
{
	return g_pfn_asGetActiveContext();
}

C_DLLEXPORT void ASEXT_CScriptBuilder_DefineWord(CScriptBuilder* pthis, const char* word)
{
	SC_SERVER_DUMMYVAR;

	if(g_call_original_CScriptBuilder_DefineWord)
		g_call_original_CScriptBuilder_DefineWord(pthis, SC_SERVER_PASS_DUMMYARG word);
	else
		g_pfn_CScriptBuilder_DefineWord(pthis, SC_SERVER_PASS_DUMMYARG word);
}

void SC_SERVER_DECL NewCScriptBuilder_DefineWord(CScriptBuilder* pthis, SC_SERVER_DUMMYARG const char* word)
{
	ASEXT_CScriptBuilder_DefineWord(pthis, word);

	if (!strcmp(word, "SERVER"))
	{
		for (size_t i = 0; i < g_ScriptBuilderDefineCallbacks.size(); ++i)
		{
			g_ScriptBuilderDefineCallbacks[i](pthis);
		}
	}
}

C_DLLEXPORT CScriptDictionary* ASEXT_CScriptDictionary_Create(void* pScriptEngine)
{
	return g_pfn_CScriptDictionary_Create(pScriptEngine);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_AddRef(CScriptDictionary* pScriptDictionary)
{
	SC_SERVER_DUMMYVAR;
	g_pfn_CScriptDictionary_AddRef(pScriptDictionary SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_Release(CScriptDictionary* pScriptDictionary)
{
	SC_SERVER_DUMMYVAR;
	g_pfn_CScriptDictionary_Release(pScriptDictionary SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_Set(CScriptDictionary* pScriptDictionary, const CString* key, const void* val, int asTypeId)
{
	SC_SERVER_DUMMYVAR;
	g_pfn_CScriptDictionary_Set(pScriptDictionary, SC_SERVER_PASS_DUMMYARG key, val, asTypeId);
}

C_DLLEXPORT bool ASEXT_CScriptDictionary_Get(CScriptDictionary* pScriptDictionary, const CString* key, void* val, int asTypeId)
{
	SC_SERVER_DUMMYVAR;
	return g_pfn_CScriptDictionary_Get(pScriptDictionary, SC_SERVER_PASS_DUMMYARG key, val, asTypeId);
}

C_DLLEXPORT bool ASEXT_CScriptDictionary_Exists(CScriptDictionary* pScriptDictionary, const CString* key)
{
	SC_SERVER_DUMMYVAR;
	return g_pfn_CScriptDictionary_Exists(pScriptDictionary, SC_SERVER_PASS_DUMMYARG key);
}

C_DLLEXPORT bool ASEXT_CScriptDictionary_IsEmpty(CScriptDictionary* pScriptDictionary)
{
	SC_SERVER_DUMMYVAR;
	return g_pfn_CScriptDictionary_IsEmpty(pScriptDictionary SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT unsigned int ASEXT_CScriptDictionary_GetSize(CScriptDictionary* pScriptDictionary)
{
	SC_SERVER_DUMMYVAR;
	return g_pfn_CScriptDictionary_GetSize(pScriptDictionary SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_Delete(CScriptDictionary* pScriptDictionary, const CString* key)
{
	SC_SERVER_DUMMYVAR;
	g_pfn_CScriptDictionary_Delete(pScriptDictionary, SC_SERVER_PASS_DUMMYARG key);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_DeleteAll(CScriptDictionary* pScriptDictionary)
{
	SC_SERVER_DUMMYVAR;
	g_pfn_CScriptDictionary_DeleteAll(pScriptDictionary SC_SERVER_PASS_DUMMYARG2);
}

C_DLLEXPORT void ASEXT_CScriptDictionary_CIterator_begin(CScriptDictionary *pDict, CScriptDictionary_CIterator *itor)
{
	SC_SERVER_DUMMYVAR;
#ifdef _WIN32
	g_pfn_CScriptDictionary_CIterator_begin(pDict, SC_SERVER_PASS_DUMMYARG itor);
#else
	g_pfn_CScriptDictionary_CIterator_begin(itor, pDict);
#endif
}

C_DLLEXPORT void ASEXT_CScriptDictionary_CIterator_end(CScriptDictionary *pDict, CScriptDictionary_CIterator *itor)
{
	SC_SERVER_DUMMYVAR;
#ifdef _WIN32
	g_pfn_CScriptDictionary_CIterator_end(pDict, SC_SERVER_PASS_DUMMYARG itor);
#else
	g_pfn_CScriptDictionary_CIterator_end(itor, pDict);
#endif
}

C_DLLEXPORT bool ASEXT_CScriptDictionary_CIterator_operator_NE(CScriptDictionary_CIterator *a1, CScriptDictionary_CIterator *a2)
{
	return false;
}

C_DLLEXPORT bool ASEXT_CScriptDictionary_CIterator_GetValue(CScriptDictionary_CIterator *it, void *data, int typeId)
{
	return false;
}

C_DLLEXPORT const char *ASEXT_CScriptDictionary_CIterator_GetKey(CScriptDictionary_CIterator *it)
{
	SC_SERVER_DUMMYVAR;
	// GetKey returns void* (actually std::string&), we cast to const char* for the C_str
	void *pStdString = g_pfn_CScriptDictionary_CIterator_GetKey(it SC_SERVER_PASS_DUMMYARG2);
	return (const char *)pStdString;
}

C_DLLEXPORT void ASEXT_CScriptDictionary_CIterator_operator_PP(CScriptDictionary_CIterator *it)
{

}
