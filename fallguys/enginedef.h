#pragma once

#include "signatures.h"

#include <com_model.h>
#include <pmtrace.h>
#include <pm_defs.h>

#include <entity_state.h>

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

#define FMODEL_ROCKET 0x1
#define FMODEL_GRENADE 0x2
#define FMODEL_GIB 0x4
#define FMODEL_ROTATE 0x8
#define FMODEL_TRACER 0x10
#define FMODEL_ZOMGIB 0x20
#define FMODEL_TRACER2 0x40
#define FMODEL_TRACER3 0x80
#define FMODEL_DYNAMIC_LIGHT 0x100
#define FMODEL_TRACE_HITBOX 0x200

//Engine private functions

// SV_PushEntity
typedef trace_t (*fnSV_PushEntity)(edict_t *ent, vec3_t * push);
trace_t NewSV_PushEntity(edict_t* ent, vec3_t* push);
PRIVATE_FUNCTION_EXTERN(SV_PushEntity);

// SV_PushMove
typedef void(*fnSV_PushMove)(edict_t *pusher, float movetime);
void NewSV_PushMove(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushMove);

// SV_PushRotate
typedef void(*fnSV_PushRotate)(edict_t *pusher, float movetime);
void NewSV_PushRotate(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushRotate);

//model_t* sv_models[8192]
extern model_t* (*sv_models)[8192];
extern double* host_frametime;
extern cvar_t* sv_gravity;
extern playermove_t *pmove;