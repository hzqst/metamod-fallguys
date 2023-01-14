#pragma once

#include <unordered_map>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h>
#include <studio.h>
#include <com_model.h>
#include <pmtrace.h>
#include <pm_defs.h>

const int PhysicShape_Box = 1;
const int PhysicShape_Sphere = 2;
const int PhysicShape_Capsule = 3;
const int PhysicShape_Cylinder = 4;
const int PhysicShape_MultiSphere = 5;

const int PhysicObject_HasClippingHull = 1;
const int PhysicObject_HasImpactImpulse = 2;

const int PhysicWheel_Front = 1;
const int PhysicWheel_Engine = 2;
const int PhysicWheel_Brake = 4;
const int PhysicWheel_Steering = 8;

class PhysicShapeParams
{
public:
	PhysicShapeParams();

	int type;
	int direction;
	Vector origin;
	Vector angles;
	Vector size;
	CScriptArray *multispheres;
};

void PhysicShapeParams_ctor(PhysicShapeParams *pthis);
void PhysicShapeParams_copyctor(PhysicShapeParams *a1, PhysicShapeParams *a2);
void PhysicShapeParams_dtor(PhysicShapeParams *pthis);
PhysicShapeParams * SC_SERVER_DECL PhysicShapeParams_opassign(PhysicShapeParams *a1, SC_SERVER_DUMMYARG PhysicShapeParams *a2);

class PhysicObjectParams
{
public:
	PhysicObjectParams();

	float mass;
	float linearfriction;
	float rollingfriction;
	float restitution;
	float ccdradius;
	float ccdthreshold;
	int flags;
	float impactimpulse_threshold;
	Vector clippinghull_size;
};

void PhysicObjectParams_ctor(PhysicObjectParams *pthis);
void PhysicObjectParams_copyctor(PhysicObjectParams *a1, PhysicObjectParams *a2);
void PhysicObjectParams_dtor(PhysicObjectParams *pthis);
PhysicObjectParams * SC_SERVER_DECL PhysicObjectParams_opassign(PhysicObjectParams *a1, SC_SERVER_DUMMYARG PhysicObjectParams *a2);

class PhysicWheelParams
{
public:
	PhysicWheelParams();

	edict_t *ent;
	Vector connectionPoint;
	Vector wheelDirection;
	Vector wheelAxle;
	float suspensionRestLength;
	float wheelRadius;
	int flags;
	float suspensionStiffness;
	float wheelsDampingRelaxation;
	float wheelsDampingCompression;
	float frictionSlip;
	float rollInfluence;
	float maxSuspensionTravelCm;
	float maxSuspensionForce;
};

void PhysicWheelParams_ctor(PhysicWheelParams *pthis);
void PhysicWheelParams_copyctor(PhysicWheelParams *a1, PhysicWheelParams *a2);
void PhysicWheelParams_dtor(PhysicWheelParams *pthis);
PhysicWheelParams * SC_SERVER_DECL PhysicWheelParams_opassign(PhysicWheelParams *a1, SC_SERVER_DUMMYARG PhysicWheelParams *a2);

class PhysicVehicleParams
{
public:
	PhysicVehicleParams();

	float suspensionStiffness;
	float suspensionCompression;
	float suspensionDamping;
	float maxSuspensionTravelCm;
	float frictionSlip;
	float maxSuspensionForce;
	int flags;
};

void PhysicVehicleParams_ctor(PhysicVehicleParams *pthis);
void PhysicVehicleParams_copyctor(PhysicVehicleParams *a1, PhysicVehicleParams *a2);
void PhysicVehicleParams_dtor(PhysicVehicleParams *pthis);
PhysicVehicleParams * SC_SERVER_DECL PhysicVehicleParams_opassign(PhysicVehicleParams *a1, SC_SERVER_DUMMYARG PhysicVehicleParams *a2);


class PhysicWheelClientInfo
{
public:
	PhysicWheelClientInfo(edict_t *ent, int flags) : m_ent(ent), m_flags(flags)
	{

	}

