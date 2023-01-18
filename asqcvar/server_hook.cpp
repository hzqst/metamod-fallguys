#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "asqcvar.h"

bool SC_SERVER_DECL CASEngineFuncs__SetQueryCvar2Callback(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int request_id, aslScriptFunction *aslfn)
{
	return ASQCvar_SetQueryCvar2Callback(request_id, aslfn);
}

void RegisterAngelScriptMethods()
{
	ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {
		
		ASEXT_RegisterFuncDef(pASDoc, "callback for svc_querycvar2 response", "void QueryCvar2ResponseCallback(CBasePlayer @pPlayer, int request_id, string cvar, string value)");

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set callback for svc_querycvar2 response", "CEngineFuncs", "bool SetQueryCvar2Callback(int request_id, QueryCvar2ResponseCallback @callback )",
			(void *)CASEngineFuncs__SetQueryCvar2Callback, 3);

	});
}