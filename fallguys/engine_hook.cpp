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

PRIVATE_FUNCTION_DEFINE(SV_PushEntity);
PRIVATE_FUNCTION_DEFINE(SV_PushMove);
PRIVATE_FUNCTION_DEFINE(SV_PushRotate);
PRIVATE_FUNCTION_DEFINE(SV_Physics_Step);
PRIVATE_FUNCTION_DEFINE(SV_Physics_Toss);
PRIVATE_FUNCTION_DEFINE(SV_RunThink);

const Vector g_vecZero = { 0 };

model_t* (*sv_models)[8192] = NULL;

double* host_frametime = NULL;

edict_t* r_worldentity = NULL;
model_t* r_worldmodel = NULL;

trace_t* NewSV_PushEntity(trace_t* trace, edict_t* ent, float* push)
{
	g_bIsPushEntity = true;
	g_PushEntity = ent;

	auto r = g_call_original_SV_PushEntity(trace, ent, push);

	//push pending entities...
	for (int i = 0; i < g_NumPendingEntities; ++i)
	{
		auto pPendingEntity = g_PendingEntities[i];

		g_PushEntity = pPendingEntity;

		trace_t temp;
		g_call_original_SV_PushEntity(&temp, pPendingEntity, push);
	}

	if (g_NumPendingEntities < 512)
	{
		g_PendingEntities[g_NumPendingEntities] = ent;
		g_NumPendingEntities++;
	}

	if ((g_bIsPushMove || g_bIsPushRotate) && IsEntitySuperPusher(g_PusherEntity))
	{
		for (int i = 0; i < g_NumPendingEntities; ++i)
		{
			auto pPendingEntity = g_PendingEntities[i];

			if (IsEntityPushee(pPendingEntity) && pPendingEntity->v.groundentity != g_PusherEntity)
			{
				Vector vuser1 = g_PusherEntity->v.vuser1;
				g_PusherEntity->v.sequence = SuperPusherTouch_MagicNumber;
				g_PusherEntity->v.vuser1.x = push[0];
				g_PusherEntity->v.vuser1.y = push[1];
				g_PusherEntity->v.vuser1.z = push[2];

				gpGamedllFuncs->dllapi_table->pfnTouch(g_PusherEntity, pPendingEntity);

				g_PusherEntity->v.sequence = SuperPusher_MagicNumber;
				g_PusherEntity->v.vuser1 = vuser1;
			}
		}
	}

	g_NumPendingEntities = 0;

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

void NewSV_Physics_Step(edict_t* ent)
{
	g_call_original_SV_Physics_Step(ent);
}

void NewSV_Physics_Toss(edict_t* ent)
{
	if (IsEntitySuperPhysBox(ent))
	{
		gPhysicsManager.CreatePhysBox(ent);

		if (g_call_original_SV_RunThink(ent))
		{
			if (ent->v.velocity[2] > 0)
				ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

			if (!ent->v.groundentity || (ent->v.groundentity->v.flags & (FL_MONSTER | FL_CLIENT)))
				ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

			if ((ent->v.flags & FL_ONGROUND) && ent->v.velocity == g_vecZero)
			{
				ent->v.avelocity = g_vecZero;
				if (ent->v.basevelocity == g_vecZero)
					return;
			}

			//SV_CheckVelocity(ent);


		}
	}
	else
	{
		g_call_original_SV_Physics_Toss(ent);
	}
}