	virtual ~PhysicWheelClientInfo()
	{

	}

	edict_t *GetEdict() const
	{
		return m_ent;
	}

	int GetFlags() const
	{
		return m_flags;
	}

	bool IsFrontWheel() const
	{
		return GetFlags() & PhysicWheel_Front;
	}

	bool IsEngineWheel() const
	{
		return GetFlags() & PhysicWheel_Engine;
	}

	bool IsBrakeWheel() const
	{
		return GetFlags() & PhysicWheel_Brake;
	}

	bool IsSteeringWheel() const
	{
		return GetFlags() & PhysicWheel_Steering;
	}

private:

	edict_t *m_ent;
	int m_flags;
};

class PhysicRaycastVehicle : public btRaycastVehicle
{
public:
	PhysicRaycastVehicle(const btVehicleTuning& tuning, btRigidBody* chassis, btVehicleRaycaster* raycaster) : btRaycastVehicle(tuning, chassis, raycaster)
	{

	}
	~PhysicRaycastVehicle()
	{
		for (int i = 0; i < m_wheelInfo.size(); ++i)
		{
			delete m_wheelInfo[i].m_clientInfo;
			m_wheelInfo[i].m_clientInfo = NULL;
		}
	}
};

enum FallGuysCollisionFilterGroups
{
	PlayerFilter = 0x40,
	ClippingHullFilter = 0x80,
};

typedef struct brushvertex_s
{
	vec3_t	pos;
}brushvertex_t;

typedef struct brushface_s
{
	//int index;
	int start_vertex;
	int num_vertexes;
}brushface_t;

typedef struct vertexarray_s
{
	vertexarray_s()
	{
	}
	std::vector<brushvertex_t> vVertexBuffer;
	std::vector<brushface_t> vFaceBuffer;
}vertexarray_t;

typedef struct indexarray_s
{
	indexarray_s()
	{
	}
	std::vector<int> vIndiceBuffer;
}indexarray_t;

class CGameObject;

ATTRIBUTE_ALIGNED16(class)
CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicObject(CGameObject *obj) : m_gameobj(obj)
	{
		
	}

	virtual ~CPhysicObject()
	{
		m_gameobj = NULL;

		for (auto action : m_actions)
		{
			delete action;
		}
		m_actions.clear();
	}

	virtual bool IsDynamic() const
	{
		return false;
	}

	virtual bool IsKinematic() const
	{
		return false;
	}

	virtual bool IsStatic() const
	{
		return false;
	}

	virtual bool IsPlayer() const
	{
		return false;
	}

	virtual bool IsClippingHull() const
	{
		return false;
	}

	virtual bool IsGhost() const
	{
		return false;
	}

	virtual bool IsSolidOptimizerGhost() const
	{
		return false;
	}

	virtual bool UseEdictSolid() const
	{
		return false;
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{

	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		for (auto action : m_actions)
		{
			world->removeAction(action);
		}
	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world)
	{

	}

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world)
	{

	}

	virtual void EndFrame(btDiscreteDynamicsWorld* world)
	{

	}

	virtual bool SetAbsBox(edict_t *ent)
	{
		return false;
	}

	virtual void DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent)
	{

	}

	CGameObject *GetGameObject() const
	{
		return m_gameobj;
	}

	void SetGameObject(CGameObject *gameobj)
	{
		m_gameobj = gameobj;
	}
	
	void AddAction(btDiscreteDynamicsWorld* world, btActionInterface *action)
	{
		m_actions.emplace_back(action);
		world->addAction(action);
	}

private:
	CGameObject *m_gameobj;
	std::vector<btActionInterface *> m_actions;
};

