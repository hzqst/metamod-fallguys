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

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

void NewTouch(edict_t *pentTouched, edict_t *pentOther)
{
	if (g_bIsPushEntity && pentTouched == g_PushEntity && g_NumPendingEntities < (int)_ARRAYSIZE(g_PendingEntities))
	{
		//Player pushes another player
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

	if (g_bIsRunPlayerMove)
	{
		int entindex = g_engfuncs.pfnIndexOfEdict(pentTouched);
		if (entindex == g_iRunPlayerMoveIndex)
		{
			if (pentOther->v.solid == SOLID_TRIGGER)
			{
				if(ASEXT_CallHook)
					(*ASEXT_CallHook)(g_PlayerTouchTriggerHook, 0, pentTouched->pvPrivateData, pentOther->pvPrivateData);
			}
			else if (pentOther->v.solid != SOLID_NOT)
			{
				if (ASEXT_CallHook)
					(*ASEXT_CallHook)(g_PlayerTouchImpactHook, 0, pentTouched->pvPrivateData, pentOther->pvPrivateData);

				if (g_engfuncs.pfnIndexOfEdict(pentOther) >= 1 && g_engfuncs.pfnIndexOfEdict(pentOther) <= gpGlobals->maxClients)
				{
					if (ASEXT_CallHook)
						(*ASEXT_CallHook)(g_PlayerTouchPlayerHook, 0, pentTouched->pvPrivateData, pentOther->pvPrivateData);
				}
			}
		}
	}
	SET_META_RESULT(MRES_IGNORED);
}

//This is only for season 1

void NewBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	if (g_bIsFallGuysSeason1)
	{
		if (Legacy_IsEntitySuperRotator(pentBlocked) && IsEntityPushee(pentOther))
		{
			if (pentBlocked->v.avelocity[0] > 1 || pentBlocked->v.avelocity[0] < -1)
			{
				vec3_t vecPlayer = pentOther->v.origin;
				vec3_t vecPusher = pentBlocked->v.origin;
				vecPusher.y = vecPlayer.y;
				vec3_t dir2 = vecPlayer - vecPusher;
				dir2 = dir2.Normalize();

				pentOther->v.velocity = pentOther->v.velocity + dir2 * pentBlocked->v.armorvalue;
			}
			else if (pentBlocked->v.avelocity[1] > 1 || pentBlocked->v.avelocity[1] < -1)
			{
				vec3_t vecPlayer = pentOther->v.origin;
				vec3_t vecPusher = pentBlocked->v.origin;
				vecPusher.x = vecPlayer.x;
				vec3_t dir2 = vecPlayer - vecPusher;
				dir2 = dir2.Normalize();

				pentOther->v.velocity = pentOther->v.velocity + dir2 * pentBlocked->v.armorvalue;
			}
			SET_META_RESULT(MRES_SUPERCEDE);
			return;
		}
		else if (Legacy_IsEntitySuperPusher(g_PusherEntity))
		{
			if (pentBlocked->v.avelocity[2] > 1 || pentBlocked->v.avelocity[2] < -1)
			{
				vec3_t vecPlayer = pentOther->v.origin;
				vec3_t vecPusher = pentBlocked->v.origin;
				vecPusher.z = vecPlayer.z;
				vec3_t dir2 = vecPlayer - vecPusher;
				dir2 = dir2.Normalize();

				pentOther->v.velocity = pentOther->v.velocity + dir2 * pentBlocked->v.armorvalue;
				pentOther->v.velocity.z += pentBlocked->v.max_health;
			}
			SET_META_RESULT(MRES_SUPERCEDE);
			return;
		}
	}
	SET_META_RESULT(MRES_IGNORED);
}

void NewSetAbsBox(edict_t *pent)
{
	if (gPhysicsManager.SetAbsBox(pent))
	{
		SET_META_RESULT(MRES_SUPERCEDE);
		return;
	}

	SET_META_RESULT(MRES_IGNORED);
}

void NewSetupVisibility(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas)
{
	g_ClientViewEntity[g_engfuncs.pfnIndexOfEdict(pClient)] = pViewEntity;

	SET_META_RESULT(MRES_IGNORED);
}

