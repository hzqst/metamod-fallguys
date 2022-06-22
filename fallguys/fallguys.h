#pragma once

const int SuperPusher_MagicNumber = 114514;
const int SuperPusherTouch_MagicNumber = 1919810;

//For SuperPusher
extern bool g_bIsPushMove;
extern bool g_bIsPushRotate;
extern bool g_bIsPushPhysicEngnie;
extern edict_t* g_PusherEntity;
extern bool g_bIsPushEntity;
extern edict_t* g_PushEntity;
extern edict_t* g_PendingEntities[512];
extern int g_NumPendingEntities;

extern edict_t *g_CurrentSuperPusher;
extern vec3_t g_CurrentSuperPusherPushingVector;

//For PlayerTouchImpact & PlayerTouchTrigger
extern bool g_bIsRunPlayerMove;
extern int g_iRunPlayerMoveIndex;

//For GetViewEntity
extern edict_t* g_ClientViewEntity[33];

//For AngelScript Hook
extern CASHook g_AddToFullPackHook;
extern CASHook g_PlayerPostThinkPostHook;
extern CASHook g_PlayerTouchTriggerHook;
extern CASHook g_PlayerTouchImpactHook;

bool IsEntitySolidPlayer(int entindex, edict_t* ent);
bool IsEntitySolidPlayer(edict_t* ent);
bool IsEntitySolidPusher(edict_t* ent);
bool IsEntityPushee(edict_t* ent);
edict_t *GetCurrentSuperPusher(Vector *out);
int GetRunPlayerMovePlayerIndex();

void FG_InstallInlineHooks();
void FG_RegisterAngelScriptHooks();