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

#include <extdll.h>

#include <meta_api.h>

#include <interface.h>

#include <capstone/capstone.h>

#include "sdk_util.h"

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"
#include "soundengine.h"

#ifndef _WIN32//for debugging
//#include <unistd.h>
#endif

IMPORT_ASEXT_API_DEFINE();

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
	NULL,			// pfnGetStudioBlendingInterface 2022/07/02 Added by hzqst
	NULL,			// pfnGetStudioBlendingInterface_Post 2022/07/02 Added by hzqst
};

// Description of plugin
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"FallGuys",	// name
	"1.91",	// version
	"2024",	// date
	"hzqst",	// author
	"https://github.com/hzqst/metamod-fallguys",	// url
	"FGUYS",	// logtag, all caps please
	PT_ANYTIME,	// (when) loadable
	PT_STARTUP,	// (when) unloadable
};

// Global vars from metamod:
meta_globals_t *gpMetaGlobals = NULL;		// metamod globals
gamedll_funcs_t *gpGamedllFuncs = NULL;	// gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs = NULL;		// metamod utility functions
DLL_FUNCTIONS *gpDllFunctionsTable = NULL;
NEW_DLL_FUNCTIONS *gpNewDllFunctionsTable = NULL;

class CDisasmFindGotPltTargetContext
{
public:
	void* imageBase;
	void* imageEnd;
	char* gotplt;
	char* result;
};

void DisasmSingleCallback_FindGotPltTarget(void* inst, byte* address, size_t instLen, void* context)
{
	auto pinst = (struct cs_insn *)inst;
	auto ctx = (CDisasmFindGotPltTargetContext*)context;

	if ((pinst->id == X86_INS_MOV || pinst->id == X86_INS_LEA)
		&& pinst->detail->x86.op_count == 2
		&& pinst->detail->x86.operands[0].type == X86_OP_REG
		&& pinst->detail->x86.operands[1].type == X86_OP_MEM
		&& pinst->detail->x86.operands[1].mem.base != 0
		&& pinst->detail->x86.operands[1].mem.disp != 0
		)
	{
		auto candidate = ctx->gotplt + pinst->detail->x86.operands[1].mem.disp;
		if (candidate > ctx->imageBase && candidate < ctx->imageEnd)
		{
			ctx->result = candidate;
		}
	}

	else if (pinst->id == X86_INS_MOV
		&& pinst->detail->x86.op_count == 2
		&& pinst->detail->x86.operands[1].type == X86_OP_REG
		&& pinst->detail->x86.operands[0].type == X86_OP_MEM
		&& pinst->detail->x86.operands[0].mem.base != 0
		&& pinst->detail->x86.operands[0].mem.disp != 0
		)
	{
		auto candidate = ctx->gotplt + pinst->detail->x86.operands[0].mem.disp;
		if (candidate > ctx->imageBase && candidate < ctx->imageEnd)
		{
			ctx->result = candidate;
		}
	}
}

qboolean DisasmCallback_FindGotPltTarget(void* inst, byte* address, size_t instLen, int instCount, int depth, void* context)
{
	auto pinst = (struct cs_insn*)inst;
	auto ctx = (CDisasmFindGotPltTargetContext*)context;

	if (instCount < 15)
	{
		if ((pinst->id == X86_INS_MOV || pinst->id == X86_INS_LEA)
			&& pinst->detail->x86.op_count == 2
			&& pinst->detail->x86.operands[0].type == X86_OP_REG
			&& pinst->detail->x86.operands[1].type == X86_OP_MEM
			&& pinst->detail->x86.operands[1].mem.base != 0
			&& pinst->detail->x86.operands[1].mem.disp != 0
			)
		{
			auto candidate = ctx->gotplt + pinst->detail->x86.operands[1].mem.disp;
			if (candidate > ctx->imageBase && candidate < ctx->imageEnd)
			{
				ctx->result = candidate;
			}

			return TRUE;
		}

		else if (pinst->id == X86_INS_MOV
			&& pinst->detail->x86.op_count == 2
			&& pinst->detail->x86.operands[1].type == X86_OP_REG
			&& pinst->detail->x86.operands[0].type == X86_OP_MEM
			&& pinst->detail->x86.operands[0].mem.base != 0
			&& pinst->detail->x86.operands[0].mem.disp != 0
			)
		{
			auto candidate = ctx->gotplt + pinst->detail->x86.operands[1].mem.disp;
			if (candidate > ctx->imageBase && candidate < ctx->imageEnd)
			{
				ctx->result = candidate;
			}

			return TRUE;
		}
	}

	if (address[0] == 0xCC)
		return TRUE;

	if (address[0] == 0x90)
		return TRUE;

	return FALSE;
}

