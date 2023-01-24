// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// mutil.cpp - utility functions to provide to plugins

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

#include <vector>
#include <set>

#include <stdio.h>			// vsnprintf(), etc
#include <stdarg.h>			// vs_start(), etc
#include <stdlib.h>			// strtol()

#include <extdll.h>			// always

#include "meta_api.h"		// 
#include "mutil.h"			// me
#include "mhook.h"			// class MHookList, etc
#include "linkent.h"		// ENTITY_FN, etc
#include "metamod.h"		// Hooks, etc
#include "types_meta.h"		// mBOOL
#include "osdep.h"			// win32 vsnprintf, etc
#include "sdk_util.h"		// ALERT, etc

 //2022-07 Added by hzqst
#include "detours.h"		// ALERT, etc
#include <capstone.h>

#ifdef _WIN32

#else
#include <procmap/MemoryMap.hpp>
#endif

static hudtextparms_t default_csay_tparms = {
	-1, 0.25,			// x, y
	2,					// effect
	0, 255, 0,	0,		// r, g, b,  a1
	0, 0, 0,	0,		// r2, g2, b2,  a2
	0, 0, 10, 10,		// fadein, fadeout, hold, fxtime
	1					// channel
};

// Log to console; newline added.
static void mutil_LogConsole(plid_t /* plid */, const char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	unsigned int len;

	va_start(ap, fmt);
	safevoid_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	// end msg with newline
	len=strlen(buf);
	if(len < sizeof(buf)-2)		// -1 null, -1 for newline
		strcat(buf, "\n");
	else
		buf[len-1] = '\n';

	SERVER_PRINT(buf);
}

// Log regular message to logs; newline added.
static void mutil_LogMessage(plid_t plid, const char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	plugin_info_t *plinfo;

	plinfo=(plugin_info_t *)plid;
	va_start(ap, fmt);
	safevoid_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[%s] %s\n", plinfo->logtag, buf);

	FILE *fp = fopen("metamod.log", "a+");
	fputs(buf, fp);
	fputs("\r\n", fp);
	fclose(fp);
}

// Log an error message to logs; newline added.
static void mutil_LogError(plid_t plid, const char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	plugin_info_t *plinfo;

	plinfo=(plugin_info_t *)plid;
	va_start(ap, fmt);
	safevoid_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[%s] ERROR: %s\n", plinfo->logtag, buf);

	FILE *fp = fopen("metamod.log", "a+");
	fputs(buf, fp);
	fputs("\r\n", fp);
	fclose(fp);
}

// Log a message only if cvar "developer" set; newline added.
static void mutil_LogDeveloper(plid_t plid, const char *fmt, ...) {
	va_list ap;
	char buf[MAX_LOGMSG_LEN];
	plugin_info_t *plinfo;

	if((int)CVAR_GET_FLOAT("developer") == 0)
		return;

	plinfo=(plugin_info_t *)plid;
	va_start(ap, fmt);
	safevoid_vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ALERT(at_logged, "[%s] dev: %s\n", plinfo->logtag, buf);

	FILE *fp = fopen("metamod.log", "a+");
	fputs(buf, fp);
	fputs("\r\n", fp);
	fclose(fp);
}

// Print a center-message, with text parameters and varargs.  Provides
// functionality to the above center_say interfaces.
static void mutil_CenterSayVarargs(plid_t plid, hudtextparms_t tparms, 
		const char *fmt, va_list ap) 
{
	char buf[MAX_LOGMSG_LEN];
	int n;
	edict_t *pEntity;

	safevoid_vsnprintf(buf, sizeof(buf), fmt, ap);

	mutil_LogMessage(plid, "(centersay) %s", buf);
	for(n=1; n <= gpGlobals->maxClients; n++) {
		pEntity=INDEXENT(n);
		META_UTIL_HudMessage(pEntity, tparms, buf);
	}
}

// Print message on center of all player's screens.  Uses default text
// parameters (color green, 10 second fade-in).
static void mutil_CenterSay(plid_t plid, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	mutil_CenterSayVarargs(plid, default_csay_tparms, fmt, ap);
	va_end(ap);
}

// Print a center-message, with given text parameters.
static void mutil_CenterSayParms(plid_t plid, hudtextparms_t tparms, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	mutil_CenterSayVarargs(plid, tparms, fmt, ap);
	va_end(ap);
}

