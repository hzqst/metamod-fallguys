// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// mutil.h - prototypes for utility functions to provide to plugins

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

#ifndef MUTIL_H
#define MUTIL_H

#include "comp_dep.h"
#include "plinfo.h"		// plugin_info_t, etc
#include "mhook.h"		// game_event_t, etc
#include "sdk_util.h"	// hudtextparms_t, etc

// max buffer size for printed messages
#define MAX_LOGMSG_LEN  1024

//2022-07 Added by hzqst
typedef struct hook_s
{
	int iType;
	qboolean bCommitted;
	void *pOldFuncAddr;
	void *pNewFuncAddr;
	void **pOrginalCall;
	void *pClass;
	int iTableIndex;
	int iFuncIndex;
	void *hModule;
	const char *pszModuleName;
	const char *pszFuncName;
	struct hook_s *pNext;
	void *pInfo;
}hook_t;

#define MH_HOOK_INLINE 1
#define MH_HOOK_VFTABLE 2
#define MH_HOOK_IAT 3

typedef void(*fnDisasmSingleCallback)(void *inst, byte *address, size_t instLen, void *context);
typedef qboolean(*fnDisasmCallback)(void *inst, byte *address, size_t instLen, int instCount, int depth, void *context);
typedef qboolean(*fnFindAddressCallback)(byte *address);

// For GetGameInfo:
typedef enum {
	GINFO_NAME = 0,
	GINFO_DESC,
	GINFO_GAMEDIR,
	GINFO_DLL_FULLPATH,
	GINFO_DLL_FILENAME,
	GINFO_REALDLL_FULLPATH,
} ginfo_t;

// Meta Utility Function table type.
typedef struct meta_util_funcs_s {
	void		(*pfnLogConsole)		(plid_t plid, const char *fmt, ...);
	void		(*pfnLogMessage)		(plid_t plid, const char *fmt, ...);
	void		(*pfnLogError)			(plid_t plid, const char *fmt, ...);
	void		(*pfnLogDeveloper)		(plid_t plid, const char *fmt, ...);
	void		(*pfnCenterSay)			(plid_t plid, const char *fmt, ...);
	void		(*pfnCenterSayParms)	(plid_t plid, hudtextparms_t tparms, 
											const char *fmt, ...);
	void		(*pfnCenterSayVarargs)	(plid_t plid, hudtextparms_t tparms, 
											const char *fmt, va_list ap);
	qboolean	(*pfnCallGameEntity)	(plid_t plid, const char *entStr,
											entvars_t *pev);
	int			(*pfnGetUserMsgID)		(plid_t plid, const char *msgname, int *size);
	const char *(*pfnGetUserMsgName)	(plid_t plid, int msgid, int *size);
	const char *(*pfnGetPluginPath)		(plid_t plid);
	const char *(*pfnGetGameInfo)		(plid_t plid, ginfo_t tag);
	
	int (*pfnLoadPlugin)(plid_t plid, const char *cmdline, PLUG_LOADTIME now, void **plugin_handle);
	int (*pfnUnloadPlugin)(plid_t plid, const char *cmdline, PLUG_LOADTIME now, PL_UNLOAD_REASON reason);
	int (*pfnUnloadPluginByHandle)(plid_t plid, void *plugin_handle, PLUG_LOADTIME now, PL_UNLOAD_REASON reason);
	
	const char *(*pfnIsQueryingClientCvar)	(plid_t plid, const edict_t *player);
	
	int (*pfnMakeRequestID)	(plid_t plid);
	
	void (*pfnGetHookTables) (plid_t plid, enginefuncs_t **peng, DLL_FUNCTIONS **pdll, NEW_DLL_FUNCTIONS **pnewdll);

	// 2022-07 Added by hzqst
	void *   (*pfnGetModuleBaseByHandle)(DLHANDLE hModule);
	DLHANDLE   (*pfnGetModuleHandle)(const char *szModuleName);
	void *   (*pfnGetModuleBaseByName)(const char *szModuleName);
	size_t   (*pfnGetImageSize)(void *ImageBase);
	qboolean (*pfnIsAddressInModuleRange)(void *lpAddress, void *lpModuleBase);
	DLHANDLE (*pfnGetGameDllHandle)(void);
	void *   (*pfnGetGameDllBase)(void);
	DLHANDLE (*pfnGetEngineHandle)(void);
	void *   (*pfnGetEngineBase)(void);
	void *   (*pfnGetEngineEnd)(void);
	void *   (*pfnGetEngineCodeBase)(void);
	void *   (*pfnGetEngineCodeEnd)(void);
	qboolean(*pfnIsValidCodePointerInEngine)(void *ptr);
	qboolean(*pfnUnHook)(hook_t *pHook);
	hook_t *(*pfnInlineHook)(void *pOldFuncAddr, void *pNewFuncAddr, void **pOrginalCall, bool bTranscation);
	void *(*pfnGetNextCallAddr)(void *pAddress, int dwCount);
	void *(*pfnSearchPattern)(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen);
	void *(*pfnReverseSearchPattern)(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen);
	int(*pfnDisasmSingleInstruction)(void *address, fnDisasmSingleCallback callback, void *context);
	qboolean(*pfnDisasmRanges)(void * DisasmBase, size_t DisasmSize, fnDisasmCallback callback, int depth, void *context);
	void *(*pfnReverseSearchFunctionBegin)(void * SearchBegin, size_t SearchSize);
	void *(*pfnReverseSearchFunctionBeginEx)(void * SearchBegin, size_t SearchSize, fnFindAddressCallback callback);
	void (*pfnCloseModuleHandle)(DLHANDLE hModule);
	DLHANDLE(*pfnLoadLibrary)(const char *szModuleName);
	void (*pfnFreeLibrary)(DLHANDLE hModule);
} mutil_funcs_t;
extern mutil_funcs_t MetaUtilFunctions DLLHIDDEN;

