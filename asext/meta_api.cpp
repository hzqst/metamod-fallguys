// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// meta_api.cpp - minimal implementation of metamod's plugin interface

// This is intended to illustrate the (more or less) bare minimum code
// required for a valid metamod plugin, and is targeted at those who want
// to port existing HL/SDK DLL code to run as a metamod plugin.

/*
 * Copyright (c) 2001-2006 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <interface.h>

#include "sdk_util.h"		// UTIL_LogPrintf, etc

#include "asext.h"

#include "serverdef.h"

hook_t *g_phook_CASDocumentation_RegisterObjectType = NULL;
hook_t *g_phook_CASDirectoryList_CreateDirectory = NULL;

// Must provide at least one of these..
static META_FUNCTIONS gMetaFunctionTable = {
	NULL,			// pfnGetEntityAPI				HL SDK; called before game DLL
	NULL,			// pfnGetEntityAPI_Post			META; called after game DLL
	GetEntityAPI2,	// pfnGetEntityAPI2				HL SDK2; called before game DLL
	GetEntityAPI2_Post,			// pfnGetEntityAPI2_Post		META; called after game DLL
	GetNewDLLFunctions,			// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
	NULL,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
	GetEngineFunctions,	// pfnGetEngineFunctions	META; called before HL engine
	NULL,			// pfnGetEngineFunctions_Post	META; called after HL engine
};

// Description of plugin
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"AngelScriptExt",	// name
	"1.7",	// version
	"2024",	// date
	"hzqst",	// author
	"https://github.com/hzqst/metamod-fallguys",	// url
	"ASEXT",	// logtag, all caps please
	PT_ANYTIME,	// (when) loadable
	PT_STARTUP,	// (when) unloadable
};

// Global vars from metamod:
meta_globals_t *gpMetaGlobals;		// metamod globals
gamedll_funcs_t *gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs;		// metamod utility functions

// Metamod requesting info about this plugin:
//  ifvers			(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(char * interfaceVersion, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs) 
{
	if (0 != strcmp(interfaceVersion, META_INTERFACE_VERSION))
	{
		pMetaUtilFuncs->pfnLogError(PLID, "Meta_Query version mismatch! expect %s but got %s", META_INTERFACE_VERSION, interfaceVersion);
		return FALSE;
	}

	// Give metamod our plugin_info struct
	*pPlugInfo=&Plugin_info;
	// Get metamod utility function table.
	gpMetaUtilFuncs=pMetaUtilFuncs;
	return TRUE;
}

// Metamod attaching plugin to the server.
//  now				(given) current phase, ie during map, during changelevel, or at startup
//  pFunctionTable	(requested) table of function tables this plugin catches
//  pMGlobals		(given) global vars from metamod
//  pGamedllFuncs	(given) copy of function tables from game dll

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME /* now */,
	META_FUNCTIONS* pFunctionTable, meta_globals_t* pMGlobals,
	gamedll_funcs_t* pGamedllFuncs)
{
	if (!pMGlobals) {
		LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
		return FALSE;
	}

	gpMetaGlobals = pMGlobals;
	if (!pFunctionTable) {
		LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
		return FALSE;
	}

	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
	gpGamedllFuncs = pGamedllFuncs;

	auto serverHandle = gpMetaUtilFuncs->pfnGetGameDllHandle();
	auto serverBase = gpMetaUtilFuncs->pfnGetGameDllBase();

	if (!serverHandle)
	{
		LOG_ERROR(PLID, "server handle not found!");
		return FALSE;
	}

	if (!serverBase)
	{
		LOG_ERROR(PLID, "server base not found!");
		return FALSE;
	}

	auto CreateInterface = (CreateInterfaceFn)gpMetaUtilFuncs->pfnGetProcAddress(serverHandle, "CreateInterface");

	if (!CreateInterface)
	{
		LOG_ERROR(PLID, "CreateInterface not found!");
		return FALSE;
	}

#ifdef _WIN32

	FILL_FROM_SIGNATURE(server, CASHook_CASHook);
	FILL_FROM_SIGNATURE(server, CASHook_Call);
	FILL_FROM_SIGNATURE(server, CString_Assign);
	FILL_FROM_SIGNATURE(server, CString_dtor);

	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectType, -1);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectProperty, -7);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterGlobalProperty, -15);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectMethod, -7);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectBehaviour, -8);
	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterFuncDef, 0);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterEnum, -7);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterEnumValue, -7);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDirectoryList_CreateDirectory, -1);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASFunction_Create, -1);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASBaseCallable_Call, -1);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASRefCountedBaseClass_InternalRelease, -7);
	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptAny_Release, 7);
	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptArray_Release, 0);

	VAR_FROM_SIGNATURE_FROM_START(server, g_pServerManager, 5);