// Allow plugins to call the entity functions in the GameDLL.  In
// particular, calling "player()" as needed by most Bots.  Suggested by
// Jussi Kivilinna.
static qboolean mutil_CallGameEntity(plid_t plid, const char *entStr, entvars_t *pev) {
	plugin_info_t *plinfo;
	ENTITY_FN pfnEntity;

	plinfo=(plugin_info_t *)plid;
	META_DEBUG(8, ("Looking up game entity '%s' for plugin '%s'", entStr,
				plinfo->name));
	pfnEntity = (ENTITY_FN) DLSYM(GameDLL.handle, entStr);
	if(!pfnEntity) {
		META_WARNING("Couldn't find game entity '%s' in game DLL '%s' for plugin '%s'", entStr, GameDLL.name, plinfo->name);
		return(false);
	}
	META_DEBUG(7, ("Calling game entity '%s' for plugin '%s'", entStr,
				plinfo->name));
	(*pfnEntity)(pev);
	return(true);
}

// Find a usermsg, registered by the gamedll, with the corresponding
// msgname, and return remaining info about it (msgid, size).
static int mutil_GetUserMsgID(plid_t plid, const char *msgname, int *size) {
	plugin_info_t *plinfo;
	MRegMsg *umsg;

	plinfo=(plugin_info_t *)plid;
	META_DEBUG(8, ("Looking up usermsg name '%s' for plugin '%s'", msgname,
				plinfo->name));
	umsg=RegMsgs->find(msgname);
	if(umsg) {
		if(size)
			*size=umsg->size;
		return(umsg->msgid);
	}
	else
		return(0);
}

// Find a usermsg, registered by the gamedll, with the corresponding
// msgid, and return remaining info about it (msgname, size).
static const char *mutil_GetUserMsgName(plid_t plid, int msgid, int *size) {
	plugin_info_t *plinfo;
	MRegMsg *umsg;

	plinfo=(plugin_info_t *)plid;
	META_DEBUG(8, ("Looking up usermsg id '%d' for plugin '%s'", msgid,
				plinfo->name));
	// Guess names for any built-in Engine messages mentioned in the SDK;
	// from dlls/util.h.
	if(msgid < 64) {
		switch(msgid) {
			case SVC_TEMPENTITY:
				if(size) *size=-1;
				return("tempentity?");
			case SVC_INTERMISSION:
				if(size) *size=-1;
				return("intermission?");
			case SVC_CDTRACK:
				if(size) *size=-1;
				return("cdtrack?");
			case SVC_WEAPONANIM:
				if(size) *size=-1;
				return("weaponanim?");
			case SVC_ROOMTYPE:
				if(size) *size=-1;
				return("roomtype?");
			case SVC_DIRECTOR:
				if(size) *size=-1;
				return("director?");
		}
	}
	umsg=RegMsgs->find(msgid);
	if(umsg) {
		if(size)
			*size=umsg->size;
		// 'name' is assumed to be a constant string, allocated in the
		// gamedll.
		return(umsg->name);
	}
	else
		return(NULL);
}

// Return the full path of the plugin's loaded dll/so file.
static const char *mutil_GetPluginPath(plid_t plid) {
	static char buf[PATH_MAX];
	MPlugin *plug;

	plug=Plugins->find(plid);
	if(!plug) {
		META_WARNING("GetPluginPath: couldn't find plugin '%s'",
				plid->name);
		return(NULL);
	}
	STRNCPY(buf, plug->pathname, sizeof(buf));
	return(buf);
}

// Return various string-based info about the game/MOD/gamedll.
static const char *mutil_GetGameInfo(plid_t plid, ginfo_t type) {
	static char buf[MAX_STRBUF_LEN];
	const char *cp;
	switch(type) {
		case GINFO_NAME:
			cp=GameDLL.name;
			break;
		case GINFO_DESC:
			cp=GameDLL.desc;
			break;
		case GINFO_GAMEDIR:
			cp=GameDLL.gamedir;
			break;
		case GINFO_DLL_FULLPATH:
			cp=GameDLL.pathname;
			break;
		case GINFO_DLL_FILENAME:
			cp=GameDLL.file;
			break;
		case GINFO_REALDLL_FULLPATH:
			cp=GameDLL.real_pathname;
			break;
		default:
			META_WARNING("GetGameInfo: invalid request '%d' from plugin '%s'",
					type, plid->name);
			return(NULL);
	}
	STRNCPY(buf, cp, sizeof(buf));
	return(buf);
}

