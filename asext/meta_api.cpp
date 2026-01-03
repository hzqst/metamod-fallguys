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
hook_t *g_phook_CScriptBuilder_DefineWord = NULL;

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
	"2.1",	// version
	"2026",	// date
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
C_DLLEXPORT int Meta_Query(const char * interfaceVersion, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs) 
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

	auto serverCodeBase = gpMetaUtilFuncs->pfnGetCodeBase(serverBase);
	auto serverCodeSize = gpMetaUtilFuncs->pfnGetCodeSize(serverBase);

	auto serverCodeEnd = (char*)serverCodeBase + serverCodeSize;

	LOG_MESSAGE(PLID, "Current server code range: %p ~ %p!", serverCodeBase, serverCodeEnd);

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

	FILL_FROM_SIGNATURE(server, asGetActiveContext);

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

	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptBuilder_DefineWord, 8);

	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_Create, 3);
	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_AddRef, 6);
	FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_Release, 10);
	FILL_FROM_SIGNATURE(server, CScriptDictionary_Set);
	FILL_FROM_SIGNATURE(server, CScriptDictionary_Get);
	FILL_FROM_SIGNATURE(server, CScriptDictionary_Exists);
	if (g_pfn_CScriptDictionary_Exists)
	{
		FILL_FROM_SIGNATURE_FROM_FUNCTION(server, CScriptDictionary_IsEmpty, g_pfn_CScriptDictionary_Exists, 0x100);
		FILL_FROM_SIGNATURE_FROM_FUNCTION(server, CScriptDictionary_GetSize, g_pfn_CScriptDictionary_Exists, 0x100);
	}
	FILL_FROM_SIGNATURE(server, CScriptDictionary_Delete);
	FILL_FROM_SIGNATURE(server, CScriptDictionary_DeleteAll);

	VAR_FROM_SIGNATURE_FROM_START(server, g_pServerManager, 5);

#else
	//Sven Co-op 5.16 rc1 and rc2 (10152 and 10182)
	if (CreateInterface("SCServerDLL003", nullptr) != nullptr)
	{
		LOG_MESSAGE(PLID, "SCServerDLL003 found! Using signatures for Sven Co-op 5.16");

		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASHook_CASHook, -1);
		FILL_FROM_SIGNATURE(server, CASHook_Call);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CString_Assign, -1);
		FILL_FROM_SIGNATURE(server, CString_dtor);
		FILL_FROM_SIGNATURE(server, asGetActiveContext);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectType, -1);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterObjectProperty, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterGlobalProperty, 9);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterObjectMethod, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterObjectBehaviour, -1);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterFuncDef, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASDocumentation_RegisterEnum, -13);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDocumentation_RegisterEnumValue, 7);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASDirectoryList_CreateDirectory, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASFunction_Create, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CASBaseCallable_Call, -8);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CASRefCountedBaseClass_InternalRelease, 3);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptAny_Release, 0);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CScriptArray_Release, -8);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CScriptBuilder_DefineWord, -1);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_Create, 8);
		FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_AddRef, 13);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_Release);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CScriptDictionary_Set, -1);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_Get);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_Exists);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_IsEmpty);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_GetSize);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_Delete);
		FILL_FROM_SIGNATURE(server, CScriptDictionary_DeleteAll);

		char pattern_CASHook_VCall[] = "\x83\xEC\x2A\xE8\x2A\x2A\x2A\x2A\x81\x2A\x2A\x2A\x2A\x2A\x8B\x2A\x24\x2A\x8B\x2A\x2A\x2A\x2A\x00\x85\x2A\x74\x2A\x0F\x2A\x2A\x06";
		auto CASHook_VCall = (char *)LOCATE_FROM_SIGNATURE(server, pattern_CASHook_VCall);
		if (!CASHook_VCall)
		{
			LOG_ERROR(PLID, "CHook_VCall not found!");
			return FALSE;
		}

		//__x86_get_pc_thunk_
		auto pic_chunk_call = CASHook_VCall + 3;
		//auto pic_chunk = gpMetaUtilFuncs->pfnGetNextCallAddr(pic_chunk_call, 1);
		auto add_addr = pic_chunk_call + 5;
		auto got_plt = add_addr + *(int*)(add_addr + 2);

		LOG_MESSAGE(PLID, "got_plt found at %p!", got_plt);

		auto mov_ebp_addr = CASHook_VCall + 18;
		g_pServerManager = (decltype(g_pServerManager))(got_plt + *(int*)(mov_ebp_addr + 2));

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
		FILL_FROM_SYMBOL(server, CScriptBuilder_DefineWord);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Create);
		FILL_FROM_SYMBOL(server, CScriptDictionary_AddRef);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Release);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Set);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Get);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Exists);
		FILL_FROM_SYMBOL(server, CScriptDictionary_IsEmpty);
		FILL_FROM_SYMBOL(server, CScriptDictionary_GetSize);
		FILL_FROM_SYMBOL(server, CScriptDictionary_Delete);
		FILL_FROM_SYMBOL(server, CScriptDictionary_DeleteAll);

		VAR_FROM_SYMBOL(server, g_pServerManager);
	}

