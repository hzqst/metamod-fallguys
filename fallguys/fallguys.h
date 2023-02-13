#pragma once

//For Fall Guys Season 1 compatibility
extern bool g_bIsFallGuysSeason1;

//For Custom StepSound
extern bool g_bUseCustomStepSound;

//For SuperPusher Entities
extern bool g_bIsPushPhysics;
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
extern void * g_PlayerTouchPlayerHook;
extern void * g_PlayerMovePlayStepSoundHook;
extern void * g_PlayerMovePlaySoundFXHook;

bool IsEntitySolidPlayer(int entindex, edict_t* ent);
bool IsEntitySolidPlayer(edict_t* ent);
bool IsEntitySolidPusher(edict_t* ent);
bool IsEntityPushee(edict_t* ent);

bool Legacy_IsEntitySuperPusher(edict_t *ent);
bool Legacy_IsEntitySuperPusherFlexible(edict_t *ent);
bool Legacy_IsEntitySuperRotator(edict_t *ent);

int GetRunPlayerMovePlayerIndex();
edict_t* GetCurrentSuperPusher(Vector* out);
edict_t* GetClientViewEntity(int clientindex);
edict_t* GetClientViewEntity(edict_t* pClient);
bool SetClientViewEntity(edict_t* pClient, edict_t*pViewEnt);

void EnableCustomStepSound(bool bEnabled);

void InstallServerHooks();
void UninstallServerHooks();
void InstallEngineHooks();
void UninstallEngineHooks();
void RegisterAngelScriptHooks();
void RegisterAngelScriptMethods(void);