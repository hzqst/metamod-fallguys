#pragma once

#include "signatures.h"

#include <edict.h>
#include <const.h>
#include <com_model.h>
#include <pmtrace.h>
#include <pm_defs.h>
#include <pm_movevars.h>

#include <entity_state.h>

#define MOVE_NORMAL 0
#define MOVE_NOMONSTERS 1
#define MOVE_MISSILE 2

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

typedef struct
{
	vec3_t		boxmins, boxmaxs;// enclose the test object along entire move
	const float		*mins, *maxs;	// size of the moving object
	vec3_t		mins2, maxs2;	// size when clipping against mosnters
	float		*start, *end;
	trace_t		trace;
	short			type;
	short			ignoretrans;
	edict_t		*passedict;
	qboolean	monsterClipBrush;
} moveclip_t;

typedef struct areanode_s
{
	int		axis;		// -1 = leaf node
	float	dist;
	struct areanode_s	*children[2];
	link_t	trigger_edicts;
	link_t	solid_edicts;
} areanode_t;

//Engine private functions

typedef int(*fnbuild_number)();
PRIVATE_FUNCTION_EXTERN(build_number);

typedef void(*fnSV_Physics)();
void NewSV_Physics();
PRIVATE_FUNCTION_EXTERN(SV_Physics);

// SV_PushEntity
typedef trace_t (*fnSV_PushEntity)(edict_t *ent, vec3_t * push);
trace_t NewSV_PushEntity(edict_t* ent, vec3_t* push);
PRIVATE_FUNCTION_EXTERN(SV_PushEntity);

// SV_PushMove
typedef void(*fnSV_PushMove)(edict_t *pusher, float movetime);
void NewSV_PushMove(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushMove);

// SV_PushRotate
typedef int(*fnSV_PushRotate)(edict_t *pusher, float movetime);
int NewSV_PushRotate(edict_t *pusher, float movetime);
PRIVATE_FUNCTION_EXTERN(SV_PushRotate);

typedef void(*fnSV_WriteMovevarsToClient)(void *message);
void NewSV_WriteMovevarsToClient(void *message);
PRIVATE_FUNCTION_EXTERN(SV_WriteMovevarsToClient);

typedef edict_t *(*fnSV_TestEntityPosition)(edict_t *ent);
edict_t *SV_TestEntityPositionEx(edict_t *ent);
PRIVATE_FUNCTION_EXTERN(SV_TestEntityPosition);

//typedef void(*fnSV_LinkEdict)(edict_t *ent, qboolean touch_triggers);
//void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);
//PRIVATE_FUNCTION_EXTERN(SV_LinkEdict);

typedef void(*fnSV_SingleClipMoveToEntity)(edict_t *ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t *trace);
void SV_SingleClipMoveToEntity(edict_t *ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t *trace);
PRIVATE_FUNCTION_EXTERN(SV_SingleClipMoveToEntity);

typedef void(*fnSV_SingleClipMoveToEntity_10152)(edict_t* ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t* trace, edict_t* passedict);
void SV_SingleClipMoveToEntity_10152(edict_t* ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t* trace, edict_t* passedict);
PRIVATE_FUNCTION_EXTERN(SV_SingleClipMoveToEntity_10152);

hull_t *SV_HullForBspNew(edict_t *ent, const vec3_t mins, const vec3_t maxs, vec3_t& offset);

extern model_t* (*sv_models)[8192];
extern double* host_frametime;
extern cvar_t* sv_gravity;
extern cvar_t* mp_footsteps;
extern playermove_t *pmove;
extern movevars_t *pmovevars;
extern areanode_t (*sv_areanodes)[32];
extern int *pg_groupop;
extern int *pg_groupmask;

int EngineGetMaxPrecacheModel(void);
model_t* EngineGetPrecachedModelByIndex(int i);
msurface_t *EngineGetSurfaceByIndex(model_t* mod, int index);
int EngineGetSurfaceIndex(model_t* mod, msurface_t* psurf);
model_t* EngineFindWorldModelBySubModel(model_t* psubmodel);