edict_t* GetClientViewEntity(int clientindex)
{
	if(clientindex > 0 && clientindex < gpGlobals->maxClients)
		return g_ClientViewEntity[clientindex];

	return NULL;
}

edict_t* GetClientViewEntity(edict_t*pClient)
{
	return GetClientViewEntity(g_engfuncs.pfnIndexOfEdict(pClient));
}

bool SetClientViewEntity(edict_t* pClient, edict_t*pViewEnt)
{
	int clientindex = g_engfuncs.pfnIndexOfEdict(pClient);
	if (clientindex > 0 && clientindex < gpGlobals->maxClients)
	{
		g_engfuncs.pfnSetView(pClient, pViewEnt);
		return true;
	}

	return false;
}

int NewAddToFullPack_Post(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	if (META_RESULT_ORIG_RET(int) == 1)
	{
		if (!gPhysicsManager.AddToFullPack(state, entindex, ent, host, hostflags, player))
		{
			SET_META_RESULT(MRES_OVERRIDE);
			return 0;
		}

		if (ASEXT_CallHook)
		{
			int uiFlags = 0;

			(*ASEXT_CallHook)(g_AddToFullPackHook, 0, state, entindex, ent, host, hostflags, player, &uiFlags);

			if (uiFlags & 1)
			{
				SET_META_RESULT(MRES_OVERRIDE);
				return 0;
			}
		}
	}

	SET_META_RESULT(MRES_IGNORED);
	return 1;
}

void NewGameInit_Post(void)
{
	sv_gravity = CVAR_GET_POINTER("sv_gravity");
	mp_footsteps = CVAR_GET_POINTER("mp_footsteps");

	gPhysicsManager.Init();

	SET_META_RESULT(MRES_IGNORED);
}

void NewStartFrame(void)
{
	gPhysicsManager.EntityStartFrame();

	gPhysicsManager.SetGravityAcceleration(sv_gravity->value);
	gPhysicsManager.StepSimulation((*host_frametime));
	
	gPhysicsManager.EntityStartFrame_Post();

	SET_META_RESULT(MRES_IGNORED);
}

int NewSpawn(edict_t *pent)
{
	//Entity 0 = world
	if (0 == g_engfuncs.pfnIndexOfEdict(pent))
	{
		gPhysicsManager.PreSpawn(pent);
	}

	SET_META_RESULT(MRES_IGNORED);
	return 1;
}

int NewSpawn_Post(edict_t *pent)
{
	//Entity 0 = world
	if (0 == g_engfuncs.pfnIndexOfEdict(pent))
	{
		gPhysicsManager.PostSpawn(pent);
	}

	SET_META_RESULT(MRES_IGNORED);
	return 1;
}

void NewServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	//Backward-compatibility for season 1
	auto mapname = STRING(gpGlobals->mapname);
	if (!strcasecmp(mapname, "fallguys"))
	{
		g_bIsFallGuysSeason1 = true;
	}
	else if (!strncasecmp(mapname, "fallguys_s", sizeof("fallguys_s") - 1))
	{
		gPhysicsManager.EnablePhysicWorld(true);
	}

	SET_META_RESULT(MRES_IGNORED);
}

void NewServerDeactivate()
{
	g_bIsFallGuysSeason1 = false;

	EnableCustomStepSound(false);

	gPhysicsManager.EnablePhysicWorld(false);

	gPhysicsManager.RemoveAllGameBodies();

	SET_META_RESULT(MRES_IGNORED);
}

void NewPlayerPreThink(edict_t *pEntity)
{
	g_bIsRunPlayerMove = true;
	g_iRunPlayerMoveIndex = g_engfuncs.pfnIndexOfEdict(pEntity);

	SET_META_RESULT(MRES_IGNORED);
}

void NewPlayerPostThink(edict_t* pEntity)
{
	g_bIsRunPlayerMove = false;
	g_iRunPlayerMoveIndex = 0;

	SET_META_RESULT(MRES_IGNORED);
}

void NewPlayerPostThink_Post(edict_t *pEntity)
{
	if(ASEXT_CallHook)
		(*ASEXT_CallHook)(g_PlayerPostThinkPostHook, 0, pEntity->pvPrivateData);

	SET_META_RESULT(MRES_IGNORED);
}

