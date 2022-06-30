#pragma once

//For SuperPusher Brush Entities
extern bool g_bIsPushMove;
extern bool g_bIsPushRotate;
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
extern void * g_AddToFullPackHook;
extern void * g_PlayerPostThinkPostHook;
extern void * g_PlayerTouchTriggerHook;
extern void * g_PlayerTouchImpactHook;

bool IsEntitySolidPlayer(int entindex, edict_t* ent);
bool IsEntitySolidPlayer(edict_t* ent);
bool IsEntitySolidPusher(edict_t* ent);
bool IsEntityPushee(edict_t* ent);

int GetRunPlayerMovePlayerIndex();
edict_t* GetCurrentSuperPusher(Vector* out);
edict_t* GetClientViewEntity(int clientindex);
edict_t* GetClientViewEntity(edict_t* pClient);

void InstallEngineHooks();
void RegisterAngelScriptHooks();
void RegisterAngelScriptMethods(void);

#define LOD_BODY 1
#define LOD_MODELINDEX 2
#define LOD_SCALE 4
#define LOD_SCALE_INTERP 8