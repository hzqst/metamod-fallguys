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

PRIVATE_FUNCTION_DEFINE(build_number);
PRIVATE_FUNCTION_DEFINE(SV_Physics);
PRIVATE_FUNCTION_DEFINE(SV_PushEntity);
PRIVATE_FUNCTION_DEFINE(SV_PushMove);
PRIVATE_FUNCTION_DEFINE(SV_PushRotate);
PRIVATE_FUNCTION_DEFINE(SV_WriteMovevarsToClient);
PRIVATE_FUNCTION_DEFINE(SV_SingleClipMoveToEntity);
PRIVATE_FUNCTION_DEFINE(SV_SingleClipMoveToEntity_10152);

const Vector g_vecZero = { 0, 0, 0 };

model_t* (*sv_models)[8192] = NULL;

double* host_frametime = NULL;

cvar_t* sv_gravity = NULL;
cvar_t* mp_footsteps = NULL;

edict_t* r_worldentity = NULL;
model_t* r_worldmodel = NULL;

playermove_t *pmove = NULL;
movevars_t *pmovevars = NULL;

areanode_t (*sv_areanodes)[32] = NULL;

int *pg_groupop = NULL;
int *pg_groupmask = NULL;

//8192 for SvEngine and 512 for GoldSrc
int EngineGetMaxPrecacheModel(void)
{
	return 8192;
}

model_t* EngineGetPrecachedModelByIndex(int i)
{
	return (*sv_models)[i];
}

msurface_t *EngineGetSurfaceByIndex(model_t *mod, int index)
{
	auto base = mod->surfaces;

	return base + index;
}

int EngineGetSurfaceIndex(model_t* mod, msurface_t *psurf)
{
	auto base = mod->surfaces;

	return psurf - base;
}

model_t* EngineFindWorldModelBySubModel(model_t* psubmodel)
{
	for (int i = 0; i < EngineGetMaxPrecacheModel(); ++i)
	{
		auto mod = EngineGetPrecachedModelByIndex(i);

		if (mod && mod->type == mod_brush && mod->name[0] && mod->name[0] != '*')
		{
			if (mod->needload == NL_PRESENT || mod->needload == NL_CLIENT)
			{
				if (mod->vertexes == psubmodel->vertexes)
					return mod;
			}
		}
	}
	return nullptr;
}

hull_t *SV_HullForBspNew(edict_t *ent, const vec3_t mins, const vec3_t maxs, vec3_t& offset)
{
	model_t		*model;
	hull_t		*hull;
	vec3_t		size;

	model = EngineGetPrecachedModelByIndex(ent->v.modelindex);

	if (!model || model->type != mod_brush)
		return NULL;

	size = (maxs - mins);
	if (size[0] <= 8)
	{
		hull = &model->hulls[0];
		offset = hull->clip_mins;
	}
	else
	{
		if (size[0] <= 36)
		{
			if (size[2] <= 36)
			{
				hull = &model->hulls[3];
				offset = hull->clip_mins - mins;
			}
			else
			{
				hull = &model->hulls[1];
				offset = hull->clip_mins - mins;
			}
		}
		else
		{
			hull = &model->hulls[2];
			offset = hull->clip_mins - mins;
		}
	}

	// calculate an offset value to center the origin
	offset = offset + ent->v.origin;
	return hull;
}

void SV_MoveBounds(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, vec3_t &boxmins, vec3_t &boxmaxs)
{
	int		i;

	for (i = 0; i < 3; i++)
	{
		if (end[i] > start[i])
		{
			boxmins[i] = start[i] + mins[i] - 1;
			boxmaxs[i] = end[i] + maxs[i] + 1;
		}
		else
		{
			boxmins[i] = end[i] + mins[i] - 1;
			boxmaxs[i] = start[i] + maxs[i] + 1;
		}
	}
}

void SV_SingleClipMoveToEntity(edict_t *ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t *trace)
{
	return g_call_original_SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, trace);
}