static int mutil_LoadMetaPlugin(plid_t plid, const char *fname, PLUG_LOADTIME now, void **plugin_handle)
{
	MPlugin *pl_loaded;
	
	if(NULL == fname) {
		return(ME_ARGUMENT);
	}

	meta_errno = ME_NOERROR;
	pl_loaded = Plugins->plugin_addload(plid, fname, now);
	if(!pl_loaded) {
		if(plugin_handle)
			*plugin_handle = NULL;
		return(meta_errno);
	} else {
		if(plugin_handle)
			*plugin_handle = (void*)pl_loaded->handle;
		return(0);
	}
}

static int mutil_UnloadMetaPlugin(plid_t plid, const char *fname, PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	MPlugin *findp = NULL;
	int pindex;
	char* endptr;

	if(NULL == fname) {
		return(ME_ARGUMENT);
	}

	pindex = strtol(fname, &endptr, 10);
	if(*fname != '\0' && *endptr == '\0')
		findp = Plugins->find(pindex);
	else
		findp = Plugins->find_match(fname);

	if(!findp)
		return(meta_errno);

	meta_errno = ME_NOERROR;

	if(findp->plugin_unload(plid, now, reason))
		return(0);
	
	return(meta_errno);
}

static int mutil_UnloadMetaPluginByHandle(plid_t plid, void *plugin_handle, PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	MPlugin *findp;

	if(NULL == plugin_handle) {
		return(ME_ARGUMENT);
	}

	if(!(findp=Plugins->find((DLHANDLE)plugin_handle)))
		return(ME_NOTFOUND);
	
	meta_errno = ME_NOERROR;

	if(findp->plugin_unload(plid, now, reason))
		return(0);

	return(meta_errno);
}

// Check if player is being queried for cvar
static const char * mutil_IsQueryingClientCvar(plid_t /*plid*/, const edict_t *player) {
	return(g_Players.is_querying_cvar(player));
}

//
static int mutil_MakeRequestID(plid_t /*plid*/) {
	return(abs(0xbeef<<16) + (++requestid_counter));
}

//
static void mutil_GetHookTables(plid_t plid, enginefuncs_t **peng, DLL_FUNCTIONS **pdll, NEW_DLL_FUNCTIONS **pnewdll) {
	if (peng)
		*peng = &meta_engfuncs;
	if (pdll)
		*pdll = g_pHookedDllFunctions;
	if (pnewdll)
		*pnewdll = g_pHookedNewDllFunctions;
}

//2022-07 Added by hzqst

void * mutil_GetModuleBaseByHandle(DLHANDLE hModule)
{
#ifdef _WIN32
	return (void *)hModule;
#else
	struct link_map *map = NULL;
	if (0 == dlinfo(hModule, RTLD_DI_LINKMAP, &map))
	{
		//META_WARNING("mutil_GetModuleBaseByHandle: map->l_ld %p", map->l_ld);
		Dl_info info;
		if (0 != dladdr((void *)map->l_ld, &info))
		{
			//META_WARNING("mutil_GetModuleBaseByHandle: info.dli_fbase %p", info.dli_fbase);
			return (void *)info.dli_fbase;
		}
	}
	return NULL;
#endif
}

DLHANDLE mutil_LoadLibrary(const char *name)
{
#ifdef _WIN32
	return LoadLibraryA(name);
#else
	return dlopen(name, RTLD_NOW);
#endif
}

void mutil_FreeLibrary(DLHANDLE handle)
{
#ifdef _WIN32
	FreeLibrary(handle);
#else
	dlclose(handle);
#endif
}

void *mutil_GetProcAddress(DLHANDLE handle, const char *name)
{
#ifdef _WIN32
	return (void *)GetProcAddress(handle, name);
#else
	return (void *)dlsym(handle);
#endif
}

void mutil_CloseModuleHandle(DLHANDLE handle)
{
#ifdef _WIN32
	
#else
	dlclose(handle);
#endif
}

DLHANDLE mutil_GetModuleHandle(const char *name)
{
#ifdef _WIN32
	return GetModuleHandleA(name);
#else
	return dlopen(name, RTLD_NOW | RTLD_GLOBAL | RTLD_NOLOAD);
#endif
}