ATTRIBUTE_ALIGNED16(class)
CGhostPhysicObject : public CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CGhostPhysicObject(CGameObject *obj) : CPhysicObject(obj)
	{
		m_ghostobj = NULL;
	}
	~CGhostPhysicObject()
	{
		//Should be removed from world before free
		if (m_ghostobj)
		{
			delete m_ghostobj;
			m_ghostobj = NULL;
		}
	}

	virtual bool IsGhost() const
	{
		return true;
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::AddToPhysicWorld(world, numDynamicObjects);
		if (m_ghostobj)
		{
			world->addCollisionObject(m_ghostobj, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::DefaultFilter);

			(*numDynamicObjects)++;
		}
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::RemoveFromPhysicWorld(world, numDynamicObjects);

		if (m_ghostobj)
		{
			world->removeCollisionObject(m_ghostobj);

			(*numDynamicObjects)--;
		}
	}

	void SetGhostObject(btPairCachingGhostObject* ghostobj)
	{
		m_ghostobj = ghostobj;
		m_ghostobj->setUserPointer(this);
	}

	btPairCachingGhostObject* GetGhostObject()
	{
		return m_ghostobj;
	}

private:
	btPairCachingGhostObject* m_ghostobj;
};

ATTRIBUTE_ALIGNED16(class)
CSolidOptimizerGhostPhysicObject : public CGhostPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CSolidOptimizerGhostPhysicObject(CGameObject *obj, int boneindex, int type) : CGhostPhysicObject(obj), m_boneindex(boneindex), m_type(type)
	{
		m_cached_sequence = -1;
		m_cached_frame = 0;
	}
	~CSolidOptimizerGhostPhysicObject()
	{

	}

	int GetOptimizerType() const
	{
		return m_type;
	}

	virtual bool IsSolidOptimizerGhost() const
	{
		return true;
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::AddToPhysicWorld(world, numDynamicObjects);

		if (GetGhostObject())
		{
			world->addCollisionObject(GetGhostObject(), btBroadphaseProxy::SensorTrigger, FallGuysCollisionFilterGroups::PlayerFilter);

			(*numDynamicObjects)++;
		}
	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world);

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world);

private:
	int m_type;
	int m_boneindex;
	vec3_t m_cached_boneorigin;
	vec3_t m_cached_boneangles;
	vec3_t m_cached_origin;
	int m_cached_sequence;
	float m_cached_frame;
};

ATTRIBUTE_ALIGNED16(class)
CCollisionPhysicObject : public CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CCollisionPhysicObject(CGameObject *obj, int group, int mask) : CPhysicObject(obj)
	{
		m_rigbody = NULL;
		m_group = group;
		m_mask = mask;
	}

	~CCollisionPhysicObject()
	{
		//Should be removed from world before free
		if (m_rigbody)
		{
			if (m_rigbody->getCollisionShape())
			{
				if (m_rigbody->getCollisionShape()->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE &&
					m_rigbody->getCollisionShape()->getUserPointer())
				{
					delete (btTriangleIndexVertexArray *)m_rigbody->getCollisionShape()->getUserPointer();
				}

				delete m_rigbody->getCollisionShape();
			}
			if (m_rigbody->getMotionState())
			{
				delete m_rigbody->getMotionState();
			}
			delete m_rigbody;
			m_rigbody = NULL;
		}
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::AddToPhysicWorld(world, numDynamicObjects);

		if (m_rigbody)
		{
			world->addRigidBody(m_rigbody, m_group, m_mask);
		}
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::RemoveFromPhysicWorld(world, numDynamicObjects);

		if (m_rigbody)
		{
			world->removeRigidBody(m_rigbody);
		}
	}

	void SetRigidBody(btRigidBody* rigbody)
	{
		m_rigbody = rigbody;
		m_rigbody->setUserPointer(this);
	}

	btRigidBody* GetRigidBody()
	{
		return m_rigbody;
	}

private:
	btRigidBody* m_rigbody;
	int m_group, m_mask;
};

ATTRIBUTE_ALIGNED16(class)
CStaticObject : public CCollisionPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CStaticObject(CGameObject *obj, int group, int mask, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic) : CCollisionPhysicObject(obj, group, mask)
	{
		m_vertexarray = vertexarray;
		m_indexarray = indexarray;
		m_kinematic = kinematic;
	}
	~CStaticObject()
	{
		
	}

	virtual bool IsKinematic() const
	{
		return m_kinematic;
	}

	virtual bool IsStatic() const
	{
		return !m_kinematic;
	}

	virtual bool UseEdictSolid() const
	{
		return IsKinematic();
	}

