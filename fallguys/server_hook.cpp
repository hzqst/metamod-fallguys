#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"
#include "physics.h"
#include "soundengine.h"

PRIVATE_FUNCTION_DEFINE(CPlayerMove_PlayStepSound);
PRIVATE_FUNCTION_DEFINE(PM_PlaySoundFX_SERVER);

void SC_SERVER_DECL CASEngineFuncs__SetPhysicSimRate(void* pthis, SC_SERVER_DUMMYARG float rate)
{
	gPhysicsManager.SetSimRate(rate);
}

void SC_SERVER_DECL CASEngineFuncs__SetPhysicPlayerConfig(void* pthis, SC_SERVER_DUMMYARG PhysicPlayerConfigs *configs)
{
	gPhysicsManager.SetPhysicPlayerConfig(configs);
}

void SC_SERVER_DECL CASEngineFuncs__EnableCustomStepSound(void* pthis, SC_SERVER_DUMMYARG bool bEnabled)
{
	EnableCustomStepSound(bEnabled);
}

void SC_SERVER_DECL CASEngineFuncs__EnablePhysicWorld(void* pthis, SC_SERVER_DUMMYARG bool bEnabled)
{
	gPhysicsManager.EnablePhysicWorld(bEnabled);
}

edict_t* SC_SERVER_DECL CASEngineFuncs__GetViewEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* pClient)
{
	return GetClientViewEntity(pClient);
}

bool SC_SERVER_DECL CASEngineFuncs__SetViewEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* pClient, edict_t* pViewEnt)
{
	return SetClientViewEntity(pClient, pViewEnt);
}

int SC_SERVER_DECL CASEngineFuncs__GetRunPlayerMovePlayerIndex(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return GetRunPlayerMovePlayerIndex();
}

bool SC_SERVER_DECL CASEntityFuncs__CreateSolidOptimizer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int boneindex, const Vector& halfext, const Vector& halfext2)
{
	return gPhysicsManager.CreateSolidOptimizer(ent, boneindex, halfext, halfext2);
}