void SV_SingleClipMoveToEntity_10152(edict_t* ent, const vec3_t& start, const vec3_t& mins, const vec3_t& maxs, const vec3_t& end, trace_t* trace, edict_t *passedict)
{
	if (g_call_original_SV_SingleClipMoveToEntity_10152)
	{
		return g_call_original_SV_SingleClipMoveToEntity_10152(ent, start, mins, maxs, end, trace, passedict);
	}

	return g_call_original_SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, trace);
}

void SV_ClipToLinksEx(areanode_t *node, moveclip_t *clip)
{
	link_t		*l, *next;
	edict_t		*touch;
	trace_t		trace;

	// touch linked edicts
	for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
	{
		next = l->next;
		touch = EDICT_FROM_AREA(l);

		if (pg_groupop)
		{
			if (touch->v.groupinfo != 0 && clip->passedict && clip->passedict->v.groupinfo != 0)
			{
				if ((*pg_groupop) == GROUP_OP_AND && (touch->v.groupinfo & clip->passedict->v.groupinfo) == 0)
					continue;

				if ((*pg_groupop) == GROUP_OP_NAND && (touch->v.groupinfo & clip->passedict->v.groupinfo) != 0)
					continue;
			}
		}

		if (touch->v.solid == SOLID_NOT)
			continue;

		if (touch == clip->passedict)
			continue;

		if (gpNewDllFunctionsTable && gpNewDllFunctionsTable->pfnShouldCollide)
		{
			if (gpNewDllFunctionsTable->pfnShouldCollide(touch, clip->passedict) == 0)
				continue;
		}

		if (touch->v.solid == SOLID_BSP)
		{
			if ((touch->v.flags & FL_MONSTERCLIP) && !clip->monsterClipBrush)
				continue;
		}
		else
		{
			if (clip->type == MOVE_NORMAL)
			{
				if(touch->v.movetype != MOVETYPE_PUSH && touch->v.movetype != MOVETYPE_PUSHSTEP && touch->v.movetype != MOVETYPE_NOCLIP)
					continue;
			}
		}

		if (clip->ignoretrans && touch->v.rendermode != kRenderNormal && !(touch->v.flags & FL_WORLDBRUSH))
			continue;

		if (clip->boxmins[0] > touch->v.absmax[0]
			|| clip->boxmins[1] > touch->v.absmax[1]
			|| clip->boxmins[2] > touch->v.absmax[2]
			|| clip->boxmaxs[0] < touch->v.absmin[0]
			|| clip->boxmaxs[1] < touch->v.absmin[1]
			|| clip->boxmaxs[2] < touch->v.absmin[2])
			continue;

		//if (touch->v.solid != SOLID_SLIDEBOX && !SV_CheckSphereIntersection(touch, clip->start, clip->end))
		//	continue;

		if (clip->passedict && clip->passedict->v.size[0] && !touch->v.size[0])
			continue;	// points never interact

		// might intersect, so do an exact clip
		if (clip->trace.allsolid)
			return;

		if (clip->passedict)
		{
			if (touch->v.owner == clip->passedict)
				continue;	// don't clip against own missiles
			if (clip->passedict->v.owner == touch)
				continue;	// don't clip against owner
		}

		if ((int)touch->v.flags & FL_MONSTER)
		{
			SV_SingleClipMoveToEntity_10152(touch, clip->start, clip->mins2, clip->maxs2, clip->end, &trace, clip->passedict);
		}
		else
		{
			vec3_t tempmins(clip->mins[0], clip->mins[1], clip->mins[2]);
			vec3_t tempmaxs(clip->maxs[0], clip->maxs[1], clip->maxs[2]);
			SV_SingleClipMoveToEntity_10152(touch, clip->start, tempmins, tempmaxs, clip->end, &trace, clip->passedict);
		}

		if (trace.allsolid || trace.startsolid ||
			trace.fraction < clip->trace.fraction)
		{
			trace.ent = touch;
			if (clip->trace.startsolid)
			{
				clip->trace = trace;
				clip->trace.startsolid = true;
			}
			else
				clip->trace = trace;
		}
		else if (trace.startsolid)
			clip->trace.startsolid = true;
	}

	// recurse down both sides
	if (node->axis == -1)
		return;

	if (clip->boxmaxs[node->axis] > node->dist)
		SV_ClipToLinksEx(node->children[0], clip);
	if (clip->boxmins[node->axis] < node->dist)
		SV_ClipToLinksEx(node->children[1], clip);
}