protected:
	vertexarray_t* m_vertexarray;
	indexarray_t* m_indexarray;
	bool m_kinematic;
};

ATTRIBUTE_ALIGNED16(class)
CDynamicObject : public CCollisionPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CDynamicObject(CGameObject *obj, int group, int mask) : CCollisionPhysicObject(obj, group, mask)
	{
		m_bEnableImpactImpulse = false;
		m_flImpactTime = 0;
		m_flImpactImpulse = 0;
		m_flImpactImpulseThreshold = 0;
		m_ImpactPoint = g_vecZero;
		m_ImpactDirection = g_vecZero;
		m_ImpactEntity = NULL;
		m_vehicle = NULL;
		m_flEngineForce = 0;
		m_flBreakForce = 0;
		m_flSteering = 0;
	}

	virtual bool IsDynamic() const
	{
		return true;
	}

	virtual bool UseEdictSolid() const
	{
		return true;
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CCollisionPhysicObject::AddToPhysicWorld(world, numDynamicObjects);

		(*numDynamicObjects) ++ ;
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CCollisionPhysicObject::RemoveFromPhysicWorld(world, numDynamicObjects);

		(*numDynamicObjects) --;
	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world);

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world);

	virtual void EndFrame(btDiscreteDynamicsWorld* world);

	virtual bool SetAbsBox(edict_t *ent);

	virtual void DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent);

	void EnableImpactImpulse(bool bEnabled, float flThreshold)
	{
		m_bEnableImpactImpulse = bEnabled;
		m_flImpactImpulseThreshold = flThreshold;
	}

	void SetVehicleAction(PhysicRaycastVehicle *vehicle)
	{
		m_vehicle = vehicle;
	}
	
	void SetVehicleEngineBrakeSteering(float engineForce, float brakeForce, float steering)
	{
		m_flEngineForce = engineForce;
		m_flBreakForce = brakeForce;
		m_flSteering = steering;
	}

protected:
	bool m_bEnableImpactImpulse;
	float m_flImpactImpulseThreshold;
	float m_flImpactImpulse;
	float m_flImpactTime;
	vec3_t m_ImpactPoint;
	vec3_t m_ImpactDirection;
	edict_t *m_ImpactEntity;

	PhysicRaycastVehicle *m_vehicle;
	float m_flEngineForce;
	float m_flBreakForce;
	float m_flSteering;
};

ATTRIBUTE_ALIGNED16(class)
CPlayerObject : public CCollisionPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPlayerObject(CGameObject *obj, int group, int mask, float mass, bool duck) : CCollisionPhysicObject(obj, group, mask)
	{
		m_duck = duck;

		m_mass = mass;
		m_PendingVelocity = g_vecZero;
		m_TickCount = 0;
		m_BlockingTickCount = 0;

		m_flImpactImpulse = 0;
		m_ImpactPoint = g_vecZero;
		m_ImpactDirection = g_vecZero;
		m_ImpactEntity = NULL;
	}

	virtual bool IsPlayer() const
	{
		return true;
	}

	virtual bool UseEdictSolid() const
	{
		return true;
	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world);

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world);

	virtual void EndFrame(btDiscreteDynamicsWorld* world);

	virtual void DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent);

	bool IsDuck() const
	{
		return m_duck;
	}

protected:
	bool m_duck;
	float m_mass;
	vec3_t m_PendingVelocity;
	uint32_t m_TickCount;
	uint32_t m_BlockingTickCount;

	float m_flImpactImpulse;
	vec3_t m_ImpactPoint;
	vec3_t m_ImpactDirection;
	edict_t *m_ImpactEntity;
};

ATTRIBUTE_ALIGNED16(class)
CClippingHullObject : public CCollisionPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CClippingHullObject(CGameObject *obj, int group, int mask, float mass) : CCollisionPhysicObject(obj, group, mask)
	{
		m_mass = mass;
	}

	virtual bool IsClippingHull() const
	{
		return true;
	}

	virtual bool UseEdictSolid() const
	{
		return true;
	}

