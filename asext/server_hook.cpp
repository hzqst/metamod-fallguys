#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include <vector>

#include "asext.h"

#include "serverdef.h"

PRIVATE_FUNCTION_DEFINE(CASHook_Call);
PRIVATE_FUNCTION_DEFINE(CASHook_CASHook);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectType);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectProperty);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectMethod);

bool g_ASDocInit = false;
std::vector<fnASDocInitCallback> g_ASDocInitCallbacks;
std::vector<CASHook *> g_ASHooks;

C_DLLEXPORT fnASEXT_CallHook ASEXT_CallHook = NULL;

C_DLLEXPORT void ASEXT_RegisterObjectMethod(void *pthis, const char *docs, const char *name, const char *func, void *pfn, int type)
{
	SC_SERVER_DUMMYVAR;
	CASMethodRegistration reg;
	reg.pfnMethod = pfn;
	g_call_original_CASDocumentation_RegisterObjectMethod((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, func, &reg, type);
}

C_DLLEXPORT void ASEXT_RegisterObjectType(void *pthis, const char *docs, const char *name, int unk, unsigned int flags)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectType((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, unk, flags);
}

C_DLLEXPORT void ASEXT_RegisterObjectProperty(void *pthis, const char *docs, const char *name, const char *prop, int offset)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectProperty((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, prop, offset);
}

int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int a4, unsigned int flags)
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

	return g_call_original_CASDocumentation_RegisterObjectType(pthis, SC_SERVER_PASS_DUMMYARG docs, name, a4, flags);
}

C_DLLEXPORT bool ASEXT_RegisterDocInitCallback(fnASDocInitCallback callback)
{
	if (g_ASDocInit)
		return false;

	g_ASDocInitCallbacks.emplace_back(callback);

	return true;
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

	g_pfn_CASHook_CASHook(hook, SC_SERVER_PASS_DUMMYARG type, ASHookFlag_MapScript | ASHookFlag_Plugin, domain, func, args, &reg);

	return hook;
}