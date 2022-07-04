#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"

IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDocInitCallback);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectMethod);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectType);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterObjectProperty);
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterHook);

fnASEXT_CallHook *ASEXT_CallHook = NULL;

edict_t* SC_SERVER_DECL CASEngineFuncs__GetViewEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* pClient)
{
	return GetClientViewEntity(pClient);
}

int SC_SERVER_DECL CASEngineFuncs__GetRunPlayerMovePlayerIndex(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return GetRunPlayerMovePlayerIndex();
}

bool SC_SERVER_DECL CASEntityFuncs__CreateSolidOptimizer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int boneindex, const Vector& halfext, const Vector& halfext2)
{
	return gPhysicsManager.CreateSolidOptimizer(ent, boneindex, halfext, halfext2);
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicBox(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	return gPhysicsManager.CreatePhysicBox(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable);
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicSphere(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	return gPhysicsManager.CreatePhysicSphere(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable);
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

void RegisterAngelScriptMethods(void)
{
	ASEXT_RegisterDocInitCallback([](void *pASDoc) {

		ASEXT_RegisterObjectType    (pASDoc, "Entity states transmit to client", "entity_state_t", 0, 0x40001u);
		ASEXT_RegisterObjectProperty(pASDoc, "Fields which are filled in by routines outside of delta compression", "entity_state_t", "int entityType", offsetof(entity_state_t, entityType));
		ASEXT_RegisterObjectProperty(pASDoc, "Index into cl_entities array for this entity.", "entity_state_t", "int number", offsetof(entity_state_t, number));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float msg_time", offsetof(entity_state_t, msg_time));
		ASEXT_RegisterObjectProperty(pASDoc, "Message number last time the player/entity state was updated.", "entity_state_t", "int messagenum", offsetof(entity_state_t, messagenum));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector origin", offsetof(entity_state_t, origin));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector angles", offsetof(entity_state_t, angles));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int modelindex", offsetof(entity_state_t, modelindex));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int sequence", offsetof(entity_state_t, sequence));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float frame", offsetof(entity_state_t, frame));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int colormap", offsetof(entity_state_t, colormap));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int16 skin", offsetof(entity_state_t, skin));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int16 solid", offsetof(entity_state_t, solid));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int effects", offsetof(entity_state_t, effects));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float scale", offsetof(entity_state_t, scale));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int8 eflags", offsetof(entity_state_t, eflags));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int rendermode", offsetof(entity_state_t, rendermode));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int renderamt", offsetof(entity_state_t, renderamt));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int renderfx", offsetof(entity_state_t, renderfx));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int movetype", offsetof(entity_state_t, movetype));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float animtime", offsetof(entity_state_t, animtime));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float framerate", offsetof(entity_state_t, framerate));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int body", offsetof(entity_state_t, body));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector velocity", offsetof(entity_state_t, velocity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector mins", offsetof(entity_state_t, mins));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector maxs", offsetof(entity_state_t, maxs));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int aiment", offsetof(entity_state_t, aiment));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int owner", offsetof(entity_state_t, owner));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float friction", offsetof(entity_state_t, friction));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float gravity", offsetof(entity_state_t, gravity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int team", offsetof(entity_state_t, team));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int playerclass", offsetof(entity_state_t, playerclass));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int health", offsetof(entity_state_t, health));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int spectator", offsetof(entity_state_t, spectator));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int weaponmodel", offsetof(entity_state_t, weaponmodel));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int gaitsequence", offsetof(entity_state_t, gaitsequence));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector basevelocity", offsetof(entity_state_t, basevelocity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int usehull", offsetof(entity_state_t, usehull));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int oldbuttons", offsetof(entity_state_t, oldbuttons));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int onground", offsetof(entity_state_t, onground));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int iStepLeft", offsetof(entity_state_t, iStepLeft));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float flFallVelocity", offsetof(entity_state_t, flFallVelocity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float fov", offsetof(entity_state_t, fov));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int weaponanim", offsetof(entity_state_t, weaponanim));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int iuser1", offsetof(entity_state_t, iuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int iuser2", offsetof(entity_state_t, iuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int iuser3", offsetof(entity_state_t, iuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "int iuser4", offsetof(entity_state_t, iuser4));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float fuser1", offsetof(entity_state_t, fuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float fuser2", offsetof(entity_state_t, fuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float fuser3", offsetof(entity_state_t, fuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "float fuser4", offsetof(entity_state_t, fuser4));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector vuser1", offsetof(entity_state_t, vuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector vuser2", offsetof(entity_state_t, vuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector vuser3", offsetof(entity_state_t, vuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "entity_state_t", "Vector vuser4", offsetof(entity_state_t, vuser4));

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get index of player that is currently running PlayerMove code", "CEngineFuncs", "int GetRunPlayerMovePlayerIndex()",
			(void *)CASEngineFuncs__GetRunPlayerMovePlayerIndex, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get view entity of specified client", "CEngineFuncs", "edict_t@ GetViewEntity(edict_t@ pClient)",
			(void *)CASEngineFuncs__GetViewEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic box for entity", "CEntityFuncs", "bool CreateSolidOptimizer(edict_t@ ent, int boneindex, const Vector& in halfextent, const Vector& in halfextent2)",
			(void *)CASEntityFuncs__CreateSolidOptimizer, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic box for entity", "CEntityFuncs", "bool CreatePhysicBox(edict_t@ ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)",
			(void *)CASEntityFuncs__CreatePhysicBox, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic sphere for entity", "CEntityFuncs", "bool CreatePhysicSphere(edict_t@ ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)",
			(void *)CASEntityFuncs__CreatePhysicSphere, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable or disable Super-Pusher for brush entity", "CEntityFuncs", "bool SetEntitySuperPusher(edict_t@ ent, bool enable)",
			(void *)CASEntityFuncs__SetEntitySuperPusher, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable Level-of-Detail for entity", "CEntityFuncs", "bool SetEntityLevelOfDetail(edict_t@ ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)",
			(void *)CASEntityFuncs__SetEntityLevelOfDetail, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create Level-of-Detail object for entity", "CEntityFuncs", "bool SetEntityPartialViewer(edict_t@ ent, int partial_viewer_mask)",
			(void *)CASEntityFuncs__SetEntityPartialViewer, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get current working Super-Pusher, return valid edict only in pfnTouch", "CEntityFuncs", "edict_t@ GetCurrentSuperPusher(Vector &out)",
			(void *)CASEntityFuncs__GetCurrentSuperPusher, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Apply impulse on physic object", "CEntityFuncs", "bool ApplyImpulse(edict_t@ ent, const Vector& in impulse, const Vector& in origin)",
			(void *)CASEntityFuncs__ApplyImpulse, 3);

	});
}