void NewPM_Move(struct playermove_s *ppmove, qboolean server)
{
	pmove = ppmove;

	gPhysicsManager.PM_StartMove();

	SET_META_RESULT(MRES_IGNORED);
}

void NewPM_Move_Post(struct playermove_s *ppmove, qboolean server)
{
	gPhysicsManager.PM_EndMove();

	SET_META_RESULT(MRES_IGNORED);
}

static DLL_FUNCTIONS gFunctionTable = 
{
	NULL,					// pfnGameInit
	NewSpawn,				// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NewTouch,				// pfnTouch
	NewBlocked,				// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NewSetAbsBox,			// pfnSetAbsBox

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
	NewServerActivate,		// pfnServerActivate
	NewServerDeactivate,	// pfnServerDeactivate

	NewPlayerPreThink,		// pfnPlayerPreThink
	NewPlayerPostThink,		// pfnPlayerPostThink

	NewStartFrame,			// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink
	
	NULL,					// pfnSys_Error

	NewPM_Move,				// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType
	
	NewSetupVisibility,		// pfnSetupVisibility
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

static DLL_FUNCTIONS gFunctionTable_Post =
{
	NewGameInit_Post,		// pfnGameInit
	NewSpawn_Post,			// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,					// pfnTouch
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
	NewPlayerPostThink_Post,					// pfnPlayerPostThink

	NULL,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink

	NULL,					// pfnSys_Error

	NewPM_Move_Post,		// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType

	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NewAddToFullPack_Post,				// pfnAddToFullPack
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

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	if (!pFunctionTable)
	{
		LOG_ERROR(PLID, "GetEntityAPI2_Post called with null pFunctionTable");
		return FALSE;
	}
	else if (*interfaceVersion != INTERFACE_VERSION)
	{
		LOG_ERROR(PLID, "GetEntityAPI2_Post version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return FALSE;
	}
	memcpy(pFunctionTable, &gFunctionTable_Post, sizeof(DLL_FUNCTIONS));
	return TRUE;
}

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	if(!pFunctionTable)
	{
		LOG_ERROR(PLID, "GetEntityAPI2 called with null pFunctionTable");
		return FALSE;
	}
	else if(*interfaceVersion != INTERFACE_VERSION)
	{
		LOG_ERROR(PLID, "GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return FALSE;
	}

	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));
	return TRUE;
}

void NewOnFreeEntPrivateData(edict_t* pEnt)
{
	gPhysicsManager.FreeEntityPrivateData(pEnt);

	SET_META_RESULT(MRES_HANDLED);
}

void NewGameShutdown(void)
{
	gPhysicsManager.Shutdown();

	SET_META_RESULT(MRES_IGNORED);
}

int NewShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	bool result = gPhysicsManager.ShouldCollide(pentTouched, pentOther);
	if (false == result)
	{
		SET_META_RESULT(MRES_SUPERCEDE);
		return 0;
	}

	SET_META_RESULT(MRES_IGNORED);
	return 1;
}

static NEW_DLL_FUNCTIONS gNewDllFunctionTable =
{
	// Called right before the object's memory is freed. 
	// Calls its destructor.
	NewOnFreeEntPrivateData,
	NULL,
	NewShouldCollide,

	// Added 2005/08/11 (no SDK update):
	NULL,//void(*pfnCvarValue)(const edict_t *pEnt, const char *value);

	// Added 2005/11/21 (no SDK update):
	//    value is "Bad CVAR request" on failure (i.e that user is not connected or the cvar does not exist).
	//    value is "Bad Player" if invalid player edict.
	NULL,//void(*pfnCvarValue2)(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
};

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS* pNewDllFunctionTable, int* interfaceVersion)
{
	if (!pNewDllFunctionTable) {
		LOG_ERROR(PLID, "GetNewDLLFunctions called with null pFunctionTable");
		return(FALSE);
	}
	else if (*interfaceVersion != NEW_DLL_FUNCTIONS_VERSION) {
		LOG_ERROR(PLID, "GetNewDLLFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return(FALSE);
	}
	memcpy(pNewDllFunctionTable, &gNewDllFunctionTable, sizeof(NEW_DLL_FUNCTIONS));

	return(TRUE);
}