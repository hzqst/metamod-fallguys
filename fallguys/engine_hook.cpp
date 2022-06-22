#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>
#include <com_model.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

PRIVATE_FUNCTION_DEFINE(SV_TestEntityPosition);
PRIVATE_FUNCTION_DEFINE(SV_PushEntity);
PRIVATE_FUNCTION_DEFINE(SV_PushMove);
PRIVATE_FUNCTION_DEFINE(SV_PushRotate);
PRIVATE_FUNCTION_DEFINE(SV_Physics_Step);
PRIVATE_FUNCTION_DEFINE(SV_Physics_Toss);
PRIVATE_FUNCTION_DEFINE(SV_RunThink);
PRIVATE_FUNCTION_DEFINE(PM_PlayerTrace);

const Vector g_vecZero = { 0, 0, 0 };

model_t* (*sv_models)[8192] = NULL;

double* host_frametime = NULL;

cvar_t* sv_gravity = NULL;

edict_t* r_worldentity = NULL;
model_t* r_worldmodel = NULL;

playermove_t *pmove = NULL;

trace_t* NewSV_PushEntity(trace_t* trace, edict_t* ent, vec3_t* push)
{
	g_bIsPushEntity = true;
	g_PushEntity = ent;

	//Here we collect players in NewTouch and add them into pending queue.
	auto r = g_call_original_SV_PushEntity(trace, ent, push);

	//Do the push job for all players
	for (int i = 0; i < g_NumPendingEntities; ++i)
	{
		auto pPendingEntity = g_PendingEntities[i];

		g_PushEntity = pPendingEntity;

		trace_t temp;
		g_call_original_SV_PushEntity(&temp, pPendingEntity, push);
	}

	//Add me into pending queue at final stage to commit the SuperPusher touch callback
	if (g_NumPendingEntities < _ARRAYSIZE(g_PendingEntities))
	{
		g_PendingEntities[g_NumPendingEntities] = ent;
		g_NumPendingEntities++;
	}

	//Commit the transaction
	if ((g_bIsPushMove || g_bIsPushRotate || g_bIsPushPhysicEngnie) && gPhysicsManager.IsSuperPusher(g_PusherEntity))
	{
		for (int i = 0; i < g_NumPendingEntities; ++i)
		{
			auto pPendingEntity = g_PendingEntities[i];

			if (IsEntityPushee(pPendingEntity) && pPendingEntity->v.groundentity != g_PusherEntity)
			{
				g_CurrentSuperPusherPushingVector = *push;
				g_CurrentSuperPusher = g_PusherEntity;

				gpGamedllFuncs->dllapi_table->pfnTouch(g_PusherEntity, pPendingEntity);

				g_CurrentSuperPusher = NULL;
			}
		}
	}

	g_NumPendingEntities = 0;
	g_PushEntity = NULL;
	g_bIsPushEntity = false;

	return r;
}

void NewSV_PushMove(edict_t *pusher, float movetime)
{
	g_bIsPushMove = true;
	g_PusherEntity = pusher;

	g_call_original_SV_PushMove(pusher, movetime);

	g_PusherEntity = NULL;
	g_bIsPushMove = false;
}

void NewSV_PushRotate(edict_t *pusher, float movetime)
{
	g_bIsPushRotate = true;
	g_PusherEntity = pusher;

	g_call_original_SV_PushRotate(pusher, movetime);

	g_PusherEntity = NULL;
	g_bIsPushRotate = false;
}
#if 0
pmtrace_t *NewPM_PlayerTrace(pmtrace_t *results, const float *start, const float *end, int traceFlags, int numphysent, physent_t *physents, int ignore_pe, int(__cdecl *pfnIgnore)(physent_t *pe))
{
	g_call_original_PM_PlayerTrace(results, start, end, traceFlags, numphysent, physents, ignore_pe, pfnIgnore);

	if (gPhysicsManager.IsRunningPlayerMove() && traceFlags == PM_NORMAL && ignore_pe == -1 && results->fraction > 0)
	{
		vec3_t _start(start[0], start[1], start[2]);
		vec3_t _end(end[0], end[1], end[2]);
		vec3_t _angles(pmove->angles);
		_angles.x = 0;//no pitch
		_angles.y = 0;
		_angles.z = 0;

		vec3_t _mins = pmove->player_mins[pmove->usehull];
		vec3_t _maxs = pmove->player_maxs[pmove->usehull];

		gPhysicsManager.PM_BoxTrace(_start, _end, _angles, _mins, _maxs, traceFlags, numphysent, physents, ignore_pe, results);
	}
	return results;
}
#endif