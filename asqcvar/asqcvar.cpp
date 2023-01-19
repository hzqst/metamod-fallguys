#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <map>

#include "enginedef.h"
#include "serverdef.h"
#include "asqcvar.h"

void * g_QueryCvarHook = NULL;
void * g_QueryCvar2Hook = NULL;

std::map<int, CASFunction *> g_QueryCvar2Callbacks;

void ASQCvar_Shutdown()
{
	for (auto itor = g_QueryCvar2Callbacks.begin(); itor != g_QueryCvar2Callbacks.end(); )
	{
		auto callback = itor->second;

		ASEXT_DereferenceCASFunction(callback);

		itor = g_QueryCvar2Callbacks.erase(itor);
	}
}

bool ASQCvar_SetQueryCvar2Callback(int request_id, aslScriptFunction *aslfn)
{
	if (g_QueryCvar2Callbacks.find(request_id) != g_QueryCvar2Callbacks.end())
		return false;

	auto callback = ASEXT_CreateCASFunction(aslfn, ASEXT_GetServerManager()->curModule, 1);

	g_QueryCvar2Callbacks[request_id] = callback;

	return true;
}

void ASQCvar_CallQueryCvar2Callback(void *pPlayer, int request_id, const char *cvarName, const char *value)
{
	auto itor = g_QueryCvar2Callbacks.find(request_id);
	if (itor == g_QueryCvar2Callbacks.end())
		return;

	auto callback = itor->second;

	if (ASEXT_CallCASBaseCallable && (*ASEXT_CallCASBaseCallable))
	{
		CString str_cvar = { 0 };
		str_cvar.assign(cvarName, strlen(cvarName));

		CString str_val = { 0 };
		str_val.assign(value, strlen(value));

		(*ASEXT_CallCASBaseCallable)(callback, 0, pPlayer, request_id, &str_cvar, &str_val);

		str_cvar.dtor();
		str_val.dtor();
	}

	if (ASEXT_CASRefCountedBaseClass_InternalRelease(callback->getReference()))
	{
		ASEXT_DereferenceCASFunction(callback);
	}

	g_QueryCvar2Callbacks.erase(itor);
}

void RegisterAngelScriptHooks()
{
	g_QueryCvarHook = ASEXT_RegisterHook("Get called when svc_querycvar response is received", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "QueryCvar", "CBasePlayer @pPlayer, const string &in value");
	g_QueryCvar2Hook = ASEXT_RegisterHook("Get called when svc_querycvar2 response is received", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "QueryCvar2", "CBasePlayer @pPlayer, int requestId, const string &in cvarName, const string &in value");
}