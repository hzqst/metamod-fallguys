// vi: set ts=4 sw=4 :
// vim: set tw=75 :

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

#include <dllapi.h>
#include <meta_api.h>

#include <detours.h>

bool IsEntitySuperPusher(edict_t *ent)
{
	return ent && (ent->v.sequence == 114514 || ent->v.sequence == 114515);
}

bool IsEntitySuperPusherFlexible(edict_t *ent)
{
	return ent && ent->v.sequence == 114515;
}

bool IsEntityPushee(edict_t *ent)
{
	return (ent->v.solid == SOLID_SLIDEBOX || ent->v.solid == SOLID_BBOX) && (ent->v.movetype == MOVETYPE_STEP || ent->v.movetype == MOVETYPE_WALK);
}

static trace_t *(*g_pfnSV_PushEntity)(trace_t * trace, edict_t *ent, float * push) = NULL;
static void (*g_pfnSV_PushMove)(edict_t *pusher, float movetime) = NULL;
static void(*g_pfnSV_PushRotate)(edict_t *pusher, float movetime) = NULL;

static bool g_bIsPushMove = false;
static bool g_bIsPushRotate = false;
static edict_t *g_PusherEntity = NULL;

void NewSV_PushMove(edict_t *pusher, float movetime)
{
	g_bIsPushMove = true;
	g_PusherEntity = pusher;

	g_pfnSV_PushMove(pusher, movetime);

	g_bIsPushMove = false;
}

void NewSV_PushRotate(edict_t *pusher, float movetime)
{
	g_bIsPushRotate = true;
	g_PusherEntity = pusher;

	g_pfnSV_PushRotate(pusher, movetime);

	g_bIsPushRotate = false;
}

static bool g_bIsPushEntity = false;
static edict_t *g_PushEntity;
static edict_t *g_PendingEntities[512] = { 0 };
static int g_NumPendingEntities = 0;

trace_t *NewSV_PushEntity(trace_t * trace, edict_t *ent, float *push)
{
	g_bIsPushEntity = true;

	g_PushEntity = ent;

	auto r = g_pfnSV_PushEntity(trace, ent, push);

	//push pending entities...
	for (int i = 0; i < g_NumPendingEntities; ++i)
	{
		auto pPendingEntity = g_PendingEntities[i];

		g_PushEntity = pPendingEntity;

		trace_t temp;
		g_pfnSV_PushEntity(&temp, pPendingEntity, push);
	}

	g_NumPendingEntities = 0;

	g_bIsPushEntity = false;

	if ((g_bIsPushMove || g_bIsPushRotate) && IsEntitySuperPusher(g_PusherEntity))
	{
		if (IsEntityPushee(ent) && ent->v.groundentity != g_PusherEntity)
		{
			vec3_t dir;
			dir.x = push[0];
			dir.y = push[1];
			dir.z = push[2];
			dir = dir.Normalize();

			if (g_PusherEntity->v.armorvalue > 0)
			{
				if (IsEntitySuperPusherFlexible(g_PusherEntity))
				{
					ent->v.velocity = dir * g_PusherEntity->v.armorvalue * g_PusherEntity->v.speed;
					if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
					{
						vec3_t dir2 = ent->v.origin - g_PusherEntity->v.origin;
						dir2.z = 0;
						dir2 = dir2.Normalize();
						ent->v.velocity = ent->v.velocity + dir2 * g_PusherEntity->v.armortype * g_PusherEntity->v.speed;
					}
				}
				else
				{
					ent->v.velocity = dir * g_PusherEntity->v.armorvalue;
					if (ent->v.velocity.z < 150)
						ent->v.velocity.z = 150;

					if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
					{
						vec3_t dir2 = ent->v.origin - g_PusherEntity->v.origin;
						dir2.z = 0;
						dir2 = dir2.Normalize();
						ent->v.velocity = ent->v.velocity + dir2 * g_PusherEntity->v.armortype;
					}
				}
			}
		}
	}

	return r;
}

