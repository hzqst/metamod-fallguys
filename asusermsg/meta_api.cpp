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

#include "sdk_util.h"		// UTIL_LogPrintf, etc

#include "enginedef.h"
#include "serverdef.h"
#include "asusermsg.h"

IMPORT_ASEXT_API_DEFINE();

// Must provide at least one of these..
static META_FUNCTIONS gMetaFunctionTable = {
	NULL,			// pfnGetEntityAPI				HL SDK; called before game DLL
	NULL,			// pfnGetEntityAPI_Post			META; called after game DLL
	NULL,	// pfnGetEntityAPI2				HL SDK2; called before game DLL
	NULL,			// pfnGetEntityAPI2_Post		META; called after game DLL
	GetNewDLLFunctions,			// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
	NULL,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
	GetEngineFunctions,			// pfnGetEngineFunctions	META; called before HL engine
	NULL,			// pfnGetEngineFunctions_Post	META; called after HL engine
	NULL,			// pfnGetStudioBlendingInterface 2022/07/02 Added by hzqst
	NULL,			// pfnGetStudioBlendingInterface_Post 2022/07/02 Added by hzqst
};

// Description of plugin
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"AngelScriptUserMsg",	// name
	"1.0",	// version
	"2024",	// date
	"hzqst",	// author
	"https://github.com/hzqst/metamod-fallguys",	// url
	"ASUSERMSG",	// logtag, all caps please
	PT_ANYTIME,	// (when) loadable
	PT_STARTUP,	// (when) unloadable
};

// Global vars from metamod:
meta_globals_t *gpMetaGlobals;		// metamod globals
gamedll_funcs_t *gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs;		// metamod utility functions

//thx OpenAI
static bool ParseInterfaceVersion(const char* interfaceVersion, int* pMajorVersion, int* pMinorVersion)
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

	if (iMajorVersion < 5 || (iMajorVersion == 5 && iMinorVersion < 14))
	{
		pMetaUtilFuncs->pfnLogError(PLID, "Meta_Query version too low! expect \"%s\" but got \"%s\"", "5:14", interfaceVersion);
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

	void *asextHandle = NULL;

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

	RegisterAngelScriptMethods();

	return TRUE;
}

// Metamod detaching plugin from the server.
// now		(given) current phase, ie during map, etc
// reason	(given) why detaching (refresh, console unload, forced unload, etc)
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME /* now */, 
		PL_UNLOAD_REASON /* reason */) 
{
	return TRUE;
}