#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"

PRIVATE_FUNCTION_DEFINE(SV_PushEntity);
PRIVATE_FUNCTION_DEFINE(SV_PushMove);
PRIVATE_FUNCTION_DEFINE(SV_PushRotate);

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
				g_PusherEntity->v.sequence = 1919810;
				g_PusherEntity->v.vuser1.x = push[0];
				g_PusherEntity->v.vuser1.y = push[1];
				g_PusherEntity->v.vuser1.z = push[2];

				gpGamedllFuncs->dllapi_table->pfnTouch(g_PusherEntity, pPendingEntity);

				g_PusherEntity->v.sequence = 114514;
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