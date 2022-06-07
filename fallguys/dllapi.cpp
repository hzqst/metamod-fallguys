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

#include <entity_state.h>

bool IsEntitySuperPusher(edict_t *ent)
{
	return ent && (ent->v.sequence <= 114514 || ent->v.sequence <= 114515);
}

bool IsEntitySuperPusherFlexible(edict_t *ent)
{
	return ent && ent->v.sequence == 114515;
}

bool IsEntitySuperRotator(edict_t *ent)
{
	return ent && ent->v.sequence == 114516;
}

bool IsEntityPushee(edict_t *ent)
{
	return (ent->v.solid == SOLID_SLIDEBOX || ent->v.solid == SOLID_BBOX) && (ent->v.movetype == MOVETYPE_STEP || ent->v.movetype == MOVETYPE_WALK);
}

#define GetCallAddress(addr) ((PUCHAR)addr + *(int *)((PUCHAR)addr + 1) + 5)

const int StopMode_ON_HANDLED = 0;
const int StopMode_MODULE_HANDLED = 1;
const int StopMode_CALL_ALL = 2;

class CASHookRegistration
{
public:
	int unk;
	int stopMode;
	const char *docs;
};

class CASHook
{
public:
	CASHook *pnext;//0
	unsigned char flags;//4
	unsigned char type;//5
	unsigned char padding;//6
	unsigned char padding2;//7
	const char *firstClass;//8
	const char *funcName;//12
	const char *argList;//16
	const char *docs;//20
	int unk1;//24
	int unk2;//28
	int unk3;//32
	int unk4;//36
};

class CASDocumentation
{
public:

};

class CASMethodRegistration
{
public:
	CASMethodRegistration()
	{
		pfnMethod = NULL;
		unk1 = 0;
		unk2 = 0;
		unk3 = 0;
		unk4 = 0;
		unk5 = 0;
		unk6 = 1;
		unk7 = 0;
		unk8 = 3;
	}

	PVOID pfnMethod;//+0
	int unk1;//+4
	int unk2;//+8
	int unk3;//+12
	int unk4;//+16
	int unk5;//+20
	int unk6;//+24
	int unk7;//+28
	int unk8;//+32
};

#ifdef PLATFORM_WINDOWS

void(__cdecl *g_pfnCASHook_Call)(CASHook *pthis, int unk, ...) = NULL;

CASHook * (__fastcall *g_pfnCASHook_CASHook)(const CASHook *pthis, int dummy, unsigned char flags, unsigned char type, const char *firstClass, const char *funcName, const char *argList, CASHookRegistration *reg) = NULL;

int (__fastcall *g_pfnCASDocumentation_RegisterObjectType)(CASDocumentation *pthis, int dummy, const char *docs, const char *name, int a4, unsigned int flags) = NULL;
int(__fastcall *g_oldCASDocumentation_RegisterObjectType)(CASDocumentation *pthis, int dummy, const char *docs, const char *name, int a4, unsigned int flags) = NULL;

int (__fastcall *g_pfnCASDocumentation_RegisterObjectProperty)(CASDocumentation *pthis, int dummy, const char *docs, const char *name, const char *prop, int offset) = NULL;

int (__fastcall *g_pfnCASDocumentation_RegisterObjectMethod)(CASDocumentation *pthis, int dummy, const char *docs, const char *objectname, const char *funcname, CASMethodRegistration *reg, int a5) = NULL;

#else

void(__cdecl *g_pfnCASHook_Call)(CASHook *pthis, int unk, ...) = NULL;

CASHook * (__cdecl *g_pfnCASHook_CASHook)(const CASHook *pthis, int flags, int type, const char *firstClass, const char *funcName, const char *argList, CASHookRegistration *reg) = NULL;

int(__cdecl *g_pfnCASDocumentation_RegisterObjectType)(CASDocumentation *pthis, const char *docs, const char *name, int a4, unsigned int flags) = NULL;

int(__cdecl *g_pfnCASDocumentation_RegisterObjectProperty)(CASDocumentation *pthis, const char *docs, const char *name, const char *prop, int offset) = NULL;

#endif

CASHook g_AddToFullPackHook = {0};

CASHook g_PlayerPostThinkPostHook = { 0 };

edict_t *g_ClientViewEntity[33] = { NULL };