trace_t SV_MoveEx(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush)
{
	moveclip_t	clip;
	int			i;
	vec3_t		worldEndPoint;
	float		worldFraction;

	memset(&clip, 0, sizeof(moveclip_t));

	// clip to world
	SV_SingleClipMoveToEntity_10152(g_engfuncs.pfnPEntityOfEntIndex(0), start, mins, maxs, end, &clip.trace, passedict);

	if (clip.trace.fraction != 0)
	{
		worldEndPoint = clip.trace.endpos;
		worldFraction = clip.trace.fraction;

		clip.trace.fraction = 1;
		clip.start = start;
		clip.end = worldEndPoint;
		clip.mins = mins;
		clip.maxs = maxs;
		clip.type = type & 0xFF;
		clip.ignoretrans = type >> 8;
		clip.passedict = passedict;
		clip.monsterClipBrush = monsterClipBrush;

		if (type == 2)
		{
			for (i = 0; i < 3; i++)
			{
				clip.mins2[i] = -15;
				clip.maxs2[i] = 15;
			}
		}
		else
		{
			clip.mins2 = mins;
			clip.maxs2 = maxs;
		}

		// create the bounding box of the entire move
		SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);

		// clip to entities
		SV_ClipToLinksEx(*sv_areanodes, &clip);

		gpGlobals->trace_ent = clip.trace.ent;
		clip.trace.fraction *= worldFraction;
	}

	return clip.trace;
}

void SV_SetGlobalTrace(trace_t *ptrace)
{
	gpGlobals->trace_allsolid = (float)ptrace->allsolid;
	gpGlobals->trace_startsolid = (float)ptrace->startsolid;

	gpGlobals->trace_inopen = (float)ptrace->inopen;
	gpGlobals->trace_inwater = (float)ptrace->inwater;

	gpGlobals->trace_endpos = ptrace->endpos;
	gpGlobals->trace_plane_normal = ptrace->plane.normal;

	gpGlobals->trace_fraction = ptrace->fraction;
	gpGlobals->trace_plane_dist = ptrace->plane.dist;

	if (ptrace->ent)
		gpGlobals->trace_ent = ptrace->ent;
	else
		gpGlobals->trace_ent = g_engfuncs.pfnPEntityOfEntIndex(0);

	gpGlobals->trace_hitgroup = ptrace->hitgroup;
}

edict_t *SV_TestEntityPositionEx(edict_t *ent)
{
	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? true : false;

	trace_t trace = SV_MoveEx(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, 0, ent, monsterClip);

	if (trace.startsolid)
	{
		SV_SetGlobalTrace(&trace);
		return trace.ent;
	}

	return NULL;
}

void NewSV_Physics()
{
	g_call_original_SV_Physics();

	gPhysicsManager.EntityEndFrame();
}