void *mutil_GetModuleBaseByName(const char *name)
{
	auto base = (void *)NULL;

	auto handle = mutil_GetModuleHandle(name);

	if (handle != NULL)
	{
		base = mutil_GetModuleBaseByHandle(handle);

		mutil_CloseModuleHandle(handle);
	}
	return base;
}

size_t mutil_GetImageSize(void *ImageBase)
{
#ifdef _WIN32
	return ((IMAGE_NT_HEADERS *)((char *)ImageBase + ((IMAGE_DOS_HEADER *)ImageBase)->e_lfanew))->OptionalHeader.SizeOfImage;
#else
	std::string name;
	void *startaddr = ImageBase;
	void *endaddr = nullptr;
	size_t imageSize = 0;
	procmap::MemoryMap m;
	for (auto &segment : m) {
		if (startaddr == segment.startAddress()) {
			name = segment.name();
		}
		if (!name.empty() && name == segment.name() && segment.endAddress() > endaddr)
		{
			endaddr = segment.endAddress();
			imageSize = (uintptr_t)endaddr - (uintptr_t)startaddr;
		}
	}
	return imageSize;//wtf?
#endif
}

qboolean mutil_IsAddressInModuleRange(void *lpAddress, void *lpModuleBase)
{
#ifdef _WIN32
	return (char *)lpAddress > (char *)lpModuleBase && (char *)lpAddress < (char *)lpModuleBase + mutil_GetImageSize(lpModuleBase);
#else
	Dl_info info;
	if (dladdr(lpAddress, &info) != 0 && info.dli_fbase == lpModuleBase)
		return true;

	return false;
#endif
}

DLHANDLE mutil_GetGameDllHandle(void)
{
	return GameDLL.handle;
}

void *mutil_GetGameDllBase(void)
{
	return (void *)GameDLL.imagebase;
}

DLHANDLE mutil_GetEngineHandle(void)
{
	return Engine.info.m_imageHandle;
}

void *mutil_GetEngineBase(void)
{
	return Engine.info.m_imageStart;
}

void *mutil_GetEngineEnd(void)
{
	return Engine.info.m_imageEnd;
}

void *mutil_GetEngineCodeBase(void)
{
	return Engine.info.m_codeStart;
}

void *mutil_GetEngineCodeEnd(void)
{
	return Engine.info.m_codeEnd;
}

qboolean mutil_IsValidCodePointerInEngine(void *ptr)
{
	return Engine.info.is_valid_code_pointer(ptr);
}

hook_t *g_pHookBase = NULL;

hook_t *mutil_NewHook(int iType)
{
	hook_t *h = new hook_t;
	memset(h, 0, sizeof(hook_t));
	h->iType = iType;
	h->pNext = g_pHookBase;
	g_pHookBase = h;
	return h;
}

void mutil_CommitHook(hook_t *pHook)
{
	if (pHook->iType == MH_HOOK_VFTABLE)
	{
		
	}
	else if (pHook->iType == MH_HOOK_IAT)
	{
		
	}
	else if (pHook->iType == MH_HOOK_INLINE)
	{
		auto d = (CDetour *)pHook->pInfo;

		d->EnableDetour();
	}
}

void mutil_FreeHook(hook_t *pHook)
{
	if (pHook->iType == MH_HOOK_VFTABLE)
	{
		//tagVTABLEDATA *info = (tagVTABLEDATA *)pHook->pInfo;
		//MH_WriteMemory(info->pVFTInfoAddr, (BYTE *)&pHook->pOldFuncAddr, sizeof(DWORD));
	}
	else if (pHook->iType == MH_HOOK_IAT)
	{
		//tagIATDATA *info = (tagIATDATA *)pHook->pInfo;
		//MH_WriteMemory(info->pAPIInfoAddr, (BYTE *)&pHook->pOldFuncAddr, sizeof(DWORD));
	}
	else if (pHook->iType == MH_HOOK_INLINE)
	{
		auto d = (CDetour *)pHook->pInfo;

		if (d->IsEnabled())
		{
			d->DisableDetour();
			d->Destroy();

			pHook->pInfo = NULL;
		}
		else
		{
			d->Destroy();
			pHook->pInfo = NULL;
		}
	}

	delete pHook;
}

void mutil_CommitHooks(void)
{
	hook_t *next = NULL;

	for (hook_t *h = g_pHookBase; h; h = next)
	{
		next = h->pNext;
		mutil_CommitHook(h);
	}
}