static trace_t *(*g_oldSV_PushEntity)(trace_t * trace, edict_t *ent, float * push) = NULL;
static void(*g_oldSV_PushMove)(edict_t *pusher, float movetime) = NULL;
static void(*g_oldSV_PushRotate)(edict_t *pusher, float movetime) = NULL;

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

	if (g_NumPendingEntities < 512)
	{
		g_PendingEntities[g_NumPendingEntities] = ent;
		g_NumPendingEntities++;
	}

	if ((g_bIsPushMove || g_bIsPushRotate) && IsEntitySuperPusher(g_PusherEntity))
	{
		for (int i = 0; i < g_NumPendingEntities; ++i)
		{
			auto pPendingEntity = g_PendingEntities[i];
		
			if (IsEntityPushee(pPendingEntity) && pPendingEntity->v.groundentity != g_PusherEntity)
			{
				Vector vuser1 = g_PusherEntity->v.vuser1;
				Vector vpush;
				vpush.x = push[0];
				vpush.y = push[1];
				vpush.z = push[2];

				g_PusherEntity->v.sequence = 1919810;
				g_PusherEntity->v.vuser1 = vpush;

				gpGamedllFuncs->dllapi_table->pfnTouch(g_PusherEntity, pPendingEntity);

				g_PusherEntity->v.sequence = 114514;
				g_PusherEntity->v.vuser1 = vuser1;
#if 0
				vec3_t dir;
				dir.x = push[0];
				dir.y = push[1];
				dir.z = push[2];
				dir = dir.Normalize();

				if (g_PusherEntity->v.armorvalue > 0)
				{
					if (IsEntitySuperPusherFlexible(g_PusherEntity))
					{
						pPendingEntity->v.velocity = dir * g_PusherEntity->v.armorvalue * g_PusherEntity->v.speed;

						if (g_PusherEntity->v.max_health > 0)
						{
							if (pPendingEntity->v.velocity.z < g_PusherEntity->v.max_health * g_PusherEntity->v.speed)
								pPendingEntity->v.velocity.z = g_PusherEntity->v.max_health * g_PusherEntity->v.speed;
						}

						if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
						{
							vec3_t dir2 = pPendingEntity->v.origin - g_PusherEntity->v.origin;
							dir2.z = 0;
							dir2 = dir2.Normalize();
							pPendingEntity->v.velocity = pPendingEntity->v.velocity + dir2 * g_PusherEntity->v.armortype * g_PusherEntity->v.speed;
						}
					}
					else
					{
						pPendingEntity->v.velocity = dir * g_PusherEntity->v.armorvalue;
						
						if (g_PusherEntity->v.max_health > 0)
						{
							if (pPendingEntity->v.velocity.z < g_PusherEntity->v.max_health)
								pPendingEntity->v.velocity.z = g_PusherEntity->v.max_health;
						}

						if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
						{
							vec3_t dir2 = pPendingEntity->v.origin - g_PusherEntity->v.origin;
							dir2.z = 0;
							dir2 = dir2.Normalize();
							pPendingEntity->v.velocity = pPendingEntity->v.velocity + dir2 * g_PusherEntity->v.armortype;
						}
					}
				}
#endif
			}
		}
	}

	g_NumPendingEntities = 0;

	g_bIsPushEntity = false;

	return r;
}

void NewTouch(edict_t *pentTouched, edict_t *pentOther)
{
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

#if 0

void NewBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	if (IsEntitySuperRotator(pentBlocked) && IsEntityPushee(pentOther))
	{
		if (pentBlocked->v.armorvalue > 0)
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
		}
		SET_META_RESULT(MRES_SUPERCEDE);
	}
	else if (IsEntitySuperPusher(g_PusherEntity))
	{
		if (pentBlocked->v.armorvalue > 0)
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
		}
		SET_META_RESULT(MRES_SUPERCEDE);
	}

	SET_META_RESULT(MRES_IGNORED);
}

#endif

#ifdef PLATFORM_WINDOWS

edict_t *__fastcall CASEngineFuncs__GetViewEntity(void *pthis, int dummy, edict_t *pClient)
{
	auto viewent = g_ClientViewEntity[g_engfuncs.pfnEntOffsetOfPEntity(pClient)];
	if (viewent)
		return viewent;

	return pClient;
}

