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

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get index of player that is currently running PlayerMove code", "CEngineFuncs", "int GetRunPlayerMovePlayerIndex()",
			(void *)CASEngineFuncs__GetRunPlayerMovePlayerIndex, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get view entity of specified client", "CEngineFuncs", "edict_t@ GetViewEntity(edict_t@ pClient)",
			(void *)CASEngineFuncs__GetViewEntity, 3);

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
			(void *)CASEntityFuncs__CreatePhysicSphere, 3);

	});
}