void NewTouch(edict_t *pentTouched, edict_t *pentOther) {

	if (g_bIsPushEntity && pentTouched == g_PushEntity && g_NumPendingEntities < 512)
	{
		if (IsEntityPushee(pentTouched) && IsEntityPushee(pentOther))
		{
			//Don't append same entity twice
			for (int i = 0; i < g_NumPendingEntities; ++i)
			{
				if (g_PendingEntities[i] == pentOther)
				{
					SET_META_RESULT(MRES_IGNORED);
					return;
				}
			}

			g_PendingEntities[g_NumPendingEntities] = pentOther;
			g_NumPendingEntities++;

			SET_META_RESULT(MRES_HANDLED);
			return;
		}
	}

	SET_META_RESULT(MRES_IGNORED);
}

void NewThink(edict_t *pent)
{
	
}

static DLL_FUNCTIONS gFunctionTable = 
{
	NULL,					// pfnGameInit
	NULL,					// pfnSpawn
	NewThink,					// pfnThink
	NULL,					// pfnUse
	NewTouch,				// pfnTouch
	NULL,					// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	NULL,					// pfnClientConnect
	NULL,					// pfnClientDisconnect
	NULL,					// pfnClientKill
	NULL,					// pfnClientPutInServer
	NULL,					// pfnClientCommand
	NULL,					// pfnClientUserInfoChanged
	NULL,					// pfnServerActivate
	NULL,					// pfnServerDeactivate

	NULL,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	NULL,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink
	
	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType
	
	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	NULL,					// pfnCmdStart
	NULL,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	NULL,					// pfnAllowLagCompensation
};

void *MH_SearchPattern(void *pStartSearch, DWORD dwSearchLen, const char *pPattern, DWORD dwPatternLen)
{
	PUCHAR dwStartAddr = (PUCHAR)pStartSearch;
	PUCHAR dwEndAddr = dwStartAddr + dwSearchLen - dwPatternLen;

	while (dwStartAddr < dwEndAddr)
	{
		bool found = true;

		for (DWORD i = 0; i < dwPatternLen; i++)
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

DWORD MH_GetModuleSize(HMODULE hModule)
{
	return ((IMAGE_NT_HEADERS *)((DWORD)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
}

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, 
		int *interfaceVersion)
{
	if(!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return(FALSE);
	}
	else if(*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	auto engine = GetModuleHandleA("hw.dll");

	if (!engine)
		engine = GetModuleHandleA("swds.dll");

	if (engine)
	{
#define SV_PUSHENTITY_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x8B\x84\x24\x2A\x00\x00\x00"
		g_pfnSV_PushEntity = (decltype(g_pfnSV_PushEntity))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHENTITY_SVENGINE, sizeof(SV_PUSHENTITY_SVENGINE) - 1);

		if (g_pfnSV_PushEntity)
		{
			DetourTransactionBegin();
			DetourAttach(&(void *&)g_pfnSV_PushEntity, NewSV_PushEntity);
			DetourTransactionCommit();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushEntity");
		}

#define SV_PUSHMOVE_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x2A\x8B\xBC\x24\x88\x00\x00\x00\xD9"
		g_pfnSV_PushMove = (decltype(g_pfnSV_PushMove))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHMOVE_SVENGINE, sizeof(SV_PUSHMOVE_SVENGINE) - 1);

		if (g_pfnSV_PushMove)
		{
			DetourTransactionBegin();
			DetourAttach(&(void *&)g_pfnSV_PushMove, NewSV_PushMove);
			DetourTransactionCommit();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushMove");
		}

#define SV_PUSHROTATE_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x84\x24\x2A\x00\x00\x00\x2A\x8B\xBC\x24\xC0\x00\x00\x00\xD9"
		g_pfnSV_PushRotate = (decltype(g_pfnSV_PushMove))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHROTATE_SVENGINE, sizeof(SV_PUSHROTATE_SVENGINE) - 1);

		if (g_pfnSV_PushRotate)
		{
			DetourTransactionBegin();
			DetourAttach(&(void *&)g_pfnSV_PushRotate, NewSV_PushRotate);
			DetourTransactionCommit();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushMove");
		}
	}
	else
	{
		UTIL_LogPrintf("Failed to locate engine dll");
	}

	return(TRUE);
}
