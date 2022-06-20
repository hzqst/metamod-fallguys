#pragma once

#include "signatures.h"

#define NL_PRESENT 0
#define NL_NEEDS_LOADED 1
#define NL_UNREFERENCED 2
#define NL_CLIENT 3

#define SURF_PLANEBACK 2
#define SURF_DRAWSKY 4
#define SURF_DRAWSPRITE 8
#define SURF_DRAWTURB 0x10
#define SURF_DRAWTILED 0x20
#define SURF_DRAWBACKGROUND 0x40
#define SURF_UNDERWATER 0x80
#define SURF_DONTWARP 0x100

#ifdef PLATFORM_WINDOWS

#define ENGINE_DLL_NAME "hw.dll"

#else

#define ENGINE_DLL_NAME "hw.so"

#endif

//Engine private functions

// SV_PushEntity
typedef trace_t *(__cdecl *fnSV_PushEntity)(trace_t * trace, edict_t *ent, float * push);
trace_t* NewSV_PushEntity(trace_t* trace, edict_t* ent, float* push);
PRIVATE_FUNCTION_EXTERN(SV_PushEntity);

// SV_PushMove
typedef void(*fnSV_PushMove)(edict_t *pusher, float movetime);
void NewSV_PushMove(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushMove);

// SV_PushRotate
typedef void(*fnSV_PushRotate)(edict_t *pusher, float movetime);
void NewSV_PushRotate(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushRotate);

// SV_Physics_Step
typedef void(*fnSV_Physics_Step)(edict_t* ent);
void NewSV_Physics_Step(edict_t* ent);
PRIVATE_FUNCTION_EXTERN(SV_Physics_Step);

// SV_Physics_Toss
typedef void(*fnSV_Physics_Toss)(edict_t* ent);
void NewSV_Physics_Toss(edict_t* ent);
PRIVATE_FUNCTION_EXTERN(SV_Physics_Toss);

// SV_RunThink
typedef qboolean(*fnSV_RunThink)(edict_t* ent);
qboolean NewSV_RunThink(edict_t* ent);
PRIVATE_FUNCTION_EXTERN(SV_RunThink);

//model_t* sv_models[8192]
typedef struct model_s model_t;
extern model_t* (*sv_models)[8192];
extern double* host_frametime;