protected:
	float m_mass;
};

ATTRIBUTE_ALIGNED16(class)
EntityMotionState : public btMotionState
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);

	EntityMotionState(CPhysicObject* obj) : btMotionState()
	{
		m_object = obj;
		m_worldTransform.setIdentity();
		m_worldTransformInitialized = false;
	}

	CPhysicObject* GetPhysicObject() const
	{
		return m_object;
	}

private:
	CPhysicObject* m_object;

	//Avoid gimbal lock
	mutable btTransform m_worldTransform;
	mutable bool m_worldTransformInitialized;
};

class CCachedBoneSolidOptimizer
{
public :
	CCachedBoneSolidOptimizer(int bone, float radius)
	{
		m_boneindex = bone;
		m_radius = radius;
		m_cached_sequence = -1;
		m_cached_frame = 0;
	}

	void StartFrame(CGameObject *obj);

	int m_boneindex;
	float m_radius;
	vec3_t m_cached_boneorigin;
	vec3_t m_cached_boneangles;
	vec3_t m_cached_origin;
	int m_cached_sequence;
	float m_cached_frame;
};

ATTRIBUTE_ALIGNED16(class)
CGameObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	CGameObject(edict_t *ent, int entindex) : m_ent(ent), m_entindex(entindex)
	{
		m_lod_flags = 0;
		m_lod_body0 = 0;
		m_lod_body1 = 0;
		m_lod_body2 = 0;
		m_lod_body3 = 0;
		m_lod_scale0 = 0;
		m_lod_scale1 = 0;
		m_lod_scale2 = 0;
		m_lod_scale3 = 0;
		m_lod_distance1 = 0;
		m_lod_distance2 = 0;
		m_lod_distance3 = 0;
		m_partial_viewer_mask = 0;
		m_super_pusher = false;
		m_semi_clip_mask = 0;
		m_original_solid = 0;
		m_follow_flags = 0;
		m_follow_ent = NULL;
		m_follow_angles_offet = g_vecZero;
		m_follow_origin_offet = g_vecZero;
	}

	virtual ~CGameObject()
	{
		m_entindex = -1;
		m_ent = NULL;

		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			delete m_physics[i];
		}
		m_physics.clear();

		for (size_t i = 0; i < m_constraints.size(); ++i)
		{
			delete m_constraints[i];
		}
		m_constraints.clear();
	}

	bool HasPhysObjects() const
	{
		return !m_physics.empty();
	}

	void AddSolidOptimizer(int bone, float radius)
	{
		m_solid_optimizer.emplace_back(bone, radius);
	}

	void AddPhysicObject(CPhysicObject *physobj, btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		physobj->AddToPhysicWorld(world, numDynamicObjects);

		m_physics.emplace_back(physobj);
	}

	void AddConstraint(btTypedConstraint *constraint, btDiscreteDynamicsWorld* world, bool disableCollisionsBetweenLinkedBodies)
	{
		world->addConstraint(constraint, disableCollisionsBetweenLinkedBodies);

		m_constraints.emplace_back(constraint);
	}

	void RemoveAllConstraints(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		for (size_t i = 0; i < m_constraints.size(); ++i)
		{
			world->removeConstraint(m_constraints[i]);
			delete m_constraints[i];
		}

		m_constraints.clear();
	}

	void RemoveAllPhysicObjects(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			auto physobj = m_physics[i];
			if (physobj)
			{
				physobj->RemoveFromPhysicWorld(world, numDynamicObjects);
				delete physobj;
			}
		}

		m_physics.clear();
	}

	bool SetAbsBox(edict_t *ent)
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			if (m_physics[i]->SetAbsBox(ent))
				return true;
		}

		return false;
	}

	bool IsSuperPusherEnabled() const
	{
		return m_super_pusher;
	}

	void SetSuperPusherEnabled(bool enabled)
	{
		m_super_pusher = enabled;
	}

	bool IsSolidOptimizerEnabled() const
	{
		return m_solid_optimizer.size() && !(m_ent->v.effects & EF_NODRAW);
	}

	void SetEntityFollow(edict_t* follow, int flags, const Vector &origin_offset, const Vector &angles_offset)
	{
		if (flags)
		{
			m_follow_flags = flags;
			m_follow_ent = follow;
			m_follow_origin_offet = origin_offset;
			m_follow_angles_offet = angles_offset;
		}
		else
		{
			m_follow_flags = 0;
			m_follow_ent = NULL;
			m_follow_origin_offet = g_vecZero;
			m_follow_angles_offet = g_vecZero;
		}
	}

	int GetEntIndex()
	{
		return m_entindex;
	}

	edict_t *GetEdict()
	{
		return m_ent;
	}

	int GetLevelOfDetailFlags() const
	{
		return m_lod_flags;
	}

	void SetLevelOfDetail(int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)
	{
		m_lod_flags = flags;
		m_lod_body0 = body_0;
		m_lod_body1 = body_1;
		m_lod_body2 = body_2;
		m_lod_body3 = body_3;
		m_lod_scale0 = scale_0;
		m_lod_scale1 = scale_1;
		m_lod_scale2 = scale_2;
		m_lod_scale3 = scale_3;
		m_lod_distance1 = distance_1;
		m_lod_distance2 = distance_2;
		m_lod_distance3 = distance_3;
	}

	void SetPartialViewer(int viewer_mask)
	{
		m_partial_viewer_mask = viewer_mask;
	}

	int GetPartialViewerMask() const
	{
		return m_partial_viewer_mask;
	}

	void SetSemiClipMask(int clip_mask)
	{
		m_semi_clip_mask = clip_mask;
	}

	void RemoveSemiClipMask(int clip_mask)
	{
		m_semi_clip_mask &= ~clip_mask;
	}

	int GetSemiClipMask() const
	{
		return m_semi_clip_mask;
	}

	void SetOriginalSolid(int original_solid)
	{
		m_original_solid = original_solid;
	}

	int GetOriginalSolid() const
	{
		return m_original_solid;
	}

	CPhysicObject *GetPhysicObjectByIndex(int index) const
	{
		return m_physics.at(index);
	}

	size_t GetNumPhysicObject() const
	{
		return m_physics.size();
	}

	void StartFrame(btDiscreteDynamicsWorld* world);

	void StartFrame_Post(btDiscreteDynamicsWorld* world);

	void EndFrame(btDiscreteDynamicsWorld* world);

	void ApplyLevelOfDetail(float distance, int *body, int *modelindex, float *scale);

	bool AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player);