int __fastcall NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, int dummy, const char *docs, const char *name, int a4, unsigned int flags)
{
	if (name && docs && !strcmp(name, "CSurvivalMode") && !strcmp(docs, "Survival Mode handler") && flags == 0x40001u)
	{
		g_pfnCASDocumentation_RegisterObjectType(pthis, dummy, "Entity states transmit to client", "entity_state_t", 0, 0x40001u);
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "Fields which are filled in by routines outside of delta compression", "entity_state_t", "int entityType", offsetof(entity_state_t, entityType));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "Index into cl_entities array for this entity.", "entity_state_t", "int number", offsetof(entity_state_t, number));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float msg_time", offsetof(entity_state_t, msg_time));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "Message number last time the player/entity state was updated.", "entity_state_t", "int messagenum", offsetof(entity_state_t, messagenum));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "Fields which can be transitted and reconstructed over the network stream.", "entity_state_t", "Vector origin", offsetof(entity_state_t, origin));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "Fields which can be transitted and reconstructed over the network stream.", "entity_state_t", "Vector angles", offsetof(entity_state_t, angles));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int modelindex", offsetof(entity_state_t, modelindex));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int sequence", offsetof(entity_state_t, sequence));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float frame", offsetof(entity_state_t, frame));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int colormap", offsetof(entity_state_t, colormap));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int16 skin", offsetof(entity_state_t, skin));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int16 solid", offsetof(entity_state_t, solid));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int effects", offsetof(entity_state_t, effects));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float scale", offsetof(entity_state_t, scale));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int rendermode", offsetof(entity_state_t, rendermode));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int renderamt", offsetof(entity_state_t, renderamt));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int renderfx", offsetof(entity_state_t, renderfx));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int movetype", offsetof(entity_state_t, movetype));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float animtime", offsetof(entity_state_t, animtime));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float framerate", offsetof(entity_state_t, framerate));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int body", offsetof(entity_state_t, body));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector velocity", offsetof(entity_state_t, velocity));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector mins", offsetof(entity_state_t, mins));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector maxs", offsetof(entity_state_t, maxs));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int aiment", offsetof(entity_state_t, aiment));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int owner", offsetof(entity_state_t, owner));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float friction", offsetof(entity_state_t, friction));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float gravity", offsetof(entity_state_t, gravity));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int team", offsetof(entity_state_t, team));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int playerclass", offsetof(entity_state_t, playerclass));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int health", offsetof(entity_state_t, health));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int spectator", offsetof(entity_state_t, spectator));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int weaponmodel", offsetof(entity_state_t, weaponmodel));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int gaitsequence", offsetof(entity_state_t, gaitsequence));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector basevelocity", offsetof(entity_state_t, basevelocity));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int usehull", offsetof(entity_state_t, usehull));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int oldbuttons", offsetof(entity_state_t, oldbuttons));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int onground", offsetof(entity_state_t, onground));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int iStepLeft", offsetof(entity_state_t, iStepLeft));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float flFallVelocity", offsetof(entity_state_t, flFallVelocity));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float fov", offsetof(entity_state_t, fov));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int weaponanim", offsetof(entity_state_t, weaponanim));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int iuser1", offsetof(entity_state_t, iuser1));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int iuser2", offsetof(entity_state_t, iuser2));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int iuser3", offsetof(entity_state_t, iuser3));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "int iuser4", offsetof(entity_state_t, iuser4));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float fuser1", offsetof(entity_state_t, fuser1));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float fuser2", offsetof(entity_state_t, fuser2));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float fuser3", offsetof(entity_state_t, fuser3));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "float fuser4", offsetof(entity_state_t, fuser4));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector vuser1", offsetof(entity_state_t, vuser1));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector vuser2", offsetof(entity_state_t, vuser2));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector vuser3", offsetof(entity_state_t, vuser3));
		g_pfnCASDocumentation_RegisterObjectProperty(pthis, dummy, "", "entity_state_t", "Vector vuser4", offsetof(entity_state_t, vuser4));

		CASMethodRegistration reg;
		reg.pfnMethod = CASEngineFuncs__GetViewEntity;
		g_pfnCASDocumentation_RegisterObjectMethod(pthis, dummy, "Get view entity of client", "CEngineFuncs", "edict_t@ GetViewEntity(edict_t@ pClient)", &reg, 3);
	}

	return g_pfnCASDocumentation_RegisterObjectType(pthis, dummy, docs, name, a4, flags);
}