#endif

	ASEXT_CallHook = (fnASEXT_CallHook)g_call_original_CASHook_Call;

	ASEXT_CallCASBaseCallable = (fnASEXT_CallCASBaseCallable)g_call_original_CASBaseCallable_Call;

	LOG_MESSAGE(PLID, "CASHook_CASHook found at %p", g_pfn_CASHook_CASHook);
	LOG_MESSAGE(PLID, "CASHook_Call found at %p", g_pfn_CASHook_Call);
	LOG_MESSAGE(PLID, "CString_Assign found at %p", g_pfn_CString_Assign);
	LOG_MESSAGE(PLID, "CString_dtor found at %p", g_pfn_CString_dtor);
	LOG_MESSAGE(PLID, "asGetActiveContext found at %p", g_pfn_asGetActiveContext);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterObjectType found at %p", g_pfn_CASDocumentation_RegisterObjectType);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterObjectProperty found at %p", g_pfn_CASDocumentation_RegisterObjectProperty);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterGlobalProperty found at %p", g_pfn_CASDocumentation_RegisterGlobalProperty);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterObjectMethod found at %p", g_pfn_CASDocumentation_RegisterObjectMethod);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterObjectBehaviour found at %p", g_pfn_CASDocumentation_RegisterObjectBehaviour);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterFuncDef found at %p", g_pfn_CASDocumentation_RegisterFuncDef);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterEnum found at %p", g_pfn_CASDocumentation_RegisterEnum);
	LOG_MESSAGE(PLID, "CASDocumentation_RegisterEnumValue found at %p", g_pfn_CASDocumentation_RegisterEnumValue);
	LOG_MESSAGE(PLID, "CASDirectoryList_CreateDirectory found at %p", g_pfn_CASDirectoryList_CreateDirectory);
	LOG_MESSAGE(PLID, "CASFunction_Create found at %p", g_pfn_CASFunction_Create);
	LOG_MESSAGE(PLID, "CASBaseCallable_Call found at %p", g_pfn_CASBaseCallable_Call);
	LOG_MESSAGE(PLID, "CASRefCountedBaseClass_InternalRelease found at %p", g_pfn_CASRefCountedBaseClass_InternalRelease);
	LOG_MESSAGE(PLID, "CScriptAny_Release found at %p", g_pfn_CScriptAny_Release);
	LOG_MESSAGE(PLID, "CScriptArray_Release found at %p", g_pfn_CScriptArray_Release);
	LOG_MESSAGE(PLID, "CScriptBuilder_DefineWord found at %p", g_pfn_CScriptBuilder_DefineWord);
	LOG_MESSAGE(PLID, "CScriptDictionary_Create found at %p", g_pfn_CScriptDictionary_Create);
	LOG_MESSAGE(PLID, "CScriptDictionary_AddRef found at %p", g_pfn_CScriptDictionary_AddRef);
	LOG_MESSAGE(PLID, "CScriptDictionary_Release found at %p", g_pfn_CScriptDictionary_Release);
	LOG_MESSAGE(PLID, "CScriptDictionary_Set found at %p", g_pfn_CScriptDictionary_Set);
	LOG_MESSAGE(PLID, "CScriptDictionary_Get found at %p", g_pfn_CScriptDictionary_Get);
	LOG_MESSAGE(PLID, "CScriptDictionary_Exists found at %p", g_pfn_CScriptDictionary_Exists);
	LOG_MESSAGE(PLID, "CScriptDictionary_IsEmpty found at %p", g_pfn_CScriptDictionary_IsEmpty);
	LOG_MESSAGE(PLID, "CScriptDictionary_GetSize found at %p", g_pfn_CScriptDictionary_GetSize);
	LOG_MESSAGE(PLID, "CScriptDictionary_Delete found at %p", g_pfn_CScriptDictionary_Delete);
	LOG_MESSAGE(PLID, "CScriptDictionary_DeleteAll found at %p", g_pfn_CScriptDictionary_DeleteAll);
	LOG_MESSAGE(PLID, "g_pServerManager found at %p!", g_pServerManager);

	INSTALL_INLINEHOOK(CASDocumentation_RegisterObjectType);
	INSTALL_INLINEHOOK(CASDirectoryList_CreateDirectory);
	INSTALL_INLINEHOOK(CScriptBuilder_DefineWord);

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
	UNINSTALL_HOOK(CScriptBuilder_DefineWord);

	return TRUE;
}