void mutil_FreeAllHook(void)
{
	hook_t *next = NULL;

	for (hook_t *h = g_pHookBase; h; h = next)
	{
		next = h->pNext;
		mutil_FreeHook(h);
	}

	g_pHookBase = NULL;
}

qboolean mutil_UnHook(hook_t *pHook)
{
	if (!g_pHookBase)
		return false;

	hook_t *h, **back;
	back = &g_pHookBase;

	while (1)
	{
		h = *back;

		if (!h)
			break;

		if (h == pHook)
		{
			*back = h->pNext;
			mutil_FreeHook(h);
			return true;
		}

		back = &h->pNext;
	}

	return false;
}

hook_t *mutil_InlineHook(void *pOldFuncAddr, void *pNewFuncAddr, void **pOrginalCall, bool bTranscation)
{
	hook_t *h = mutil_NewHook(MH_HOOK_INLINE);
	h->pOldFuncAddr = pOldFuncAddr;
	h->pNewFuncAddr = pNewFuncAddr;
	h->pOrginalCall = pOrginalCall;

	if (!pOrginalCall)
	{
		return NULL;
	}

	auto d = CDetourManager::CreateDetour(pNewFuncAddr, pOrginalCall, pOldFuncAddr);

	h->pInfo = d;

	if (bTranscation)
	{
		h->bCommitted = false;
	}
	else
	{
		d->EnableDetour();
		h->bCommitted = true;
	}

	return h;
}

void *mutil_GetNextCallAddr(void *pAddress, int dwCount)
{
	static char *pbAddress = NULL;

	if (pAddress)
		pbAddress = (char *)pAddress;
	else
		pbAddress = pbAddress + 5;

	for (int i = 0; i < dwCount; i++)
	{
		byte code = *(byte *)pbAddress;

		if (code == 0xFF && *(byte *)(pbAddress + 1) == 0x15)
		{
			return *(void **)(pbAddress + 2);
		}

		if (code == 0xE8)
		{
			return (void *)(pbAddress + 5 + *(int *)(pbAddress + 1));
		}

		pbAddress++;
	}

	return NULL;
}

void *mutil_SearchPattern(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen)
{
	char * dwStartAddr = (char *)pStartSearch;
	char * dwEndAddr = dwStartAddr + dwSearchLen - dwPatternLen;

	while (dwStartAddr < dwEndAddr)
	{
		bool found = true;

		for (size_t i = 0; i < dwPatternLen; i++)
		{
			char code = *(char *)(dwStartAddr + i);

			if (pPattern[i] != 0x2A && pPattern[i] != code)
			{
				found = false;
				break;
			}
		}

		if (found)
			return (void *)dwStartAddr;

		dwStartAddr++;
	}

	return NULL;
}

void *mutil_ReverseSearchPattern(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen)
{
	char * dwStartAddr = (char *)pStartSearch;
	char * dwEndAddr = dwStartAddr - dwSearchLen - dwPatternLen;

	while (dwStartAddr > dwEndAddr)
	{
		bool found = true;

		for (size_t i = 0; i < dwPatternLen; i++)
		{
			char code = *(char *)(dwStartAddr + i);

			if (pPattern[i] != 0x2A && pPattern[i] != code)
			{
				found = false;
				break;
			}
		}

		if (found)
			return (void *)dwStartAddr;

		dwStartAddr--;
	}

	return 0;
}

int mutil_DisasmSingleInstruction(void *address, fnDisasmSingleCallback callback, void *context)
{
	int instLen = 0;
	csh handle = 0;
	if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) == CS_ERR_OK)
	{
		if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) == CS_ERR_OK)
		{
			cs_insn *insts = NULL;
			size_t count = 0;

			const uint8_t *addr = (uint8_t *)address;
			uint64_t vaddr = ((uint64_t)address & 0x00000000FFFFFFFFull);
			size_t size = 15;

#ifdef _WIN32
			bool accessable = !IsBadReadPtr(addr, size);

			if (accessable)
#endif
			{
				count = cs_disasm(handle, addr, size, vaddr, 1, &insts);
				if (count)
				{
					callback(insts, (byte *)address, insts->size, context);

					instLen += insts->size;
				}
			}

			if (insts) {
				cs_free(insts, count);
				insts = NULL;
			}
		}
		cs_close(&handle);
	}

	return instLen;
}