//thx OpenAI
static bool ParseInterfaceVersion(const char * interfaceVersion, int *pMajorVersion, int* pMinorVersion)
{
	char* endPtr = nullptr;

	*pMajorVersion = strtol(interfaceVersion, &endPtr, 10);

	if (endPtr == interfaceVersion || (*endPtr != '\0' && *endPtr != ':')) {
		return false;
	}

	if (*endPtr == ':') {
		*pMinorVersion = strtol(endPtr + 1, &endPtr, 10);
		if (endPtr == interfaceVersion + 1 || *endPtr != '\0') {
			return false;
		}
	}
	else {
		*pMinorVersion = 0;
	}

	return true;
}

// Metamod requesting info about this plugin:
//  ifvers			(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(const char * interfaceVersion, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs) 
{
	int iMajorVersion = 0, iMinorVersion = 0;
	if (!ParseInterfaceVersion(interfaceVersion, &iMajorVersion, &iMinorVersion))
	{
		pMetaUtilFuncs->pfnLogError(PLID, "Meta_Query failed to parse version string \"%s\"!", interfaceVersion);
		return FALSE;
	}

	if (iMajorVersion < 5 || (iMajorVersion == 5 && iMinorVersion < 16))
	{
		pMetaUtilFuncs->pfnLogError(PLID, "Meta_Query version too low! expect \"%s\" but got \"%s\"", "5:16", interfaceVersion);
		return FALSE;
	}

	// Give metamod our plugin_info struct
	*pPlugInfo=&Plugin_info;
	// Get metamod utility function table.
	gpMetaUtilFuncs=pMetaUtilFuncs;
	return TRUE;
}

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

	gpMetaUtilFuncs->pfnGetHookTables(PLID, NULL, &gpDllFunctionsTable, &gpNewDllFunctionsTable);

	auto engineHandle = gpMetaUtilFuncs->pfnGetEngineHandle();
	auto engineBase = gpMetaUtilFuncs->pfnGetEngineBase();

	if (!engineHandle)
	{
		LOG_ERROR(PLID, "engine handle not found!");
		return FALSE;
	}

	if (!engineBase)
	{
		LOG_ERROR(PLID, "engine base not found!");
		return FALSE;
	}

	auto engineSize = gpMetaUtilFuncs->pfnGetImageSize(engineBase);
	auto engineEnd = (char*)engineBase + engineSize;

	LOG_MESSAGE(PLID, "Current engine dll range: %p ~ %p!", engineBase, engineEnd);

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

	auto serverSize = gpMetaUtilFuncs->pfnGetImageSize(serverBase);
	auto serverEnd = (char*)serverBase + serverSize;

	LOG_MESSAGE(PLID, "Current server dll range: %p ~ %p!", serverBase, serverEnd);

	void* asextHandle = NULL;

#ifdef _WIN32
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.dll", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#else
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.so", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#endif
	if (!asextHandle)
	{
		LOG_ERROR(PLID, "asext dll handle not found!");
		return FALSE;
	}

	IMPORT_ASEXT_API(asext);

	auto CreateInterface = (CreateInterfaceFn)gpMetaUtilFuncs->pfnGetProcAddress(serverHandle, "CreateInterface");

	if (!CreateInterface)
	{
		LOG_ERROR(PLID, "CreateInterface not found!");
		return FALSE;
	}

	LOG_MESSAGE(PLID, "Current engine type: %s!", gpMetaUtilFuncs->pfnGetEngineType());

#ifdef _WIN32

	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, PM_PlaySoundFX_SERVER, -1);
	FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CPlayerMove_PlayStepSound, -1);

	FILL_FROM_SIGNATURED_CALLER_FROM_START(engine, build_number, 0);

	FILL_FROM_SIGNATURE(engine, SV_Physics);
	FILL_FROM_SIGNATURE(engine, SV_PushEntity);
	FILL_FROM_SIGNATURE(engine, SV_PushMove);
	FILL_FROM_SIGNATURE(engine, SV_PushRotate);
	FILL_FROM_SIGNATURE(engine, SV_WriteMovevarsToClient);

	if (g_pfn_build_number() >= 10152)
	{
		FILL_FROM_SIGNATURED_CALLER_FROM_END(engine, SV_SingleClipMoveToEntity_10152, -1);
	}
	else
	{
		FILL_FROM_SIGNATURED_CALLER_FROM_END(engine, SV_SingleClipMoveToEntity, -1);
	}

	VAR_FROM_SIGNATURE_FROM_START(engine, sv_models, 13);

	VAR_FROM_SIGNATURE_FROM_START(engine, host_frametime, 7);
	VAR_FROM_SIGNATURE_FROM_END(engine, pmovevars, 0);
	VAR_FROM_SIGNATURE_FROM_START(engine, sv_areanodes, 9);

	VAR_FROM_SIGNATURE_FROM_END(engine, pg_groupop, -8);
	VAR_FROM_SIGNATURE_FROM_END(engine, pg_groupmask, -2);

