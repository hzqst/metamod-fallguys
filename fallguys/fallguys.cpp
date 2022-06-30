#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

//For SuperPusher
bool g_bIsPushMove = false;
bool g_bIsPushRotate = false;
edict_t* g_PusherEntity = NULL;
bool g_bIsPushEntity = false;
edict_t* g_PushEntity;
edict_t* g_PendingEntities[512] = { 0 };
int g_NumPendingEntities = 0;

edict_t *g_CurrentSuperPusher = NULL;
vec3_t g_CurrentSuperPusherPushingVector = g_vecZero;

//For PlayerTouchImpact & PlayerTouchTrigger
bool g_bIsRunPlayerMove = false;
int g_iRunPlayerMoveIndex = 0;

//For GetViewEntity
edict_t* g_ClientViewEntity[33] = { 0 };

//For AngelScript Hook
CASHook g_AddToFullPackHook = { 0 };
CASHook g_PlayerPostThinkPostHook = { 0 };
CASHook g_PlayerTouchTriggerHook = { 0 };
CASHook g_PlayerTouchImpactHook = { 0 };

bool IsEntitySolidPlayer(int entindex, edict_t* ent)
{
	return entindex >= 1 && entindex <= gpGlobals->maxClients && ent->v.solid >= SOLID_BBOX && ent->v.solid <= SOLID_SLIDEBOX;
}

bool IsEntitySolidPlayer(edict_t* ent)
{
	return IsEntitySolidPlayer(g_engfuncs.pfnIndexOfEdict(ent), ent);
}

bool IsEntitySolidPusher(edict_t* ent)
{
	return ent->v.solid == SOLID_BSP && ent->v.movetype == MOVETYPE_PUSH;
}

bool IsEntityPushee(edict_t* ent)
{
	return (ent->v.solid == SOLID_SLIDEBOX || ent->v.solid == SOLID_BBOX) && (ent->v.movetype == MOVETYPE_STEP || ent->v.movetype == MOVETYPE_WALK);
}

edict_t *GetCurrentSuperPusher(Vector *out)
{
	if (g_CurrentSuperPusher)
	{
		*out = g_CurrentSuperPusherPushingVector;

		return g_CurrentSuperPusher;
	}

	*out = g_vecZero;

	return NULL;
}

int GetRunPlayerMovePlayerIndex()
{
	return g_iRunPlayerMoveIndex;
}

void FG_InstallInlineHooks()
{
	INSTALL_INLINEHOOK(SV_PushEntity);
	INSTALL_INLINEHOOK(SV_PushMove);
	INSTALL_INLINEHOOK(SV_PushRotate);
	INSTALL_INLINEHOOK(CASDocumentation_RegisterObjectType);
}

void FG_RegisterAngelScriptHook_AddToFullPack(int dummy)
{
	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = StopMode_CALL_ALL;
	reg.docs = "Post call of gEntityInterface.pfnAddToFullPack, only visible entity goes here";

	g_pfn_CASHook_CASHook(&g_AddToFullPackHook, SC_SERVER_PASS_DUMMYARG 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerAddToFullPack", "entity_state_t@ state, int entindex, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out", &reg);
}

void FG_RegisterAngelScriptHook_PlayerPostThinkPost(int dummy)
{
	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = StopMode_CALL_ALL;
	reg.docs = "Post call of gEntityInterface.pfnPlayerPostThink";

	g_pfn_CASHook_CASHook(&g_PlayerPostThinkPostHook, SC_SERVER_PASS_DUMMYARG 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer", &reg);
}

void FG_RegisterAngelScriptHook_PlayerTouchTrigger(int dummy)
{
	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = StopMode_CALL_ALL;
	reg.docs = "Get called when player touches a trigger";

	g_pfn_CASHook_CASHook(&g_PlayerTouchTriggerHook, SC_SERVER_PASS_DUMMYARG 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerTouchTrigger", "CBasePlayer@ pPlayer, CBaseEntity@ pOther", &reg);
}

void FG_RegisterAngelScriptHook_PlayerTouchImpact(int dummy)
{
	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = StopMode_CALL_ALL;
	reg.docs = "Get called when player impacts a solid entities or world, player's velocity is temporarily set to impactvelocity";

	g_pfn_CASHook_CASHook(&g_PlayerTouchImpactHook, SC_SERVER_PASS_DUMMYARG 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerTouchImpact", "CBasePlayer@ pPlayer, CBaseEntity@ pOther", &reg);
}

void FG_RegisterAngelScriptHooks()
{
	FG_RegisterAngelScriptHook_AddToFullPack(0);
	FG_RegisterAngelScriptHook_PlayerPostThinkPost(0);
	FG_RegisterAngelScriptHook_PlayerTouchTrigger(0);
	FG_RegisterAngelScriptHook_PlayerTouchImpact(0);
}