private:
	edict_t *m_ent;
	int m_entindex;

	std::vector<CPhysicObject *> m_physics;
	std::vector<btTypedConstraint *> m_constraints;

	int m_lod_flags;

	int m_lod_body0;
	int m_lod_body1;
	int m_lod_body2;
	int m_lod_body3;

	float m_lod_scale0;
	float m_lod_scale1;
	float m_lod_scale2;
	float m_lod_scale3;

	float m_lod_distance1;
	float m_lod_distance2;
	float m_lod_distance3;

	bool m_super_pusher;

	int m_partial_viewer_mask;

	int m_semi_clip_mask;

	int m_original_solid;

	int m_follow_flags;

	edict_t* m_follow_ent;

	vec3_t m_follow_origin_offet;

	vec3_t m_follow_angles_offet;

	std::vector<CCachedBoneSolidOptimizer> m_solid_optimizer;
};

class CPhysicsManager
{
public:
	CPhysicsManager();
	void Init(void);

	//Server spawn or shutdown
	void PreSpawn(edict_t *ent);
	void PostSpawn(edict_t *ent);
	void Shutdown(void);

	//Mesh Builder
	void BuildSurfaceDisplayList(model_t* mod, msurface_t* fa, std::vector<glpoly_t*> &glpolys);
	void GenerateBrushIndiceArray(std::vector<glpoly_t*> &glpolys);
	void GenerateWorldVerticeArray(std::vector<glpoly_t*> &glpolys);
	void GenerateIndexedArrayRecursiveWorldNode(mnode_t* node, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrushface(brushface_t* brushface, indexarray_t* indexarray);
	void GenerateIndexedArrayForSurface(msurface_t* psurf, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrush(model_t* mod, vertexarray_t* vertexarray, indexarray_t* indexarray);
	
	void SetSimRate(float rate);
	void SetGravity(float velocity);
	void StepSimulation(double framerate);

	int GetSolidPlayerMask();
	int GetNumDynamicBodies();
	CGameObject* GetGameObject(int entindex);
	CGameObject* GetGameObject(edict_t* ent);
	void RemoveGameObject(int entindex);
	void RemoveAllGameBodies();

	bool IsEntitySuperPusher(edict_t* ent);
	bool ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin);
	bool SetVehicleEngineBrakeSteering(edict_t* ent, float engineForce, float brakeForce, float steering);

	CDynamicObject* CreateDynamicObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold);
	CStaticObject* CreateStaticObject(CGameObject *obj, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic);
	CPlayerObject* CreatePlayerObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, float mass, bool duck);
	CClippingHullObject* CreateClippingHullObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, float mass);

	void AddGameObject(CGameObject *obj);

	bool CreateBrushModel(edict_t* ent);
	bool CreatePhysicObject(edict_t* ent, PhysicShapeParams *shapeParams, PhysicObjectParams *objectParams);
	bool CreateCompoundPhysicObject(edict_t* ent, PhysicShapeParams *shapeParamArray, size_t numShapeParam, PhysicObjectParams *objectParams);
	btCollisionShape *CreateCollisionShapeFromParams(CGameObject *obj, PhysicShapeParams *shapeParams);
	bool CreatePhysicObjectPost(edict_t *ent, CGameObject *obj, btCollisionShape *shape, PhysicObjectParams *objectParams);
	bool CreatePlayerBox(edict_t* ent);
	bool CreateSolidOptimizer(edict_t* ent, int boneindex, const Vector &mins, const Vector &maxs);
	bool CreatePhysicVehicle(edict_t* ent, PhysicWheelParams **wheelParamArray, size_t numWheelParam, PhysicVehicleParams *vehicleParams);
	bool SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3);
	bool SetEntityPartialViewer(edict_t* ent, int partial_viewer_mask);
	bool SetEntitySuperPusher(edict_t* ent, bool enable);
	bool SetEntityFollow(edict_t* ent, edict_t* follow, int flags, const Vector &origin_offset, const Vector &angles_offset);

	void EntityStartFrame(void);
	void EntityStartFrame_Post(void);
	void EntityEndFrame(void);
	void FreeEntityPrivateData(edict_t* ent);
	bool SetAbsBox(edict_t *pent);
	bool AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player);
	
	bool PM_ShouldCollide(int info);
	void PM_StartMove();
	void PM_EndMove(); 
	qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity);
	int PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce);
	int PM_FlyMove();

	edict_t* GetCurrentImpactEntity(Vector *vecImpactPoint, Vector *vecImpactDirection, float *flImpactImpulse);
	void SetCurrentImpactEntity(edict_t* pImpactEntity, const Vector &vecImpactPoint, const Vector &vecImpactDirection, float flImpactImpulse);

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btGhostPairCallback *m_ghostPairCallback;
	btOverlapFilterCallback *m_overlapFilterCallback;

	std::vector<CGameObject*> m_gameObjects;
	int m_maxIndexGameObject;
	int m_numDynamicObjects;
	std::vector<indexarray_t*> m_brushIndexArray;
	vertexarray_t* m_worldVertexArray;
	float m_gravity;
	float m_simrate;

	int m_solidPlayerMask;

	float m_CurrentImpactImpulse;
	Vector m_CurrentImpactPoint;
	Vector m_CurrentImpactDirection;
	edict_t *m_CurrentImpactEntity;
};

extern CPhysicsManager gPhysicsManager;