#else

	//Sven Co-op 5.16 rc1 and rc2 (10152 and 10182)
	if (CreateInterface("SCServerDLL003", nullptr) != nullptr)
	{
		LOG_MESSAGE(PLID, "SCServerDLL003 found! Using signatures for Sven Co-op 5.16");

		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, CPlayerMove_PlayStepSound, -1);
		FILL_FROM_SIGNATURED_CALLER_FROM_END(server, PM_PlaySoundFX_SERVER, -1);
	}
	else
	{
		LOG_MESSAGE(PLID, "SCServerDLL003 not found! Using symbols for Sven Co-op 5.15");

		FILL_FROM_SYMBOL(server, CPlayerMove_PlayStepSound);
		FILL_FROM_SYMBOL(server, PM_PlaySoundFX_SERVER);
	}

	FILL_FROM_SYMBOL_NO_CHECK(engine, build_number);

	if (!g_pfn_build_number)
	{
		if (!strcmp(gpMetaUtilFuncs->pfnGetEngineType(), "i686"))
		{
			FILL_FROM_SIGNATURED_TY_CALLER_FROM_START(engine, build_number, i686, 0);

			LOG_MESSAGE(PLID, "build_number found at %p!", g_pfn_build_number);

			LOG_MESSAGE(PLID, "Current engine build_number = %d!", g_pfn_build_number());

			FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(engine, SV_Physics, i686, -1);
			LOG_MESSAGE(PLID, "SV_Physics found at %p!", g_pfn_SV_Physics);

			FILL_FROM_SIGNATURED_TY_CALLER_FROM_START(engine, SV_PushEntity, i686, 0);
			LOG_MESSAGE(PLID, "SV_PushEntity found at %p!", g_pfn_SV_PushEntity);

			FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(engine, SV_PushMove, i686, -1);
			LOG_MESSAGE(PLID, "SV_PushMove found at %p!", g_pfn_SV_PushMove);

			FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(engine, SV_PushRotate, i686, -9);
			LOG_MESSAGE(PLID, "SV_PushRotate found at %p!", g_pfn_SV_PushRotate);

			FILL_FROM_SIGNATURED_TY_CALLER_FROM_START(engine, SV_WriteMovevarsToClient, i686, 3);
			LOG_MESSAGE(PLID, "SV_WriteMovevarsToClient found at %p!", g_pfn_SV_WriteMovevarsToClient);

			if (g_pfn_build_number() >= 10152)
			{
				FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(engine, SV_SingleClipMoveToEntity_10152, i686, -1);
				LOG_MESSAGE(PLID, "SV_SingleClipMoveToEntity_10152 found at %p!", g_pfn_SV_SingleClipMoveToEntity_10152);
			}
			else
			{
				FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(engine, SV_SingleClipMoveToEntity, i686, -1);
				LOG_MESSAGE(PLID, "SV_SingleClipMoveToEntity found at %p!", g_pfn_SV_SingleClipMoveToEntity);
			}

			auto gotplt_prolog = (char*)LOCATE_FROM_SIGNATURE(engine, gotplt_prolog_Signature);
			if (!gotplt_prolog)
			{
				LOG_ERROR(PLID, "gotplt_prolog not found in engine dll!");
				return FALSE;
			}

			//__x86_get_pc_thunk_
			auto pic_chunk_call = gotplt_prolog + 8;
			auto add_addr = pic_chunk_call + 5;
			auto got_plt = add_addr + *(int*)(add_addr + 2);
			LOG_MESSAGE(PLID, "got_plt found at %p!", got_plt);

			void* sv = NULL;

			if (1)
			{
				auto sv_models_addr = (char*)LOCATE_FROM_SIGNATURE(engine, sv_model_Signature);
				if (!sv_models_addr)
				{
					LOG_ERROR(PLID, "sv_models_addr not found in engine dll!");
					return FALSE;
				}
				if (!sv)
				{
					CDisasmFindGotPltTargetContext ctx = { 0 };
					ctx.imageBase = engineBase;
					ctx.imageEnd = engineEnd;
					ctx.gotplt = got_plt;

					gpMetaUtilFuncs->pfnDisasmSingleInstruction(sv_models_addr - 5, DisasmSingleCallback_FindGotPltTarget, &ctx);

					if (ctx.result)
					{
						sv = (decltype(sv))ctx.result;
					}
				}
				if (!sv)
				{
					CDisasmFindGotPltTargetContext ctx = { 0 };
					ctx.imageBase = engineBase;
					ctx.imageEnd = engineEnd;
					ctx.gotplt = got_plt;

					gpMetaUtilFuncs->pfnDisasmSingleInstruction(sv_models_addr - 6, DisasmSingleCallback_FindGotPltTarget, &ctx);

					if (ctx.result)
					{
						sv = (decltype(sv))ctx.result;
					}
				}
				if (!sv)
				{
					CDisasmFindGotPltTargetContext ctx = { 0 };
					ctx.imageBase = engineBase;
					ctx.imageEnd = engineEnd;
					ctx.gotplt = got_plt;

					gpMetaUtilFuncs->pfnDisasmSingleInstruction(sv_models_addr - 7, DisasmSingleCallback_FindGotPltTarget, &ctx);

					if (ctx.result)
					{
						sv = (decltype(sv))ctx.result;
					}
				}

				if (!sv)
				{
					LOG_ERROR(PLID, "sv not found in engine dll!");
					return FALSE;
				}

				LOG_MESSAGE(PLID, "sv found at %p!", sv);

				sv_models = (decltype(sv_models))((char*)sv + offset_sv_models);

				if (!sv_models)
				{
					LOG_ERROR(PLID, "sv_models not found in engine dll!");
					return FALSE;
				}

				LOG_MESSAGE(PLID, "sv_models found at %p!", sv_models);
			}

			if (1)
			{
				char pattern[] = host_frametime_Signature;

				auto searchBegin = (char*)engineBase;
				auto searchEnd = (char*)engineEnd;
				while (1)
				{
					auto pFound = LOCATE_FROM_SIGNATURE_FROM_FUNCTION(searchBegin, searchEnd - searchBegin, pattern);
					if (pFound)
					{
						auto pFoundNextInstruction = (char*)pFound + sizeof(pattern) - 1;

						CDisasmFindGotPltTargetContext ctx = { 0 };
						ctx.imageBase = engineBase;
						ctx.imageEnd = engineEnd;
						ctx.gotplt = got_plt;

						gpMetaUtilFuncs->pfnDisasmSingleInstruction(pFoundNextInstruction, DisasmSingleCallback_FindGotPltTarget, &ctx);

						if (ctx.result)
						{
							host_frametime = (decltype(host_frametime))ctx.result;
							break;
						}

						searchBegin = (char*)pFound + sizeof(pattern) - 1;
					}
					else
					{
						break;
					}
				}

				if (!host_frametime)
				{
					LOG_ERROR(PLID, "host_frametime not found in engine dll!");
					return FALSE;
				}
			}

			if (1)
			{
				CDisasmFindGotPltTargetContext ctx = { 0 };

				ctx.imageBase = engineBase;
				ctx.imageEnd = engineEnd;
				ctx.gotplt = got_plt;

				gpMetaUtilFuncs->pfnDisasmRanges((void*)g_pfn_SV_WriteMovevarsToClient, 0x150, DisasmCallback_FindGotPltTarget, 0, &ctx);

				if (ctx.result)
				{
					pmovevars = (decltype(pmovevars))ctx.result;
				}

				if (!pmovevars)
				{
					LOG_ERROR(PLID, "movevars not found in engine dll!");
					return FALSE;
				}
				LOG_MESSAGE(PLID, "movevars found at %p!", pmovevars);
			}

			if (1)
			{
				char pattern[] = sv_areanodes_Signature;

				auto searchBegin = (char*)engineBase;
				auto searchEnd = (char*)engineEnd;
				while (1)
				{
					auto pFound = LOCATE_FROM_SIGNATURE_FROM_FUNCTION(searchBegin, searchEnd - searchBegin, pattern);
					if (pFound)
					{
						auto pFoundNextInstruction = (char*)pFound + sizeof(pattern) - 1;

						CDisasmFindGotPltTargetContext ctx = { 0 };

						ctx.imageBase = engineBase;
						ctx.imageEnd = engineEnd;
						ctx.gotplt = got_plt;

						gpMetaUtilFuncs->pfnDisasmSingleInstruction(pFoundNextInstruction, DisasmSingleCallback_FindGotPltTarget, &ctx);

						if (ctx.result)
						{
							sv_areanodes = (decltype(sv_areanodes))ctx.result;
							break;
						}

						searchBegin = (char*)pFound + sizeof(pattern) - 1;
					}
					else
					{
						break;
					}
				}

				if (!sv_areanodes)
				{
					LOG_ERROR(PLID, "sv_areanodes not found in engine dll!");
					return FALSE;
				}
				LOG_MESSAGE(PLID, "sv_areanodes found at %p!", sv_areanodes);
			}

			if (1)
			{
				char pattern[] = PF_SetGroupMask_Signature;

				auto searchBegin = (char*)engineBase;
				auto searchEnd = (char*)engineEnd;
				while (1)
				{
					auto pFound = LOCATE_FROM_SIGNATURE_FROM_FUNCTION(searchBegin, searchEnd - searchBegin, pattern);
					if (pFound)
					{
						auto g_groupmask_instruction = (char*)pFound + 4;
						auto g_groupop_instruction = (char*)pFound + 14;

						if (1)
						{
							CDisasmFindGotPltTargetContext ctx = { 0 };

							ctx.imageBase = engineBase;
							ctx.imageEnd = engineEnd;
							ctx.gotplt = got_plt;

							gpMetaUtilFuncs->pfnDisasmSingleInstruction(g_groupmask_instruction, DisasmSingleCallback_FindGotPltTarget, &ctx);

							if (ctx.result)
							{
								pg_groupmask = (decltype(pg_groupmask))ctx.result;
							}
						}

						if (1)
						{
							CDisasmFindGotPltTargetContext ctx = { 0 };

							ctx.imageBase = engineBase;
							ctx.imageEnd = engineEnd;
							ctx.gotplt = got_plt;

							gpMetaUtilFuncs->pfnDisasmSingleInstruction(g_groupop_instruction, DisasmSingleCallback_FindGotPltTarget, &ctx);

							if (ctx.result)
							{
								pg_groupop = (decltype(pg_groupop))ctx.result;
							}
						}

						if (pg_groupop)
							break;

						searchBegin = (char*)pFound + sizeof(pattern) - 1;
					}
					else
					{
						break;
					}
				}

				if (!pg_groupmask)
				{
					LOG_ERROR(PLID, "g_groupmask not found in engine dll!");
					return FALSE;
				}
				LOG_MESSAGE(PLID, "g_groupmask found at %p!", pg_groupmask);

				if (!pg_groupop)
				{
					LOG_ERROR(PLID, "g_groupop not found in engine dll!");
					return FALSE;
				}
				LOG_MESSAGE(PLID, "g_groupop found at %p!", pg_groupop);
			}
		}
		else
		{
			LOG_ERROR(PLID, "engine_amd.so is not supported yet!");
			return FALSE;
		}
	}
	else
	{
		LOG_MESSAGE(PLID, "Current engine build_number = %d!", g_pfn_build_number());

		FILL_FROM_SYMBOL(engine, SV_Physics);
		FILL_FROM_SYMBOL(engine, SV_PushEntity);
		FILL_FROM_SYMBOL(engine, SV_PushMove);
		FILL_FROM_SYMBOL(engine, SV_PushRotate);
		FILL_FROM_SYMBOL(engine, SV_WriteMovevarsToClient);

		if (g_pfn_build_number() >= 10152)
		{
			FILL_FROM_SYMBOL(engine, SV_SingleClipMoveToEntity_10152);
		}
		else
		{
			FILL_FROM_SYMBOL(engine, SV_SingleClipMoveToEntity);
		}

		void* sv = NULL;

		VAR_FROM_SYMBOL(engine, sv);

		sv_models = (decltype(sv_models))((char*)sv + offset_sv_models);

		VAR_FROM_SYMBOL(engine, host_frametime);
		VAR_FROM_SYMBOL(engine, pmovevars);
		VAR_FROM_SYMBOL(engine, sv_areanodes);

		VAR_FROM_SYMBOL(engine, pg_groupop);
		VAR_FROM_SYMBOL(engine, pg_groupmask);
	}

#endif


#ifndef _WIN32//for debugging
	//sleep(15);
#endif

	LoadFMOD();
	InstallEngineHooks();
	InstallServerHooks();
	RegisterAngelScriptMethods();
	RegisterAngelScriptHooks();

	return TRUE;
}

// Metamod detaching plugin from the server.
// now		(given) current phase, ie during map, etc
// reason	(given) why detaching (refresh, console unload, forced unload, etc)
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME /* now */, 
		PL_UNLOAD_REASON /* reason */) 
{
	gPhysicsManager.Shutdown();
	UninstallServerHooks();
	UninstallEngineHooks();
	UnloadFMOD();

	return TRUE;
}