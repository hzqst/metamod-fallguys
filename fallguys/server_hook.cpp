#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

PRIVATE_FUNCTION_DEFINE(CASHook_Call);
PRIVATE_FUNCTION_DEFINE(CASHook_CASHook);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectType);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectProperty);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectMethod);

edict_t* SC_SERVER_DECL CASEngineFuncs__GetViewEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* pClient)
{
	return GetClientViewEntity(pClient);
}

int SC_SERVER_DECL CASEngineFuncs__GetRunPlayerMovePlayerIndex(void* pthis, SC_SERVER_DUMMYARG_NOCOMMA)
{
	return GetRunPlayerMovePlayerIndex();
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicBox(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	return gPhysicsManager.CreatePhysicBox(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySuperPusher(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, bool enable)
{
	return gPhysicsManager.SetEntitySuperPusher(ent, enable);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityLevelOfDetail(void* pthis, SC_SERVER_DUMMYARG edict_t* ent,
	int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)
{
	return gPhysicsManager.SetEntityLevelOfDetail(ent, flags, body_0, scale_0, body_1, scale_1, distance_1, body_2, scale_2, distance_2, body_3, scale_3, distance_3);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityPartialViewer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int partial_viewer_mask)
{
	return gPhysicsManager.SetEntityPartialViewer(ent, partial_viewer_mask);
}

edict_t *SC_SERVER_DECL CASEntityFuncs__GetCurrentSuperPusher(void* pthis, SC_SERVER_DUMMYARG Vector* out)
{
	return GetCurrentSuperPusher(out);
}

bool SC_SERVER_DECL CASEntityFuncs__ApplyImpulse(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, const Vector& impulse, const Vector& origin)
{
	return gPhysicsManager.ApplyImpulse(ent, impulse, origin);
}

int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int a4, unsigned int flags)
{
	if (name && docs && !strcmp(name, "CSurvivalMode") && !strcmp(docs, "Survival Mode handler") && flags == 0x40001u)
	{
		g_call_original_CASDocumentation_RegisterObjectType    (pthis, SC_SERVER_PASS_DUMMYARG "Entity states transmit to client", "entity_state_t", 0, 0x40001u);
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Fields which are filled in by routines outside of delta compression", "entity_state_t", "int entityType", offsetof(entity_state_t, entityType));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Index into cl_entities array for this entity.", "entity_state_t", "int number", offsetof(entity_state_t, number));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float msg_time", offsetof(entity_state_t, msg_time));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Message number last time the player/entity state was updated.", "entity_state_t", "int messagenum", offsetof(entity_state_t, messagenum));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Fields which can be transitted and reconstructed over the network stream.", "entity_state_t", "Vector origin", offsetof(entity_state_t, origin));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Fields which can be transitted and reconstructed over the network stream.", "entity_state_t", "Vector angles", offsetof(entity_state_t, angles));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int modelindex", offsetof(entity_state_t, modelindex));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int sequence", offsetof(entity_state_t, sequence));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float frame", offsetof(entity_state_t, frame));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int colormap", offsetof(entity_state_t, colormap));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int16 skin", offsetof(entity_state_t, skin));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int16 solid", offsetof(entity_state_t, solid));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int effects", offsetof(entity_state_t, effects));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float scale", offsetof(entity_state_t, scale));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int8 eflags", offsetof(entity_state_t, eflags));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int rendermode", offsetof(entity_state_t, rendermode));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int renderamt", offsetof(entity_state_t, renderamt));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int renderfx", offsetof(entity_state_t, renderfx));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int movetype", offsetof(entity_state_t, movetype));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float animtime", offsetof(entity_state_t, animtime));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float framerate", offsetof(entity_state_t, framerate));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int body", offsetof(entity_state_t, body));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector velocity", offsetof(entity_state_t, velocity));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector mins", offsetof(entity_state_t, mins));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector maxs", offsetof(entity_state_t, maxs));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int aiment", offsetof(entity_state_t, aiment));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int owner", offsetof(entity_state_t, owner));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float friction", offsetof(entity_state_t, friction));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float gravity", offsetof(entity_state_t, gravity));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int team", offsetof(entity_state_t, team));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int playerclass", offsetof(entity_state_t, playerclass));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int health", offsetof(entity_state_t, health));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int spectator", offsetof(entity_state_t, spectator));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int weaponmodel", offsetof(entity_state_t, weaponmodel));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int gaitsequence", offsetof(entity_state_t, gaitsequence));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector basevelocity", offsetof(entity_state_t, basevelocity));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int usehull", offsetof(entity_state_t, usehull));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int oldbuttons", offsetof(entity_state_t, oldbuttons));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int onground", offsetof(entity_state_t, onground));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iStepLeft", offsetof(entity_state_t, iStepLeft));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float flFallVelocity", offsetof(entity_state_t, flFallVelocity));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fov", offsetof(entity_state_t, fov));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int weaponanim", offsetof(entity_state_t, weaponanim));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser1", offsetof(entity_state_t, iuser1));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser2", offsetof(entity_state_t, iuser2));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser3", offsetof(entity_state_t, iuser3));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser4", offsetof(entity_state_t, iuser4));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser1", offsetof(entity_state_t, fuser1));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser2", offsetof(entity_state_t, fuser2));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser3", offsetof(entity_state_t, fuser3));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser4", offsetof(entity_state_t, fuser4));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser1", offsetof(entity_state_t, vuser1));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser2", offsetof(entity_state_t, vuser2));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser3", offsetof(entity_state_t, vuser3));
		g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser4", offsetof(entity_state_t, vuser4));

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEngineFuncs__GetRunPlayerMovePlayerIndex;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Get index of player that is currently running PlayerMove code", "CEngineFuncs", "int GetRunPlayerMovePlayerIndex()", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEngineFuncs__GetViewEntity;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Get view entity of specified client", "CEngineFuncs", "edict_t@ GetViewEntity(edict_t@ pClient)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__CreatePhysicBox;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Create physic box for entity", "CEntityFuncs", "bool CreatePhysicBox(edict_t@ ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__SetEntitySuperPusher;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Enable or disable Super-Pusher for entity", "CEntityFuncs", "bool SetEntitySuperPusher(edict_t@ ent, bool enable)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__SetEntityLevelOfDetail;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Enable Level-of-Detail for entity", "CEntityFuncs", "bool SetEntityLevelOfDetail(edict_t@ ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__SetEntityPartialViewer;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Create Level-of-Detail object for entity", "CEntityFuncs", "bool SetEntityPartialViewer(edict_t@ ent, int partial_viewer_mask)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__GetCurrentSuperPusher;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Get current working Super-Pusher, return valid edict only in pfnTouch", "CEntityFuncs", "edict_t@ GetCurrentSuperPusher(Vector &out)", &reg, 3);
		}

		if (1)
		{
			CASMethodRegistration reg;
			reg.pfnMethod = CASEntityFuncs__ApplyImpulse;
			g_call_original_CASDocumentation_RegisterObjectMethod(pthis, dummy, "Apply impulse on physic object", "CEntityFuncs", "bool ApplyImpulse(edict_t@ ent, const Vector& in impulse, const Vector& in origin)", &reg, 3);
		}
	}

	return g_call_original_CASDocumentation_RegisterObjectType(pthis, dummy, docs, name, a4, flags);
}