#else
	//Sven Co-op 5.16 rc1 and rc2 (10152 and 10182)
	if (CreateInterface("SCServerDLL003", nullptr) != nullptr)
	{
		LOG_MESSAGE(PLID, "SCServerDLL003 found! Using signatures for Sven Co-op 5.16");

		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASHook_CASHook, -1);

		LOG_MESSAGE(PLID, "222");

		FILL_FROM_SIGNATURE(server, CASHook_Call);

		LOG_MESSAGE(PLID, "333");

		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CString_Assign, -1);

		LOG_MESSAGE(PLID, "444");

		FILL_FROM_SIGNATURE(server, CString_dtor);

		LOG_MESSAGE(PLID, "555");

		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectType, -1);
		LOG_MESSAGE(PLID, "666");
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectProperty, -8);
		LOG_MESSAGE(PLID, "777");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterGlobalProperty, 9);
		LOG_MESSAGE(PLID, "888");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterObjectMethod, 0);
		LOG_MESSAGE(PLID, "999");
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectBehaviour, -1);
		LOG_MESSAGE(PLID, "000");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterFuncDef, 0);
		LOG_MESSAGE(PLID, "1111");
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterEnum, -13);
		LOG_MESSAGE(PLID, "2222");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterEnumValue, 7);
		LOG_MESSAGE(PLID, "3333");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDirectoryList_CreateDirectory, 0);
		LOG_MESSAGE(PLID, "4444");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASFunction_Create, 0);
		LOG_MESSAGE(PLID, "5555");
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASBaseCallable_Call, -8);
		LOG_MESSAGE(PLID, "6666");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASRefCountedBaseClass_InternalRelease, 3);
		LOG_MESSAGE(PLID, "7777");
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptAny_Release, 0);
		LOG_MESSAGE(PLID, "8888");
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CScriptArray_Release, -8);

		LOG_MESSAGE(PLID, "CScriptArray_Release found!");

		char pattern_CASHook_VCall[] = "\x83\xEC\x2A\xE8\x2A\x2A\x2A\x2A\x81\x2A\x2A\x2A\x2A\x2A\x8B\x2A\x24\x2A\x8B\x2A\x2A\x2A\x2A\x00\x85\x2A\x74\x2A\x0F\x2A\x2A\x06";
		auto CASHook_VCall = (char *)LOCATE_FROM_SIGNATURE(server, pattern_CASHook_VCall);
		if (!CASHook_VCall)
		{
			LOG_ERROR(PLID, "CHook_VCall not found!");
			return FALSE;
		}
		LOG_MESSAGE(PLID, "CASHook_VCall at %p!", CASHook_VCall);

		//__x86_get_pc_thunk_
		auto pic_chunk_call = CASHook_VCall + 3;
		//auto pic_chunk = gpMetaUtilFuncs->pfnGetNextCallAddr(pic_chunk_call, 1);
		auto add_addr = pic_chunk_call + 5;
		auto got_plt = add_addr + *(int*)(add_addr + 2);
		auto mov_ebp_addr = CASHook_VCall + 18;
		g_pServerManager = (decltype(g_pServerManager))(got_plt + *(int*)(mov_ebp_addr + 2));

		LOG_MESSAGE(PLID, "g_pServerManager at %p!", g_pServerManager);

	}
	else
	{
		LOG_MESSAGE(PLID, "SCServerDLL003 not found! Using symbols for Sven Co-op 5.15");

		FILL_FROM_SYMBOL(server, CASHook_CASHook);
		FILL_FROM_SYMBOL(server, CASHook_Call);
		FILL_FROM_SYMBOL(server, CString_Assign);
		FILL_FROM_SYMBOL(server, CString_dtor);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterObjectType);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterObjectProperty);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterGlobalProperty);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterObjectMethod);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterObjectBehaviour);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterFuncDef);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterEnum);
		FILL_FROM_SYMBOL(server, CASDocumentation_RegisterEnumValue);
		FILL_FROM_SYMBOL(server, CASDirectoryList_CreateDirectory);
		FILL_FROM_SYMBOL(server, CASFunction_Create);
		FILL_FROM_SYMBOL(server, CASBaseCallable_Call);
		FILL_FROM_SYMBOL(server, CASRefCountedBaseClass_InternalRelease);
		FILL_FROM_SYMBOL(server, CScriptAny_Release);
		FILL_FROM_SYMBOL(server, CScriptArray_Release);

		VAR_FROM_SYMBOL(server, g_pServerManager);
	}

#endif

	ASEXT_CallHook = (fnASEXT_CallHook)g_call_original_CASHook_Call;

	ASEXT_CallCASBaseCallable = (fnASEXT_CallCASBaseCallable)g_call_original_CASBaseCallable_Call;

	INSTALL_INLINEHOOK(CASDocumentation_RegisterObjectType);
	INSTALL_INLINEHOOK(CASDirectoryList_CreateDirectory);

	return TRUE;
}

// Metamod detaching plugin from the server.
// now		(given) current phase, ie during map, etc
// reason	(given) why detaching (refresh, console unload, forced unload, etc)
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME /* now */, 
		PL_UNLOAD_REASON /* reason */) 
{
	//Nope, AngelScript doesn't provide unloading procedures

	UNINSTALL_HOOK(CASDocumentation_RegisterObjectType);
	UNINSTALL_HOOK(CASDirectoryList_CreateDirectory);

	return TRUE;
}