// Convenience macros for MetaUtil functions
#define LOG_CONSOLE			(*gpMetaUtilFuncs->pfnLogConsole)
#define LOG_MESSAGE			(*gpMetaUtilFuncs->pfnLogMessage)
#define LOG_ERROR			(*gpMetaUtilFuncs->pfnLogError)
#define LOG_DEVELOPER		(*gpMetaUtilFuncs->pfnLogDeveloper)
#define CENTER_SAY			(*gpMetaUtilFuncs->pfnCenterSay)
#define CENTER_SAY_PARMS	(*gpMetaUtilFuncs->pfnCenterSayParms)
#define CENTER_SAY_VARARGS	(*gpMetaUtilFuncs->pfnCenterSayVarargs)
#define CALL_GAME_ENTITY	(*gpMetaUtilFuncs->pfnCallGameEntity)
#define GET_USER_MSG_ID		(*gpMetaUtilFuncs->pfnGetUserMsgID)
#define GET_USER_MSG_NAME	(*gpMetaUtilFuncs->pfnGetUserMsgName)
#define GET_PLUGIN_PATH		(*gpMetaUtilFuncs->pfnGetPluginPath)
#define GET_GAME_INFO		(*gpMetaUtilFuncs->pfnGetGameInfo)
#define LOAD_PLUGIN		(*gpMetaUtilFuncs->pfnLoadPlugin)
#define UNLOAD_PLUGIN		(*gpMetaUtilFuncs->pfnUnloadPlugin)
#define UNLOAD_PLUGIN_BY_HANDLE	(*gpMetaUtilFuncs->pfnUnloadPluginByHandle)
#define IS_QUERYING_CLIENT_CVAR (*gpMetaUtilFuncs->pfnIsQueryingClientCvar)
#define MAKE_REQUESTID		(*gpMetaUtilFuncs->pfnMakeRequestID)
#define GET_HOOK_TABLES         (*gpMetaUtilFuncs->pfnGetHookTables)

#endif /* MUTIL_H */
