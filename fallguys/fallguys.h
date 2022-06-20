#pragma once

const int SuperPusher_MagicNumber = 114514;
const int SuperPusherTouch_MagicNumber = 1919810;
const int SuperPhysBox_MagicNumber = 114515;

//For SuperPusher
extern bool g_bIsPushMove;
extern bool g_bIsPushRotate;
extern edict_t* g_PusherEntity;
extern bool g_bIsPushEntity;
extern edict_t* g_PushEntity;
extern edict_t* g_PendingEntities[512];
extern int g_NumPendingEntities;

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

bool IsEntitySuperPhysBox(edict_t* ent);
bool IsEntitySuperPusher(edict_t* ent);
bool IsEntityPushee(edict_t* ent);

void FG_InstallInlineHooks();
void FG_RegisterAngelScriptHooks();