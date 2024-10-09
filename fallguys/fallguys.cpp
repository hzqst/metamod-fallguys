#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

//For Fall Guys Season 1 compatibility
bool g_bIsFallGuysSeason1 = false;

//For Custom StepSound
bool g_bUseCustomStepSound = false;

//For SuperPusher Entities
bool g_bIsPushPhysics = false;
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
void * g_AddToFullPackHook = NULL;
void * g_PlayerPostThinkPostHook = NULL;
void * g_PlayerTouchTriggerHook = NULL;
void * g_PlayerTouchImpactHook = NULL;
void * g_PlayerTouchPlayerHook = NULL;
void * g_PlayerMovePlayStepSoundHook = NULL;
void * g_PlayerMovePlaySoundFXHook = NULL;

bool IsEntitySolidPlayer(int entindex, edict_t* ent)
{
	return entindex >= 1 && entindex <= gpGlobals->maxClients && ent->v.solid >= SOLID_BBOX && ent->v.solid <= SOLID_SLIDEBOX && ent->v.movetype != MOVETYPE_NOCLIP;
}

bool IsEntitySolidPlayer(edict_t* ent)
{
	return IsEntitySolidPlayer(g_engfuncs.pfnIndexOfEdict(ent), ent);
}

bool IsEntitySolidPusher(edict_t* ent)
{
	return ent->v.solid == SOLID_BSP && ent->v.movetype == MOVETYPE_PUSH;
}

bool Legacy_IsEntitySuperPusher(edict_t *ent)
{
	return ent && (ent->v.sequence >= 114514 || ent->v.sequence <= 114515);
}

bool Legacy_IsEntitySuperPusherFlexible(edict_t *ent)
{
	return ent && ent->v.sequence == 114515;
}

bool Legacy_IsEntitySuperRotator(edict_t *ent)
{
	return ent && ent->v.sequence == 114516;
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
	if (!g_bIsRunPlayerMove)
		return 0;

	return g_iRunPlayerMoveIndex;
}

static float g_saved_footsteps = 1;

void EnableCustomStepSound(bool bEnabled)
{
	if (!g_bUseCustomStepSound && bEnabled)
	{
		g_saved_footsteps = mp_footsteps->value;
		mp_footsteps->value = 0;

		g_bUseCustomStepSound = true;
	}
	else if (g_bUseCustomStepSound && !bEnabled)
	{
		g_bUseCustomStepSound = false;

		g_engfuncs.pfnCVarSetFloat("mp_footsteps", g_saved_footsteps);
	}
}

void RegisterAngelScriptHooks()
{
	g_AddToFullPackHook = ASEXT_RegisterHook("Post call of gEntityInterface.pfnAddToFullPack, only visible entity goes here", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerAddToFullPack", "entity_state_t@ state, int entindex, edict_t @ent, edict_t@ host, int hostflags, int player, uint& out result");

	g_PlayerPostThinkPostHook = ASEXT_RegisterHook("Post call of gEntityInterface.pfnPlayerPostThink", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer");

	g_PlayerTouchTriggerHook = ASEXT_RegisterHook("Get called when player touches a trigger", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerTouchTrigger", "CBasePlayer@ pPlayer, CBaseEntity@ pOther");

	g_PlayerTouchImpactHook = ASEXT_RegisterHook("Get called when player impacts a solid entities or world, player's velocity is temporarily set to impactvelocity", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerTouchImpact", "CBasePlayer@ pPlayer, CBaseEntity@ pOther");

	g_PlayerTouchPlayerHook = ASEXT_RegisterHook("Get called when player touches a player, player's velocity is temporarily set to impactvelocity", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerTouchPlayer", "CBasePlayer@ pPlayer, CBasePlayer@ pOther");

	g_PlayerMovePlayStepSoundHook = ASEXT_RegisterHook("Get called when playing step sound", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerMovePlayStepSound", "CBasePlayer@ pPlayer, playermove_t@ playermove, int iType, float flVolume, bool bIsJump, uint& out uiFlags");

	g_PlayerMovePlaySoundFXHook = ASEXT_RegisterHook("Get called when playing player move sound", StopMode_CALL_ALL, 2, ASFlag_MapScript | ASFlag_Plugin, "Player", "PlayerMovePlaySoundFX", "CBasePlayer@ pPlayer, int playerindex, Vector origin, int type, const string& in sound, float vol, float att, int flags, int pitch, uint& out uiFlags");
}