qboolean mutil_DisasmRanges(void * DisasmBase, size_t DisasmSize, fnDisasmCallback callback, int depth, void *context)
{
	qboolean success = false;

	csh handle = 0;
	if (cs_open(CS_ARCH_X86, CS_MODE_32, &handle) == CS_ERR_OK)
	{
		cs_insn *insts = NULL;
		size_t count = 0;
		int instCount = 1;

		if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) == CS_ERR_OK)
		{
			byte *pAddress = (byte *)DisasmBase;

			do
			{
				const uint8_t *addr = (uint8_t *)pAddress;
				uint64_t vaddr = ((uint64_t)pAddress & 0x00000000FFFFFFFFull);
				size_t size = 15;

				if (insts) {
					cs_free(insts, count);
					insts = NULL;
				}
#ifdef _WIN32
				bool accessable = !IsBadReadPtr(addr, size);

				if (!accessable)
					break;
#endif

				count = cs_disasm(handle, addr, size, vaddr, 1, &insts);
				if (!count)
					break;

				size_t instLen = insts[0].size;
				if (!instLen)
					break;

				if (callback(&insts[0], pAddress, instLen, instCount, depth, context))
				{
					success = true;
					break;
				}

				pAddress += instLen;
				instCount++;
			} while (pAddress < (byte *)DisasmBase + DisasmSize);
		}

		if (insts) {
			cs_free(insts, count);
			insts = NULL;
		}

		cs_close(&handle);
	}

	return success;
}

class walk_context_t
{
public:
	walk_context_t()
	{
		address = 0;
		len = 0;
		depth = 0;
	}
	walk_context_t(void * a, size_t l, int d) : address(a), len(l), depth(d)
	{

	}
	void * address;
	size_t len;
	int depth;
};

typedef struct
{
	void * base;
	size_t max_insts;
	int max_depth;
	std::set<uintp> code;
	std::set<uintp> branches;
	std::vector<walk_context_t> walks;

	void *DesiredAddress;
	bool bFoundDesiredAddress;
}MH_ReverseSearchFunctionBegin_ctx;

typedef struct
{
	bool bPushRegister;
	bool bSubEspImm;
}MH_ReverseSearchFunctionBegin_ctx2;

void *mutil_ReverseSearchFunctionBegin(void * SearchBegin, size_t SearchSize)
{
	byte *SearchPtr = (byte *)SearchBegin;
	byte *SearchEnd = (byte *)SearchBegin - SearchSize;

	while (SearchPtr > SearchEnd)
	{
		void *Candidate = NULL;
		bool bShouldCheck = false;

		if (SearchPtr[0] == 0xCC || SearchPtr[0] == 0x90 || SearchPtr[0] == 0xC3)
		{
			if (SearchPtr[1] == 0xCC || SearchPtr[1] == 0x90)
			{
				if (SearchPtr[2] != 0x90 &&
					SearchPtr[2] != 0xCC)
				{
					bShouldCheck = true;
					Candidate = SearchPtr + 2;
				}
			}
			else if (
				SearchPtr[1] != 0x90 &&
				SearchPtr[1] != 0xCC)
			{
				MH_ReverseSearchFunctionBegin_ctx2 ctx2 = { 0 };

				mutil_DisasmSingleInstruction(SearchPtr + 1, [](void *inst, byte *address, size_t instLen, void * context)
				{
					auto pinst = (cs_insn *)inst;
					auto ctx = (MH_ReverseSearchFunctionBegin_ctx2 *)context;

					if (pinst->id == X86_INS_PUSH &&
						pinst->detail->x86.op_count == 1 &&
						pinst->detail->x86.operands[0].type == X86_OP_REG)
					{
						ctx->bPushRegister = true;
					}
					else if (pinst->id == X86_INS_SUB &&
						pinst->detail->x86.op_count == 2 &&
						pinst->detail->x86.operands[0].type == X86_OP_REG &&
						pinst->detail->x86.operands[0].reg == X86_REG_ESP &&
						pinst->detail->x86.operands[1].type == X86_OP_IMM)
					{
						ctx->bSubEspImm = true;
					}

				}, &ctx2);

				if (ctx2.bPushRegister || ctx2.bSubEspImm)
				{
					bShouldCheck = true;
					Candidate = SearchPtr + 1;
				}
			}
		}

		if (bShouldCheck)
		{
			MH_ReverseSearchFunctionBegin_ctx ctx = { 0 };

			ctx.bFoundDesiredAddress = false;
			ctx.DesiredAddress = SearchBegin;
			ctx.base = Candidate;
			ctx.max_insts = 1000;
			ctx.max_depth = 16;
			ctx.walks.emplace_back(ctx.base, 0x1000, 0);

			while (ctx.walks.size())
			{
				auto walk = ctx.walks[ctx.walks.size() - 1];
				ctx.walks.pop_back();

				mutil_DisasmRanges(walk.address, walk.len, [](void *inst, byte *address, size_t instLen, int instCount, int depth, void *context) -> qboolean
					{
						auto pinst = (cs_insn *)inst;
						auto ctx = (MH_ReverseSearchFunctionBegin_ctx *)context;

						if (address == ctx->DesiredAddress)
						{
							ctx->bFoundDesiredAddress = true;
							return true;
						}

						if (ctx->code.size() > ctx->max_insts)
							return true;

						if (ctx->code.find((uintp)address) != ctx->code.end())
							return true;

						ctx->code.emplace((uintp)address);

						if ((pinst->id == X86_INS_JMP || (pinst->id >= X86_INS_JAE && pinst->id <= X86_INS_JS)) &&
							pinst->detail->x86.op_count == 1 &&
							pinst->detail->x86.operands[0].type == X86_OP_IMM)
						{
							void *imm = (void *)pinst->detail->x86.operands[0].imm;
							auto foundbranch = ctx->branches.find((uintp)imm);
							if (foundbranch == ctx->branches.end())
							{
								ctx->branches.emplace((uintp)imm);
								if (depth + 1 < ctx->max_depth)
									ctx->walks.emplace_back(imm, 0x300, depth + 1);
							}

							if (pinst->id == X86_INS_JMP)
								return true;
						}

						if (address[0] == 0xCC)
							return true;

						if (pinst->id == X86_INS_RET)
							return true;

						return false;
					}, walk.depth, &ctx);
			}

			if (ctx.bFoundDesiredAddress)
			{
				return Candidate;
			}
		}

		SearchPtr--;
	}

	return NULL;
}