#else

int __cdecl NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, const char *docs, const char *name, int a4, unsigned int flags)
{
	return g_pfnCASDocumentation_RegisterObjectType(pthis, docs, name, a4, flags);
}
#endif

void NewSetupVisibility(struct edict_s *pViewEntity, struct edict_s *pClient, unsigned char **pvs, unsigned char **pas)
{
	auto clientIndex = g_engfuncs.pfnEntOffsetOfPEntity(pClient);
	g_ClientViewEntity[clientIndex] = pViewEntity;

	SET_META_RESULT(MRES_IGNORED);
}

int NewAddToFullPack_Post(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	int *pServerAddToFullPackReturnValue = (int *)gpMetaGlobals->orig_ret;
	if (*pServerAddToFullPackReturnValue == 1)
	{
		if (g_pfnCASHook_Call)
		{
			int uiFlags = 0;

			g_pfnCASHook_Call(&g_AddToFullPackHook, 0, state, e, ent, host, hostflags, player, &uiFlags);

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

void NewPlayerPostThink_Post(edict_t *pEntity)
{
	if (g_pfnCASHook_Call)
	{
		g_pfnCASHook_Call(&g_PlayerPostThinkPostHook, 0, pEntity->pvPrivateData);
	}

	SET_META_RESULT(MRES_IGNORED);
}

static DLL_FUNCTIONS gFunctionTable = 
{
	NULL,					// pfnGameInit
	NULL,					// pfnSpawn
	NULL,					// pfnThink
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
	NULL,					// pfnGameInit
	NULL,					// pfnSpawn
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

	NULL,					// pfnPM_Move
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

void *MH_SearchPattern(void *pStartSearch, size_t dwSearchLen, const char *pPattern, size_t dwPatternLen)
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

size_t MH_GetModuleSize(void *hModule)
{
#ifdef PLATFORM_WINDOWS
	return ((IMAGE_NT_HEADERS *)((char *)hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
#else
	return 0;
#endif
}

void *MH_GetModuleBase(const char *name)
{
#ifdef PLATFORM_WINDOWS
	return (void *)GetModuleHandleA(name);
#else
	return (void *)dlopen(name, RTLD_NOW | RTLD_NOLOAD);
#endif
}

#ifdef PLATFORM_WINDOWS

#define SV_PUSHENTITY_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x8B\x84\x24\x2A\x00\x00\x00"
#define SV_PUSHMOVE_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x2A\x8B\xBC\x24\x88\x00\x00\x00\xD9"
#define SV_PUSHROTATE_SVENGINE "\x81\xEC\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x84\x24\x2A\x00\x00\x00\x2A\x8B\xBC\x24\xC0\x00\x00\x00\xD9"

#define CASHOOK_CTOR_SVENGINE "\x8A\x44\x24\x2A\x2A\x54\x24\x2A\x2A\x8A\x5C\x24\x2A\x2A\x8B\xF1\xB9"
#define CASHOOK_CALL_SVENGINE "\x8B\x4C\x24\x04\x8D\x44\x24\x0C\x50\xFF\x74\x24\x0C\x6A\x00\xE8"

#define CASDOC_REG_OBJECT_TYPE_SVENGINE "\x68\x01\x00\x04\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8"
#define CASDOC_REG_OBJECT_PROP_SVENGINE "\x6A\x00\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x8B\xCE\xE8\x2A\x2A\x2A\x2A\x6A\x04"
#define CASDOC_REG_OBJECT_METHOD_SVENGINE "\x50\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x6A\x03"

#endif

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable,
	int *interfaceVersion)
{
	if (!pFunctionTable) {
		UTIL_LogPrintf("GetEntityAPI2_Post called with null pFunctionTable");
		return(FALSE);
	}
	else if (*interfaceVersion != INTERFACE_VERSION) {
		UTIL_LogPrintf("GetEntityAPI2_Post version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pFunctionTable, &gFunctionTable_Post, sizeof(DLL_FUNCTIONS));
	return(TRUE);
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

	auto engine = MH_GetModuleBase("hw.dll");

	if (!engine)
		engine = MH_GetModuleBase("swds.dll");

	CDetourManager::Init();

	if (engine)
	{
#ifdef PLATFORM_WINDOWS
		g_oldSV_PushEntity = (decltype(g_oldSV_PushEntity))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHENTITY_SVENGINE, sizeof(SV_PUSHENTITY_SVENGINE) - 1);
#else
		g_oldSV_PushEntity = (decltype(g_oldSV_PushEntity))dlsym(engine, "_Z13SV_PushEntityP7edict_sPf");
#endif
		if (g_oldSV_PushEntity)
		{
			auto detour = CDetourManager::CreateDetour((void *)NewSV_PushEntity, (void **)&g_pfnSV_PushEntity, (void *)g_oldSV_PushEntity);
			detour->EnableDetour();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushEntity");
		}
#ifdef PLATFORM_WINDOWS
		g_oldSV_PushMove = (decltype(g_oldSV_PushMove))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHMOVE_SVENGINE, sizeof(SV_PUSHMOVE_SVENGINE) - 1);
#else
		g_oldSV_PushMove = (decltype(g_oldSV_PushMove))dlsym(engine, "_Z11SV_PushMoveP7edict_sf");
#endif
		if (g_oldSV_PushMove)
		{
			auto detour = CDetourManager::CreateDetour((void *)NewSV_PushMove, (void **)&g_pfnSV_PushMove, (void *)g_oldSV_PushMove);
			detour->EnableDetour();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushMove");
		}
#ifdef PLATFORM_WINDOWS
		g_oldSV_PushRotate = (decltype(g_oldSV_PushRotate))MH_SearchPattern(engine, MH_GetModuleSize(engine), SV_PUSHROTATE_SVENGINE, sizeof(SV_PUSHROTATE_SVENGINE) - 1);
#else
		g_oldSV_PushRotate = (decltype(g_oldSV_PushRotate))dlsym(engine, "_Z13SV_PushRotateP7edict_sf");
#endif
		if (g_oldSV_PushRotate)
		{
			auto detour = CDetourManager::CreateDetour((void *)NewSV_PushRotate, (void **)&g_pfnSV_PushRotate, (void *)g_oldSV_PushRotate);
			detour->EnableDetour();
		}
		else
		{
			UTIL_LogPrintf("Failed to locate SV_PushRotate");
		}
	}
	else
	{
		UTIL_LogPrintf("Failed to get engine dll");
	}

	char szGameDir[64];
	g_engfuncs.pfnGetGameDir(szGameDir);

	if (!strcmp(szGameDir, "svencoop"))
	{
		auto server = MH_GetModuleBase("server.dll");
		
		if (server)
		{
#ifdef PLATFORM_WINDOWS
			g_pfnCASHook_CASHook = (decltype(g_pfnCASHook_CASHook))MH_SearchPattern(server, MH_GetModuleSize(server), CASHOOK_CTOR_SVENGINE, sizeof(CASHOOK_CTOR_SVENGINE) - 1);
#else
			g_pfnCASHook_CASHook = (decltype(g_pfnCASHook_CASHook))dlsym(engine, "__ZN7CASHookC2EhhPKcS1_S1_RK16CASHookArguments");
#endif
			if (g_pfnCASHook_CASHook)
			{
				if (1)
				{
					CASHookRegistration reg;
					reg.unk = 0;
					reg.stopMode = StopMode_CALL_ALL;
					reg.docs = "gEntityInterface.pfnAddToFullPack";

#ifdef PLATFORM_WINDOWS
					g_pfnCASHook_CASHook(&g_AddToFullPackHook, 0, 2, 3, "Player", "PlayerAddToFullPack", "entity_state_t@ state, int e, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out", &reg);
#else
					g_pfnCASHook_CASHook(&g_AddToFullPackHook, 2, 3, "Player", "PlayerAddToFullPack", "entity_state_t@ state, int e, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out", &reg);
#endif
				}
				if (1)
				{
					CASHookRegistration reg;
					reg.unk = 0;
					reg.stopMode = StopMode_CALL_ALL;
					reg.docs = "Post call of gEntityInterface.pfnPlayerPostThink";

#ifdef PLATFORM_WINDOWS
					g_pfnCASHook_CASHook(&g_PlayerPostThinkPostHook, 0, 2, 3, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer", &reg);
#else
					g_pfnCASHook_CASHook(&g_PlayerPostThinkPostHook, 2, 3, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer", &reg);
#endif
				}
			}
			else
			{
				UTIL_LogPrintf("Failed to locate CASHook_CASHook");
			}

#ifdef PLATFORM_WINDOWS
			g_pfnCASHook_Call = (decltype(g_pfnCASHook_Call))MH_SearchPattern(server, MH_GetModuleSize(server), CASHOOK_CALL_SVENGINE, sizeof(CASHOOK_CALL_SVENGINE) - 1);
#else
			g_pfnCASHook_Call = (decltype(g_pfnCASHook_Call))dlsym(server, "__ZN7CASHook4CallEiz");
#endif
			if (g_pfnCASHook_Call)
			{

			}
			else
			{
				UTIL_LogPrintf("Failed to locate CASHook_Call");
			}

			if (1)
			{
#ifdef PLATFORM_WINDOWS
			auto Caller = MH_SearchPattern(server, MH_GetModuleSize(server), CASDOC_REG_OBJECT_TYPE_SVENGINE, sizeof(CASDOC_REG_OBJECT_TYPE_SVENGINE) - 1);
			if (Caller)
			{
				g_pfnCASDocumentation_RegisterObjectType = (decltype(g_pfnCASDocumentation_RegisterObjectType))GetCallAddress((PUCHAR)Caller + sizeof(CASDOC_REG_OBJECT_TYPE_SVENGINE) - 1 - 1);
			}

#else
				g_pfnCASDocumentation_RegisterObjectType = (decltype(g_pfnCASDocumentation_RegisterObjectType))dlsym(server, "__ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im");

#endif
				if (g_pfnCASDocumentation_RegisterObjectType)
				{
					g_oldCASDocumentation_RegisterObjectType = g_pfnCASDocumentation_RegisterObjectType;

					auto detour = CDetourManager::CreateDetour((void *)NewCASDocumentation_RegisterObjectType, (void **)&g_pfnCASDocumentation_RegisterObjectType, (void *)g_oldCASDocumentation_RegisterObjectType);
					detour->EnableDetour();
				}
				else
				{
					UTIL_LogPrintf("Failed to locate CASDocumentation_RegisterObjectType");
				}
			}

			if (1)
			{
#ifdef PLATFORM_WINDOWS
				auto Caller = MH_SearchPattern(server, MH_GetModuleSize(server), CASDOC_REG_OBJECT_PROP_SVENGINE, sizeof(CASDOC_REG_OBJECT_PROP_SVENGINE) - 1);
				if (Caller)
				{
					g_pfnCASDocumentation_RegisterObjectProperty = (decltype(g_pfnCASDocumentation_RegisterObjectProperty))GetCallAddress((PUCHAR)Caller + sizeof(CASDOC_REG_OBJECT_PROP_SVENGINE) - 1 - 7);
				}

#else
				g_pfnCASDocumentation_RegisterObjectProperty = (decltype(g_pfnCASDocumentation_RegisterObjectProperty))dlsym(server, "__ZN16CASDocumentation18RegisterObjectTypeEPKcS1_im");

#endif
				if (g_pfnCASDocumentation_RegisterObjectProperty)
				{

				}
				else
				{
					UTIL_LogPrintf("Failed to locate CASDocumentation_RegisterObjectProperty");
				}

			}

			if (1)
			{
#ifdef PLATFORM_WINDOWS
				auto Caller = MH_SearchPattern(server, MH_GetModuleSize(server), CASDOC_REG_OBJECT_METHOD_SVENGINE, sizeof(CASDOC_REG_OBJECT_METHOD_SVENGINE) - 1);
				if (Caller)
				{
					g_pfnCASDocumentation_RegisterObjectMethod = (decltype(g_pfnCASDocumentation_RegisterObjectMethod))GetCallAddress((PUCHAR)Caller + sizeof(CASDOC_REG_OBJECT_METHOD_SVENGINE) - 1 - 7);
				}

#else
				g_pfnCASDocumentation_RegisterObjectMethod = (decltype(g_pfnCASDocumentation_RegisterObjectMethod))dlsym(server, "_ZN16CASDocumentation20RegisterObjectMethodEPKcS1_S1_RK10asSFuncPtrm");

#endif
				if (g_pfnCASDocumentation_RegisterObjectMethod)
				{

				}
				else
				{
					UTIL_LogPrintf("Failed to locate g_pfnCASDocumentation_RegisterObjectMethod");
				}

			}
		}
		else
		{
			UTIL_LogPrintf("Failed to get server dll");
		}
	}

	return(TRUE);
}
