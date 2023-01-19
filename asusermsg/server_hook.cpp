#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "asusermsg.h"

bool SC_SERVER_DECL CASEngineFuncs__RegisterUserMsgHook(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int msgType, aslScriptFunction *aslfn)
{
	return g_UserMsgHookManager.RegisterUserMsgHook(msgType, aslfn);
}

bool SC_SERVER_DECL CASEngineFuncs__EnableUserMsgHook(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int msgType, bool bEnabled)
{
	return g_UserMsgHookManager.EnableUserMsgHook(msgType, bEnabled);
}

bool SC_SERVER_DECL CASEngineFuncs__EnableUserMsgHookGlobal(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, bool bEnabled)
{
	return g_UserMsgHookManager.EnableUserMsgHookGlobal(bEnabled);
}
bool SC_SERVER_DECL CASEngineFuncs__BlockCurrentUserMsg(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return g_UserMsgHookManager.BlockCurrentUserMsg();
}

bool SC_SERVER_DECL CASEngineFuncs__GetUserMsgId(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, CString *str)
{
	return g_UserMsgHookManager.GetUserMsgId(str->c_str());
}

int SC_SERVER_DECL UserMsgArgs_GetArgCount(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return g_UserMsgHookManager.GetArgCount();
}

int SC_SERVER_DECL UserMsgArgs_GetArgType(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int index)
{
	return g_UserMsgHookManager.GetArgType(index);
}

int SC_SERVER_DECL UserMsgArgs_GetArgInteger(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int index)
{
	return g_UserMsgHookManager.GetArgInteger(index);
}

float SC_SERVER_DECL UserMsgArgs_GetArgFloat(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int index)
{
	return g_UserMsgHookManager.GetArgFloat(index);
}

void SC_SERVER_DECL UserMsgArgs_GetArgString(void* pthis SC_SERVER_DUMMYARG_NOCOMMA, int index, CString *str)
{
	g_UserMsgHookManager.GetArgString(index, str);
}

void RegisterAngelScriptMethods()
{
	ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {
		
		ASEXT_RegisterFuncDef(pASDoc, "callback for usermsg hook", "void UserMsgHookCallback(int msgDest, int msgType, Vector origin, edict_t @pEdict)");

		ASEXT_RegisterObjectMethod(pASDoc, "Register a usermsg hook", "CEngineFuncs", "bool RegisterUserMsgHook(int msgType, UserMsgHookCallback @callback )", (void *)CASEngineFuncs__RegisterUserMsgHook, 3);

		ASEXT_RegisterObjectMethod(pASDoc, "Enable a usermsg hook", "CEngineFuncs", "bool EnableUserMsgHook(int msgType, bool enabled)", (void *)CASEngineFuncs__EnableUserMsgHook, 3);

		ASEXT_RegisterObjectMethod(pASDoc, "Enable usermsg hook globally", "CEngineFuncs", "bool EnableUserMsgHookGlobal(bool enabled)", (void *)CASEngineFuncs__EnableUserMsgHookGlobal, 3);

		ASEXT_RegisterObjectMethod(pASDoc, "Block original usermsg in hook handler", "CEngineFuncs", "bool BlockCurrentUserMsg()", (void *)CASEngineFuncs__BlockCurrentUserMsg, 3);

		ASEXT_RegisterObjectMethod(pASDoc, "Get usermsg id by name", "CEngineFuncs", "int GetUserMsgId(const string& in name)", (void *)CASEngineFuncs__GetUserMsgId, 3);

		ASEXT_RegisterObjectMethod(pASDoc, "", "CEngineFuncs", "int GetUserMsgArgCount()", (void *)UserMsgArgs_GetArgCount, 3);
		ASEXT_RegisterObjectMethod(pASDoc, "", "CEngineFuncs", "int GetUserMsgArgType(int index)", (void *)UserMsgArgs_GetArgType, 3);
		ASEXT_RegisterObjectMethod(pASDoc, "", "CEngineFuncs", "int GetUserMsgArgInteger(int index)", (void *)UserMsgArgs_GetArgInteger, 3);
		ASEXT_RegisterObjectMethod(pASDoc, "", "CEngineFuncs", "float GetUserMsgArgFloat(int index)", (void *)UserMsgArgs_GetArgFloat, 3);
		ASEXT_RegisterObjectMethod(pASDoc, "", "CEngineFuncs", "void GetUserMsgArgString(int index, string& out str)", (void *)UserMsgArgs_GetArgString, 3);

	});
}