trace_t NewSV_PushEntity(edict_t* ent, vec3_t* push)
{
	g_bIsPushEntity = true;
	g_PushEntity = ent;

	//Here we collect players in NewTouch and add them into pending queue.
	auto r = g_call_original_SV_PushEntity(ent, push);

	//Do the push job for all players
	for (int i = 0; i < g_NumPendingEntities; ++i)
	{
		auto pPendingEntity = g_PendingEntities[i];

		g_PushEntity = pPendingEntity;

		g_call_original_SV_PushEntity(pPendingEntity, push);
	}

	//Add me into pending queue at final stage to commit the SuperPusher touch callback
	if (g_NumPendingEntities < (int)_ARRAYSIZE(g_PendingEntities))
	{
		g_PendingEntities[g_NumPendingEntities] = ent;
		g_NumPendingEntities++;
	}

	//Commit the transaction
	if ((g_bIsPushMove || g_bIsPushRotate) && 
		gPhysicsManager.IsEntitySuperPusher(g_PusherEntity))
	{
		for (int i = 0; i < g_NumPendingEntities; ++i)
		{
			auto pPendingEntity = g_PendingEntities[i];

			if (IsEntityPushee(pPendingEntity) && pPendingEntity->v.groundentity != g_PusherEntity)
			{
				if (g_bIsFallGuysSeason1)
				{
					if (g_PusherEntity->v.armorvalue > 0)
					{
						vec3_t dir = *push;
						dir = dir.Normalize();

						if (Legacy_IsEntitySuperPusherFlexible(g_PusherEntity))
						{
							pPendingEntity->v.velocity = dir * g_PusherEntity->v.armorvalue * g_PusherEntity->v.speed;

							if (g_PusherEntity->v.max_health > 0)
							{
								if (pPendingEntity->v.velocity.z < g_PusherEntity->v.max_health * g_PusherEntity->v.speed)
									pPendingEntity->v.velocity.z = g_PusherEntity->v.max_health * g_PusherEntity->v.speed;
							}

							if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
							{
								vec3_t dir2 = pPendingEntity->v.origin - g_PusherEntity->v.origin;
								dir2.z = 0;
								dir2 = dir2.Normalize();
								pPendingEntity->v.velocity = pPendingEntity->v.velocity + dir2 * g_PusherEntity->v.armortype * g_PusherEntity->v.speed;
							}
						}
						else
						{
							pPendingEntity->v.velocity = dir * g_PusherEntity->v.armorvalue;

							if (g_PusherEntity->v.max_health > 0)
							{
								if (pPendingEntity->v.velocity.z < g_PusherEntity->v.max_health)
									pPendingEntity->v.velocity.z = g_PusherEntity->v.max_health;
							}

							if (g_PusherEntity->v.avelocity[1] != 0 && g_PusherEntity->v.armortype > 0)
							{
								vec3_t dir2 = pPendingEntity->v.origin - g_PusherEntity->v.origin;
								dir2.z = 0;
								dir2 = dir2.Normalize();
								pPendingEntity->v.velocity = pPendingEntity->v.velocity + dir2 * g_PusherEntity->v.armortype;
							}
						}
					}
				}
				else
				{
					g_CurrentSuperPusherPushingVector = *push;
					g_CurrentSuperPusher = g_PusherEntity;

					gpGamedllFuncs->dllapi_table->pfnTouch(g_PusherEntity, pPendingEntity);

					g_CurrentSuperPusher = NULL;
				}
			}
		}
	}
	else if (g_bIsPushPhysics)
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

int NewSV_PushRotate(edict_t *pusher, float movetime)
{
	g_bIsPushRotate = true;
	g_PusherEntity = pusher;

	int result = g_call_original_SV_PushRotate(pusher, movetime);

	g_PusherEntity = NULL;
	g_bIsPushRotate = false;

	return result;
}

void NewSV_WriteMovevarsToClient(void *message)
{
	if (g_bUseCustomStepSound)
	{
		int footsteps = pmovevars->footsteps;
		pmovevars->footsteps = 0;

		g_call_original_SV_WriteMovevarsToClient(message);

		pmovevars->footsteps = footsteps;
		return;
	}

	g_call_original_SV_WriteMovevarsToClient(message);
}

hook_t *g_phook_SV_Physics = NULL;
hook_t *g_phook_SV_PushEntity = NULL;
hook_t *g_phook_SV_PushMove = NULL;
hook_t *g_phook_SV_PushRotate = NULL;
hook_t *g_phook_SV_WriteMovevarsToClient = NULL;

void InstallEngineHooks()
{
	INSTALL_INLINEHOOK(SV_Physics);
	INSTALL_INLINEHOOK(SV_PushEntity);
	INSTALL_INLINEHOOK(SV_PushMove);
	INSTALL_INLINEHOOK(SV_PushRotate);
	INSTALL_INLINEHOOK(SV_WriteMovevarsToClient);
}

void UninstallEngineHooks()
{
	UNINSTALL_HOOK(SV_Physics);
	UNINSTALL_HOOK(SV_PushEntity);
	UNINSTALL_HOOK(SV_PushMove);
	UNINSTALL_HOOK(SV_PushRotate);
	UNINSTALL_HOOK(SV_WriteMovevarsToClient);
}