//Legacy call for "Fall Guys in Sven Co-op Season 2", call CreatePhysicObject instead in Season 3
bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicBox(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, float mass, float linearfriction, float rollingfriction, float restitution, float ccdradius, float ccdthreshold, bool pushable)
{
	PhysicShapeParams shapeParams;
	shapeParams.type = PhysicShape_Box;
	shapeParams.size = Vector((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f);

	PhysicObjectParams objectParams;
	objectParams.mass = mass;
	objectParams.linearfriction = linearfriction;
	objectParams.rollingfriction = rollingfriction;
	objectParams.restitution = restitution;
	objectParams.ccdradius = ccdradius;
	objectParams.ccdthreshold = ccdthreshold;

	return gPhysicsManager.CreatePhysicObject(ent, &shapeParams, &objectParams);
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicObject(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, PhysicShapeParams *shapeParams, PhysicObjectParams *objectParams)
{
	return gPhysicsManager.CreatePhysicObject(ent, shapeParams, objectParams);
}

bool SC_SERVER_DECL CASEntityFuncs__CreateCompoundPhysicObject(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, CScriptArray *shapeParamArray, PhysicObjectParams *objectParams)
{
	bool bResult = false;

	PhysicShapeParams **pdata = (PhysicShapeParams **)shapeParamArray->data();

	bResult = gPhysicsManager.CreateCompoundPhysicObject(ent, pdata, shapeParamArray->size(), objectParams);
	
	ASEXT_CScriptArray_Release(shapeParamArray);

	return bResult;
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicVehicle(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, PhysicVehicleParams *vehicleParams, CScriptArray *wheelParamArray)
{
	bool bResult = false;

	PhysicWheelParams **pdata = (PhysicWheelParams **)wheelParamArray->data();

	bResult = gPhysicsManager.CreatePhysicVehicle(ent, vehicleParams, pdata, wheelParamArray->size());

	ASEXT_CScriptArray_Release(wheelParamArray);
	
	return bResult;
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicTrigger(void* pthis, SC_SERVER_DUMMYARG edict_t* ent)
{
	return gPhysicsManager.CreatePhysicTrigger(ent);
}

bool SC_SERVER_DECL CASEntityFuncs__CreatePhysicWater(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, float density, float linear_drag, float angular_drag)
{
	return gPhysicsManager.CreatePhysicWater(ent, density, linear_drag, angular_drag);
}

bool SC_SERVER_DECL CASEntityFuncs__SetPhysicObjectTransform(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, const Vector &origin, const Vector &angles)
{
	return gPhysicsManager.SetPhysicObjectTransform(ent, origin, angles);
}

bool SC_SERVER_DECL CASEntityFuncs__SetPhysicObjectFreeze(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, bool freeze)
{
	return gPhysicsManager.SetPhysicObjectFreeze(ent, freeze);
}

bool SC_SERVER_DECL CASEntityFuncs__SetPhysicObjectNoCollision(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, bool no_collision)
{
	return gPhysicsManager.SetPhysicObjectNoCollision(ent, no_collision);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityFollow(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, edict_t* follow, int flags, const Vector &origin_offset, const Vector &angles_offset)
{
	return gPhysicsManager.SetEntityFollow(ent, follow, flags, origin_offset, angles_offset);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityEnvStudioAnim(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int flags, float overrideCurFrame, float overrideMaxFrame, CScriptArray *keyframes)
{
	bool bResult = false;

	EnvStudioKeyframe **pdata = (EnvStudioKeyframe **)keyframes->data();

	bResult = gPhysicsManager.SetEntityEnvStudioAnim(ent, flags, overrideCurFrame, overrideMaxFrame, pdata, keyframes->size());

	ASEXT_CScriptArray_Release(keyframes);

	return bResult;
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityCustomMoveSize(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, const Vector &mins, const Vector &maxs)
{
	return gPhysicsManager.SetEntityCustomMoveSize(ent, mins, maxs);
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

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySemiVisible(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int player_mask)
{
	return gPhysicsManager.SetEntitySemiVisible(ent, player_mask);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySemiClip(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int player_mask)
{
	return gPhysicsManager.SetEntitySemiClip(ent, player_mask);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityPMSemiClip(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int player_mask)
{
	return gPhysicsManager.SetEntityPMSemiClip(ent, player_mask);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySemiClipToPlayer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	if (!(entindex >= 1 && entindex <= gpGlobals->maxClients))
		return false;

	return gPhysicsManager.SetEntitySemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityPMSemiClipToPlayer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	if (!(entindex >= 1 && entindex <= gpGlobals->maxClients))
		return false;

	return gPhysicsManager.SetEntityPMSemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntitySemiClipToPlayer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	if (!(entindex >= 1 && entindex <= gpGlobals->maxClients))
		return false;

	return gPhysicsManager.UnsetEntitySemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntityPMSemiClipToPlayer(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	if (!(entindex >= 1 && entindex <= gpGlobals->maxClients))
		return false;

	return gPhysicsManager.UnsetEntityPMSemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySemiClipToEntityIndex(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	return gPhysicsManager.SetEntitySemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityPMSemiClipToEntityIndex(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	return gPhysicsManager.SetEntityPMSemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntitySemiClipToEntityIndex(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	return gPhysicsManager.UnsetEntitySemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntityPMSemiClipToEntityIndex(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int entindex)
{
	return gPhysicsManager.UnsetEntityPMSemiClipToEntityIndex(ent, entindex);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntitySemiClipToEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, edict_t* targetEntity)
{
	return gPhysicsManager.SetEntitySemiClipToEntity(ent, targetEntity);
}

bool SC_SERVER_DECL CASEntityFuncs__SetEntityPMSemiClipToEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, edict_t* targetEntity)
{
	return gPhysicsManager.SetEntityPMSemiClipToEntity(ent, targetEntity);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntitySemiClipToEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, edict_t* targetEntity)
{
	return gPhysicsManager.UnsetEntitySemiClipToEntity(ent, targetEntity);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntityPMSemiClipToEntity(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, edict_t* targetEntity)
{
	return gPhysicsManager.UnsetEntityPMSemiClipToEntity(ent, targetEntity);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntitySemiClipToAll(void* pthis, SC_SERVER_DUMMYARG edict_t* ent)
{
	return gPhysicsManager.UnsetEntitySemiClipToAll(ent);
}

bool SC_SERVER_DECL CASEntityFuncs__UnsetEntityPMSemiClipToAll(void* pthis, SC_SERVER_DUMMYARG edict_t* ent)
{
	return gPhysicsManager.UnsetEntityPMSemiClipToAll(ent);
}

edict_t *SC_SERVER_DECL CASEntityFuncs__GetCurrentSuperPusher(void* pthis, SC_SERVER_DUMMYARG Vector* vecPushDirection)
{
	return GetCurrentSuperPusher(vecPushDirection);
}

edict_t *SC_SERVER_DECL CASEntityFuncs__GetCurrentPhysicImpactEntity(void* pthis, SC_SERVER_DUMMYARG Vector* vecImpactPoint, Vector* vecImpactDirection, float *flImpactImpulse)
{
	return gPhysicsManager.GetCurrentImpactEntity(vecImpactPoint, vecImpactDirection, flImpactImpulse);
}

bool SC_SERVER_DECL CASEntityFuncs__ApplyPhysicImpulse(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, const Vector& impulse, const Vector& origin)
{
	return gPhysicsManager.ApplyPhysicImpulse(ent, impulse, origin);
}

bool SC_SERVER_DECL CASEntityFuncs__ApplyPhysicForce(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, const Vector& impulse, const Vector& origin)
{
	return gPhysicsManager.ApplyPhysicForce(ent, impulse, origin);
}

bool SC_SERVER_DECL CASEntityFuncs__SetVehicleEngine(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int wheelIndex, bool enableMotor, float targetVelcoity, float maxMotorForce)
{
	return gPhysicsManager.SetVehicleEngine(ent, wheelIndex, enableMotor, targetVelcoity, maxMotorForce);
}

bool SC_SERVER_DECL CASEntityFuncs__SetVehicleSteering(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int wheelIndex, float angularTarget, float targetVelcoity, float maxMotorForce)
{
	return gPhysicsManager.SetVehicleSteering(ent, wheelIndex, angularTarget, targetVelcoity, maxMotorForce);
}

bool SC_SERVER_DECL CASEntityFuncs__GetVehicleWheelRuntimeInfo(void* pthis, SC_SERVER_DUMMYARG edict_t* ent, int wheelIndex, PhysicWheelRuntimeInfo *RuntimeInfo)
{
	return gPhysicsManager.GetVehicleWheelRuntimeInfo(ent, wheelIndex, RuntimeInfo);
}

float SC_SERVER_DECL CASSoundEngine_GetSoundInfo(void* pthis, SC_SERVER_DUMMYARG CString *str, SoundEngine_SoundInfo *SoundInfo)
{
	return SoundEngine_GetSoundInfo(str->c_str(), SoundInfo);
}

void SC_SERVER_DECL CASPlayerMove_GetTextureName(playermove_t *pthis, SC_SERVER_DUMMYARG CString *str)
{
	str->assign(pthis->sztexturename, strlen(pthis->sztexturename));
}

void SC_SERVER_DECL NewCPlayerMove_PlayStepSound(void *pthis, SC_SERVER_DUMMYARG int iType, float flVolume, bool bIsJump)
{
	if (g_bUseCustomStepSound)
	{
		if (g_pfn_build_number() >= 10152)
		{
			playermove_10152_t* playermove = *(playermove_10152_t**)((char*)pthis + 0x460);

			int uiFlags = 0;

			if (ASEXT_CallHook)
			{
				(*ASEXT_CallHook)(g_PlayerMovePlayStepSoundHook, 0, g_engfuncs.pfnPEntityOfEntIndex(g_iRunPlayerMoveIndex)->pvPrivateData, playermove, iType, flVolume, bIsJump, &uiFlags);
			}

			if (uiFlags & 1)
				return;

			int footsteps = playermove->movevars->footsteps;
			playermove->movevars->footsteps = 1;

			g_call_original_CPlayerMove_PlayStepSound(pthis, SC_SERVER_PASS_DUMMYARG iType, flVolume, bIsJump);

			playermove->movevars->footsteps = footsteps;
		}
		else
		{
			playermove_t* playermove = *(playermove_t**)((char*)pthis + 0x460);

			int uiFlags = 0;

			if (ASEXT_CallHook)
			{
				(*ASEXT_CallHook)(g_PlayerMovePlayStepSoundHook, 0, g_engfuncs.pfnPEntityOfEntIndex(g_iRunPlayerMoveIndex)->pvPrivateData, playermove, iType, flVolume, bIsJump, &uiFlags);
			}

			if (uiFlags & 1)
				return;

			int footsteps = playermove->movevars->footsteps;
			playermove->movevars->footsteps = 1;

			g_call_original_CPlayerMove_PlayStepSound(pthis, SC_SERVER_PASS_DUMMYARG iType, flVolume, bIsJump);

			playermove->movevars->footsteps = footsteps;
		}
	}
	else
	{
		g_call_original_CPlayerMove_PlayStepSound(pthis, SC_SERVER_PASS_DUMMYARG iType, flVolume, bIsJump);
	}
}

void NewPM_PlaySoundFX_SERVER(int playerindex, vec3_t *origin, int type, const char *sound, float vol, float att, int flags, int pitch)
{
	if (g_bUseCustomStepSound)
	{
		int uiFlags = 0;
		CString str = {0};
		str.assign(sound, strlen(sound));

		if (ASEXT_CallHook)
		{
			(*ASEXT_CallHook)(g_PlayerMovePlaySoundFXHook, 0, g_engfuncs.pfnPEntityOfEntIndex(g_iRunPlayerMoveIndex)->pvPrivateData, playerindex, origin, type, &str, vol, att, flags, pitch, &uiFlags);
		}

		str.dtor();

		if (uiFlags & 1)
			return;
	}
	g_call_original_PM_PlaySoundFX_SERVER(playerindex, origin, type, sound, vol, att, flags, pitch);
}

hook_t *g_phook_CPlayerMove_PlayStepSound = NULL;
hook_t *g_phook_PM_PlaySoundFX_SERVER = NULL;

void InstallServerHooks()
{
	INSTALL_INLINEHOOK(CPlayerMove_PlayStepSound);
	INSTALL_INLINEHOOK(PM_PlaySoundFX_SERVER);
}

void UninstallServerHooks()
{
	UNINSTALL_HOOK(CPlayerMove_PlayStepSound);
	UNINSTALL_HOOK(PM_PlaySoundFX_SERVER);
}

void RegisterAngelScriptMethods(void)
{
	ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {

		/* SoundEngine_SoundInfo */

		REGISTER_PLAIN_VALUE_OBJECT(SoundEngine_SoundInfo);

		ASEXT_RegisterObjectProperty(pASDoc, "", "SoundEngine_SoundInfo", "int type", offsetof(SoundEngine_SoundInfo, type));
		ASEXT_RegisterObjectProperty(pASDoc, "", "SoundEngine_SoundInfo", "int format", offsetof(SoundEngine_SoundInfo, format));
		ASEXT_RegisterObjectProperty(pASDoc, "", "SoundEngine_SoundInfo", "int channels", offsetof(SoundEngine_SoundInfo, channels));
		ASEXT_RegisterObjectProperty(pASDoc, "", "SoundEngine_SoundInfo", "int bits", offsetof(SoundEngine_SoundInfo, bits));
		ASEXT_RegisterObjectProperty(pASDoc, "", "SoundEngine_SoundInfo", "uint length", offsetof(SoundEngine_SoundInfo, length));

		/* EnvStudioKeyframe */

		REGISTER_PLAIN_VALUE_OBJECT(EnvStudioKeyframe);

		ASEXT_RegisterObjectProperty(pASDoc, "", "EnvStudioKeyframe", "float frame", offsetof(EnvStudioKeyframe, frame));
		ASEXT_RegisterObjectProperty(pASDoc, "", "EnvStudioKeyframe", "float renderamt", offsetof(EnvStudioKeyframe, renderamt));
		ASEXT_RegisterObjectProperty(pASDoc, "", "EnvStudioKeyframe", "float scale", offsetof(EnvStudioKeyframe, scale));

		/* PhysicPlayerConfigs */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicPlayerConfigs);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicPlayerConfigs", "float mass", offsetof(PhysicPlayerConfigs, mass));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicPlayerConfigs", "float density", offsetof(PhysicPlayerConfigs, density));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicPlayerConfigs", "float maxPendingVelocity", offsetof(PhysicPlayerConfigs, maxPendingVelocity));

		/* PhysicShapeParams */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicShapeParams);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "int type", offsetof(PhysicShapeParams, type));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "int direction", offsetof(PhysicShapeParams, direction));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "Vector origin", offsetof(PhysicShapeParams, origin));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "Vector angles", offsetof(PhysicShapeParams, angles));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "Vector size", offsetof(PhysicShapeParams, size));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicShapeParams", "array<float>@ multispheres", offsetof(PhysicShapeParams, multispheres));

		/* PhysicObjectParams */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicObjectParams);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float mass", offsetof(PhysicObjectParams, mass));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float density", offsetof(PhysicObjectParams, density));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float linearfriction", offsetof(PhysicObjectParams, linearfriction));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float rollingfriction", offsetof(PhysicObjectParams, rollingfriction));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float restitution", offsetof(PhysicObjectParams, restitution));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float ccdradius", offsetof(PhysicObjectParams, ccdradius));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float ccdthreshold", offsetof(PhysicObjectParams, ccdthreshold));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "int flags", offsetof(PhysicObjectParams, flags));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "float impactimpulse_threshold", offsetof(PhysicObjectParams, impactimpulse_threshold));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "int clippinghull_shapetype", offsetof(PhysicObjectParams, clippinghull_shapetype));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "int clippinghull_shapedirection", offsetof(PhysicObjectParams, clippinghull_shapedirection));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "Vector clippinghull_size", offsetof(PhysicObjectParams, clippinghull_size));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicObjectParams", "Vector centerofmass", offsetof(PhysicObjectParams, centerofmass));

		/* PhysicWheelParams */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicWheelParams);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "edict_t@ ent", offsetof(PhysicWheelParams, ent));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "Vector connectionPoint", offsetof(PhysicWheelParams, connectionPoint));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "Vector wheelDirection", offsetof(PhysicWheelParams, wheelDirection));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "Vector wheelAxle", offsetof(PhysicWheelParams, wheelAxle));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "float suspensionStiffness", offsetof(PhysicWheelParams, suspensionStiffness));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "float suspensionDamping", offsetof(PhysicWheelParams, suspensionDamping));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "float suspensionLowerLimit", offsetof(PhysicWheelParams, suspensionLowerLimit));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "float suspensionUpperLimit", offsetof(PhysicWheelParams, suspensionUpperLimit));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "float rayCastHeight", offsetof(PhysicWheelParams, rayCastHeight));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "int springIndex", offsetof(PhysicWheelParams, springIndex));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "int engineIndex", offsetof(PhysicWheelParams, engineIndex));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "int steerIndex", offsetof(PhysicWheelParams, steerIndex));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "int flags", offsetof(PhysicWheelParams, flags));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelParams", "int index", offsetof(PhysicWheelParams, index));

		/* PhysicVehicleParams */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicVehicleParams);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicVehicleParams", "int type", offsetof(PhysicVehicleParams, type));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicVehicleParams", "int flags", offsetof(PhysicVehicleParams, flags));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicVehicleParams", "float idleEngineForce", offsetof(PhysicVehicleParams, idleEngineForce));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicVehicleParams", "float idleSteeringForce", offsetof(PhysicVehicleParams, idleSteeringForce));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicVehicleParams", "float idleSteeringSpeed", offsetof(PhysicVehicleParams, idleSteeringSpeed));

		/* PhysicWheelRuntimeInfo */

		REGISTER_PLAIN_VALUE_OBJECT(PhysicWheelRuntimeInfo);

		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "bool hitGround", offsetof(PhysicWheelRuntimeInfo, hitGround));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "Vector hitPointInWorld", offsetof(PhysicWheelRuntimeInfo, hitPointInWorld));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "Vector hitNormalInWorld", offsetof(PhysicWheelRuntimeInfo, hitNormalInWorld));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "float rpm", offsetof(PhysicWheelRuntimeInfo, rpm));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "float waterVolume", offsetof(PhysicWheelRuntimeInfo, waterVolume));
		ASEXT_RegisterObjectProperty(pASDoc, "", "PhysicWheelRuntimeInfo", "float totalVolume", offsetof(PhysicWheelRuntimeInfo, totalVolume));

		/* playermove_t */

		ASEXT_RegisterObjectType(pASDoc, "PlayerMove control struct in engine", "playermove_t", 0, asOBJ_REF | asOBJ_NOCOUNT );
		ASEXT_RegisterObjectProperty(pASDoc, "player index of current player that playing with playermove code", "playermove_t", "int player_index", offsetof(playermove_t, player_index));
		ASEXT_RegisterObjectProperty(pASDoc, "For debugging, are we running physics code on server side?", "playermove_t", "int server", offsetof(playermove_t, server));
		ASEXT_RegisterObjectProperty(pASDoc, "1 == multiplayer server", "playermove_t", "int multiplayer", offsetof(playermove_t, multiplayer));
		ASEXT_RegisterObjectProperty(pASDoc, "realtime on host, for reckoning duck timing", "playermove_t", "float time", offsetof(playermove_t, time));
		ASEXT_RegisterObjectProperty(pASDoc, "Duration of this frame", "playermove_t", "float frametime", offsetof(playermove_t, frametime));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector forward", offsetof(playermove_t, forward));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector right", offsetof(playermove_t, right));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector up", offsetof(playermove_t, up));
		ASEXT_RegisterObjectProperty(pASDoc, "Movement origin.", "playermove_t", "Vector origin", offsetof(playermove_t, origin));
		ASEXT_RegisterObjectProperty(pASDoc, "Movement view angles.", "playermove_t", "Vector angles", offsetof(playermove_t, angles));
		ASEXT_RegisterObjectProperty(pASDoc, "Angles before movement view angles were looked at.", "playermove_t", "Vector oldangles", offsetof(playermove_t, oldangles));
		ASEXT_RegisterObjectProperty(pASDoc, "Current movement direction.", "playermove_t", "Vector velocity", offsetof(playermove_t, velocity));
		ASEXT_RegisterObjectProperty(pASDoc, "For waterjumping, a forced forward velocity so we can fly over lip of ledge.", "playermove_t", "Vector movedir", offsetof(playermove_t, movedir));
		ASEXT_RegisterObjectProperty(pASDoc, "Velocity of the conveyor we are standing, e.g.", "playermove_t", "Vector basevelocity", offsetof(playermove_t, basevelocity));
		ASEXT_RegisterObjectProperty(pASDoc, "Our eye position.", "playermove_t", "Vector view_ofs", offsetof(playermove_t, view_ofs));
		ASEXT_RegisterObjectProperty(pASDoc, "Time we started duck", "playermove_t", "float flDuckTime", offsetof(playermove_t, flDuckTime));
		ASEXT_RegisterObjectProperty(pASDoc, "In process of ducking or ducked already?", "playermove_t", "int bInDuck", offsetof(playermove_t, bInDuck));
		ASEXT_RegisterObjectProperty(pASDoc, "Next time we can play a step sound", "playermove_t", "int flTimeStepSound", offsetof(playermove_t, flTimeStepSound));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int iStepLeft", offsetof(playermove_t, iStepLeft));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float flFallVelocity", offsetof(playermove_t, flFallVelocity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector punchangle", offsetof(playermove_t, punchangle));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float flSwimTime", offsetof(playermove_t, flSwimTime));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float flNextPrimaryAttack", offsetof(playermove_t, flNextPrimaryAttack));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int effects", offsetof(playermove_t, effects));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int flags", offsetof(playermove_t, flags));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int usehull", offsetof(playermove_t, usehull));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float gravity", offsetof(playermove_t, gravity));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float friction", offsetof(playermove_t, friction));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int oldbuttons", offsetof(playermove_t, oldbuttons));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float waterjumptime", offsetof(playermove_t, waterjumptime));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int dead", offsetof(playermove_t, dead));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int deadflag", offsetof(playermove_t, deadflag));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int spectator", offsetof(playermove_t, spectator));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int movetype", offsetof(playermove_t, movetype));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int onground", offsetof(playermove_t, onground));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int waterlevel", offsetof(playermove_t, waterlevel));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int watertype", offsetof(playermove_t, watertype));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int oldwaterlevel", offsetof(playermove_t, oldwaterlevel));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "char chtexturetype", offsetof(playermove_t, chtexturetype));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float maxspeed", offsetof(playermove_t, maxspeed));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float clientmaxspeed", offsetof(playermove_t, clientmaxspeed));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int iuser1", offsetof(playermove_t, iuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int iuser2", offsetof(playermove_t, iuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int iuser3", offsetof(playermove_t, iuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "int iuser4", offsetof(playermove_t, iuser4));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float fuser1", offsetof(playermove_t, fuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float fuser2", offsetof(playermove_t, fuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float fuser3", offsetof(playermove_t, fuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "float fuser4", offsetof(playermove_t, fuser4));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector vuser1", offsetof(playermove_t, vuser1));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector vuser2", offsetof(playermove_t, vuser2));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector vuser3", offsetof(playermove_t, vuser3));
		ASEXT_RegisterObjectProperty(pASDoc, "", "playermove_t", "Vector vuser4", offsetof(playermove_t, vuser4)); 
		ASEXT_RegisterObjectMethod(pASDoc,
			"get texture name", "playermove_t", "void GetTextureName(string& out texture_name)",
			(void *)CASPlayerMove_GetTextureName, 3);

		/* entity_state_t */

		ASEXT_RegisterObjectType    (pASDoc, "Entity states transmit to client", "entity_state_t", 0, asOBJ_REF | asOBJ_NOCOUNT);
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
			"Disable stepsound temporarily until level changes", "CEngineFuncs", "void EnableCustomStepSound(bool bEnabled)",
			(void *)CASEngineFuncs__EnableCustomStepSound, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable physic world temporarily until level changes", "CEngineFuncs", "void EnablePhysicWorld(bool bEnabled)",
			(void *)CASEngineFuncs__EnablePhysicWorld, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set simulation rate of Bullet Engine world", "CEngineFuncs", "void SetPhysicSimRate(float rate)",
			(void *)CASEngineFuncs__SetPhysicSimRate, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get index of player that is currently running PlayerMove code", "CEngineFuncs", "int GetRunPlayerMovePlayerIndex()",
			(void *)CASEngineFuncs__GetRunPlayerMovePlayerIndex, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get client's view entity", "CEngineFuncs", "edict_t@ GetViewEntity(edict_t@ pClient)",
			(void *)CASEngineFuncs__GetViewEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set client's view entity", "CEngineFuncs", "bool SetViewEntity(edict_t@ pClient, edict_t@ pViewEnt)",
			(void *)CASEngineFuncs__SetViewEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic box for entity", "CEntityFuncs", "bool CreateSolidOptimizer(edict_t@ ent, int boneindex, const Vector& in halfextent, const Vector& in halfextent2)",
			(void *)CASEntityFuncs__CreateSolidOptimizer, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic object for entity", "CEntityFuncs", "bool CreatePhysicBox(edict_t@ ent, float mass, float linearfriction, float rollingfriction, float restitution, float ccdradius, float ccdthreshold, bool pushable )",
			(void *)CASEntityFuncs__CreatePhysicBox, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic object for entity", "CEntityFuncs", "bool CreatePhysicObject(edict_t@ ent, const PhysicShapeParams& in shapeParams, const PhysicObjectParams& in objectParams )",
			(void *)CASEntityFuncs__CreatePhysicObject, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic compound shape for entity", "CEntityFuncs", "bool CreateCompoundPhysicObject(edict_t@ ent, const array<PhysicShapeParams>@ shapeParamArray, const PhysicObjectParams& in objectParams)",
			(void *)CASEntityFuncs__CreateCompoundPhysicObject, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create vehicle manager for physic entity", "CEntityFuncs", "bool CreatePhysicVehicle(edict_t@ ent, const PhysicVehicleParams& in vehicleParams, const array<PhysicWheelParams>& in wheels)",
			(void *)CASEntityFuncs__CreatePhysicVehicle, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic trigger that fires pfnTouch when colliding with physic object", "CEntityFuncs", "bool CreatePhysicTrigger(edict_t@ ent)",
			(void *)CASEntityFuncs__CreatePhysicTrigger, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Create physic water", "CEntityFuncs", "bool CreatePhysicWater(edict_t@ ent, float density, float linear_drag, float angular_drag)",
			(void *)CASEntityFuncs__CreatePhysicWater, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set physic object's transform", "CEntityFuncs", "bool SetPhysicObjectTransform(edict_t@ ent, const Vector& in origin, const Vector& in angles )",
			(void *)CASEntityFuncs__SetPhysicObjectTransform, 3);
		
		ASEXT_RegisterObjectMethod(pASDoc,
			"Set physic object's activation state to freeze or unfreeze", "CEntityFuncs", "bool SetPhysicObjectFreeze(edict_t@ ent, bool freeze )",
			(void *)CASEntityFuncs__SetPhysicObjectFreeze, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set physic object to be in no_collision state", "CEntityFuncs", "bool SetPhysicObjectNoCollision(edict_t@ ent, bool no_collision )",
			(void *)CASEntityFuncs__SetPhysicObjectNoCollision, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable or disable Super-Pusher for brush entity", "CEntityFuncs", "bool SetEntitySuperPusher(edict_t@ ent, bool enable)",
			(void *)CASEntityFuncs__SetEntitySuperPusher, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable Level-of-Detail for entity", "CEntityFuncs", "bool SetEntityLevelOfDetail(edict_t@ ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)",
			(void *)CASEntityFuncs__SetEntityLevelOfDetail, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable Semi-Visible for entity", "CEntityFuncs", "bool SetEntityPartialViewer(edict_t@ ent, int player_mask)",
			(void *)CASEntityFuncs__SetEntitySemiVisible, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable Semi-Visible for entity", "CEntityFuncs", "bool SetEntitySemiVisible(edict_t@ ent, int player_mask)",
			(void *)CASEntityFuncs__SetEntitySemiVisible, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable SemiClip for entity", "CEntityFuncs", "bool SetEntitySemiClip(edict_t@ ent, int player_mask)",
			(void *)CASEntityFuncs__SetEntitySemiClip, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable SemiClip for entity", "CEntityFuncs", "bool SetEntitySemiClipToPlayer(edict_t@ ent, int entindex)",
			(void *)CASEntityFuncs__SetEntitySemiClipToPlayer, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Disable SemiClip for entity", "CEntityFuncs", "bool UnsetEntitySemiClipToPlayer(edict_t@ ent, int entindex)",
			(void *)CASEntityFuncs__UnsetEntitySemiClipToPlayer, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable SemiClip for entity", "CEntityFuncs", "bool SetEntitySemiClipToEntityIndex(edict_t@ ent, int entindex)",
			(void*)CASEntityFuncs__SetEntitySemiClipToEntityIndex, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Disable SemiClip for entity", "CEntityFuncs", "bool UnsetEntitySemiClipToEntityIndex(edict_t@ ent, int entindex)",
			(void*)CASEntityFuncs__UnsetEntitySemiClipToEntityIndex, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable SemiClip for entity", "CEntityFuncs", "bool SetEntitySemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)",
			(void *)CASEntityFuncs__SetEntitySemiClipToEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Disable SemiClip for entity", "CEntityFuncs", "bool UnsetEntitySemiClipToEntity(edict_t@ ent, edict_t@ targetEntity)",
			(void *)CASEntityFuncs__UnsetEntitySemiClipToEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Disable SemiClip for entity", "CEntityFuncs", "bool UnsetEntitySemiClipToAll(edict_t@ ent)",
			(void*)CASEntityFuncs__UnsetEntitySemiClipToAll, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable follow for the entity", "CEntityFuncs", "bool SetEntityFollow(edict_t@ ent, edict_t@ follow, int flags, const Vector& in origin_offset, const Vector& in angles_offset )",
			(void *)CASEntityFuncs__SetEntityFollow, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable follow for the entity", "CEntityFuncs", "bool SetEntityEnvStudioAnim(edict_t@ ent, int flags, float overrideCurFrame, float overrideMaxFrame, array<EnvStudioKeyframe>@ keyframes )",
			(void *)CASEntityFuncs__SetEntityEnvStudioAnim, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Enable follow for the entity", "CEntityFuncs", "bool SetEntityCustomMoveSize(edict_t@ ent, const Vector& in mins, const Vector& in maxs )",
			(void *)CASEntityFuncs__SetEntityCustomMoveSize, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get current working Super-Pusher entity and push direction, return valid edict only in pfnTouch callback", "CEntityFuncs", "edict_t@ GetCurrentSuperPusher(Vector& out vecPushDirection)",
			(void *)CASEntityFuncs__GetCurrentSuperPusher, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get current impact information from Bullet Engine, return valid edict only in pfnTouch callback", "CEntityFuncs", "edict_t@ GetCurrentPhysicImpactEntity(Vector& out vecImpactPoint, Vector& out vecImpactDirection, float& out flImpactImpulse)",
			(void *)CASEntityFuncs__GetCurrentPhysicImpactEntity, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Apply impulse on physic object", "CEntityFuncs", "bool ApplyPhysicImpulse(edict_t@ ent, const Vector& in impulse, const Vector& in origin)",
			(void *)CASEntityFuncs__ApplyPhysicImpulse, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Apply force on physic object", "CEntityFuncs", "bool ApplyPhysicForce(edict_t@ ent, const Vector& in impulse, const Vector& in origin)",
			(void *)CASEntityFuncs__ApplyPhysicForce, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set engine wheel's motor and servo for the vehicle", "CEntityFuncs", "bool SetVehicleEngine(edict_t@ ent, int wheelIndex, bool enableMotor, float targetVelcoity, float maxMotorForce)",
			(void *)CASEntityFuncs__SetVehicleEngine, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set steering wheel's motor and servo for the vehicle", "CEntityFuncs", "bool SetVehicleSteering(edict_t@ ent, int wheelIndex, float angularTarget, float targetVelcoity, float maxMotorForce)",
			(void *)CASEntityFuncs__SetVehicleSteering, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Set steering wheel's motor and servo for the vehicle", "CEntityFuncs", "bool GetVehicleWheelRuntimeInfo(edict_t@ ent, int wheelIndex, PhysicWheelRuntimeInfo & out RuntimeInfo)",
			(void *)CASEntityFuncs__GetVehicleWheelRuntimeInfo, 3);

		ASEXT_RegisterObjectMethod(pASDoc,
			"Get sound's playback length, format, type, channels, bits", "CSoundEngine", "bool GetSoundInfo(const string& in szSoundName, SoundEngine_SoundInfo & out SoundInfo)",
			(void *)CASSoundEngine_GetSoundInfo, 3);

	});
}