void *mutil_ReverseSearchFunctionBeginEx(void * SearchBegin, size_t SearchSize, fnFindAddressCallback callback)
{
	byte * SearchPtr = (byte *)SearchBegin;
	byte *SearchEnd = (byte *)SearchBegin - SearchSize;

	while (SearchPtr > SearchEnd)
	{
		void *Candidate = NULL;
		bool bShouldCheck = false;

		if (SearchPtr[0] == 0xCC || SearchPtr[0] == 0x90 || SearchPtr[0] == 0xC3)
		{
			if (SearchPtr[1] == 0xCC || SearchPtr[1] == 0x90)
			{
				if (SearchPtr[2] != 0x90 &&
					SearchPtr[2] != 0xCC)
				{
					bShouldCheck = true;
					Candidate = SearchPtr + 2;
				}
			}
			else if (
				SearchPtr[1] != 0x90 &&
				SearchPtr[1] != 0xCC)
			{
				MH_ReverseSearchFunctionBegin_ctx2 ctx2 = { 0 };

				mutil_DisasmSingleInstruction(SearchPtr + 1, [](void* inst, byte *address, size_t instLen, void *context)
				{
					auto pinst = (cs_insn*)inst;
					auto ctx = (MH_ReverseSearchFunctionBegin_ctx2*)context;

					if (pinst->id == X86_INS_PUSH &&
						pinst->detail->x86.op_count == 1 &&
						pinst->detail->x86.operands[0].type == X86_OP_REG)
					{
						ctx->bPushRegister = true;
					}
					else if (pinst->id == X86_INS_SUB &&
						pinst->detail->x86.op_count == 2 &&
						pinst->detail->x86.operands[0].type == X86_OP_REG &&
						pinst->detail->x86.operands[0].reg == X86_REG_ESP &&
						pinst->detail->x86.operands[1].type == X86_OP_IMM)
					{
						ctx->bSubEspImm = true;
					}

					}, &ctx2);

				if (ctx2.bPushRegister || ctx2.bSubEspImm)
				{
					bShouldCheck = true;
					Candidate = SearchPtr + 1;
				}
			}
		}

		if (bShouldCheck && callback((byte *)Candidate))
		{
			MH_ReverseSearchFunctionBegin_ctx ctx = { 0 };

			ctx.bFoundDesiredAddress = false;
			ctx.DesiredAddress = SearchBegin;
			ctx.base = Candidate;
			ctx.max_insts = 1000;
			ctx.max_depth = 16;
			ctx.walks.emplace_back(ctx.base, 0x1000, 0);

			while (ctx.walks.size())
			{
				auto walk = ctx.walks[ctx.walks.size() - 1];
				ctx.walks.pop_back();

				mutil_DisasmRanges(walk.address, walk.len, [](void* inst, unsigned char *address, size_t instLen, int instCount, int depth, void *context) -> qboolean
					{
						auto pinst = (cs_insn*)inst;
						auto ctx = (MH_ReverseSearchFunctionBegin_ctx*)context;

						if (address == ctx->DesiredAddress)
						{
							ctx->bFoundDesiredAddress = true;
							return true;
						}

						if (ctx->code.size() > ctx->max_insts)
							return true;

						if (ctx->code.find((uintp)address) != ctx->code.end())
							return true;

						ctx->code.emplace((uintp)address);

						if ((pinst->id == X86_INS_JMP || (pinst->id >= X86_INS_JAE && pinst->id <= X86_INS_JS)) &&
							pinst->detail->x86.op_count == 1 &&
							pinst->detail->x86.operands[0].type == X86_OP_IMM)
						{
							void * imm = (void *)pinst->detail->x86.operands[0].imm;
							auto foundbranch = ctx->branches.find((uintp)imm);
							if (foundbranch == ctx->branches.end())
							{
								ctx->branches.emplace((uintp)imm);
								if (depth + 1 < ctx->max_depth)
									ctx->walks.emplace_back(imm, 0x300, depth + 1);
							}

							if (pinst->id == X86_INS_JMP)
								return true;
						}

						if (address[0] == 0xCC)
							return true;

						if (pinst->id == X86_INS_RET)
							return true;

						return false;
					}, walk.depth, &ctx);
			}

			if (ctx.bFoundDesiredAddress)
			{
				return Candidate;
			}
		}

		SearchPtr--;
	}

	return NULL;
}


// Meta Utility Function table.
mutil_funcs_t MetaUtilFunctions = {
	mutil_LogConsole,		// pfnLogConsole
	mutil_LogMessage,		// pfnLogMessage
	mutil_LogError,			// pfnLogError
	mutil_LogDeveloper,		// pfnLogDeveloper
	mutil_CenterSay,		// pfnCenterSay
	mutil_CenterSayParms,	// pfnCenterSayParms
	mutil_CenterSayVarargs,	// pfnCenterSayVarargs
	mutil_CallGameEntity,	// pfnCallGameEntity
	mutil_GetUserMsgID,		// pfnGetUserMsgID
	mutil_GetUserMsgName,	// pfnGetUserMsgName
	mutil_GetPluginPath,	// pfnGetPluginPath
	mutil_GetGameInfo,		// pfnGetGameInfo
	mutil_LoadMetaPlugin, // pfnLoadPlugin
	mutil_UnloadMetaPlugin, // pfnUnloadPlugin
	mutil_UnloadMetaPluginByHandle, // pfnUnloadPluginByHandle
	mutil_IsQueryingClientCvar, // pfnIsQueryingClientCvar
	mutil_MakeRequestID, 	// pfnMakeRequestID
	mutil_GetHookTables,   // pfnGetHookTables

	//2022-07 Added by hzqst
	mutil_GetModuleBaseByHandle,
	mutil_GetModuleHandle,
	mutil_GetModuleBaseByName,
	mutil_GetImageSize,
	mutil_IsAddressInModuleRange,
	mutil_GetGameDllHandle,
	mutil_GetGameDllBase,
	mutil_GetEngineHandle,
	mutil_GetEngineBase,
	mutil_GetEngineEnd,
	mutil_GetEngineCodeBase,
	mutil_GetEngineCodeEnd,
	mutil_IsValidCodePointerInEngine,
	mutil_UnHook,
	mutil_InlineHook,
	mutil_GetNextCallAddr,
	mutil_SearchPattern,
	mutil_ReverseSearchPattern,
	mutil_DisasmSingleInstruction,
	mutil_DisasmRanges,
	mutil_ReverseSearchFunctionBegin,
	mutil_ReverseSearchFunctionBeginEx,
	mutil_CloseModuleHandle,
	mutil_LoadLibrary,
	mutil_FreeLibrary,
	mutil_GetProcAddress
};
