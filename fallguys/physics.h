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

enum FallGuysCollisionFilterGroups
{
	PlayerFilter = 0x40,
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

	virtual bool IsGhost() const
	{
		return false;
	}

	virtual bool IsSolidOptimizerGhost() const
	{
		return false;
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{

	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{

	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world)
	{

	}

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world)
	{

	}

	virtual bool SetAbsBox(edict_t *ent)
	{
		return false;
	}

	virtual int PM_CheckStuck(int hitent, physent_t *blocker, vec3_t *impactvelocity)
	{
		return 0;
	}

	CGameObject *GetGameObject() const
	{
		return m_gameobj;
	}

	void SetGameObject(CGameObject *gameobj)
	{
		m_gameobj = gameobj;
	}

private:
	CGameObject *m_gameobj;
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
		if (m_ghostobj)
		{
			world->addCollisionObject(m_ghostobj, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::DefaultFilter);

			(*numDynamicObjects)++;
		}
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
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
			delete m_rigbody;
			m_rigbody = NULL;
		}
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		if (m_rigbody)
		{
			world->addRigidBody(m_rigbody, m_group, m_mask);
		}
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
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
	CDynamicObject(CGameObject *obj, int group, int mask, float mass, bool pushable) : CCollisionPhysicObject(obj, group, mask)
	{
		m_mass = mass;
		m_pushable = pushable;
	}

	virtual bool IsDynamic() const
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

	virtual bool SetAbsBox(edict_t *ent);

	virtual int PM_CheckStuck(int hitent, physent_t *blocker, vec3_t *impactvelocity);

protected:
	float m_mass;
	bool m_pushable;
};

ATTRIBUTE_ALIGNED16(class)
CPlayerObject : public CCollisionPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPlayerObject(CGameObject *obj, int group, int mask, float mass) : CCollisionPhysicObject(obj, group, mask)
	{
		m_mass = mass;
	}

	virtual bool IsPlayer() const
	{
		return true;
	}

	virtual void StartFrame(btDiscreteDynamicsWorld* world);

	virtual void StartFrame_Post(btDiscreteDynamicsWorld* world);

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
	}

	~CGameObject()
	{
		m_entindex = -1;
		m_ent = NULL;
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			delete m_physics[i];
		}
		m_physics.clear();
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
	//cpp14 not support
#if 0
	void RemovePhysicObject(CPhysicObject *physobj, btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		physobj->RemoveFromPhysicWorld(world, numDynamicObjects);

		m_physics.erase(std::find(m_physics.begin(), m_physics.end(), physobj));
	}
#endif
	void RemoveAllPhysicObjects(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			m_physics[i]->RemoveFromPhysicWorld(world, numDynamicObjects);
			delete m_physics[i];
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

	int PM_CheckStuck(int hitent, physent_t *blocker, vec3_t *impactvelocity)
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			int result = m_physics[i]->PM_CheckStuck(hitent, blocker, impactvelocity);
			if (result != 0)
				return result;
		}

		return 0;
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

	void StartFrame(btDiscreteDynamicsWorld* world);

	void StartFrame_Post(btDiscreteDynamicsWorld* world);

	void ApplyLevelOfDetail(float distance, int *body, int *modelindex, float *scale);

	bool AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player);

private:
	edict_t *m_ent;
	int m_entindex;

	std::vector<CPhysicObject *> m_physics;

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

	std::vector<CCachedBoneSolidOptimizer> m_solid_optimizer;
};

class CPhysicsManager
{
public:
	CPhysicsManager();
	void Init(void);

	//Server spawn or shutdown
	void NewMap(edict_t *ent);
	void Shutdown(void);

	//Mesh Builder
	void BuildSurfaceDisplayList(model_t* mod, msurface_t* fa, std::vector<glpoly_t*> &glpolys);
	void GenerateBrushIndiceArray(std::vector<glpoly_t*> &glpolys);
	void GenerateWorldVerticeArray(std::vector<glpoly_t*> &glpolys);
	void GenerateIndexedArrayRecursiveWorldNode(mnode_t* node, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrushface(brushface_t* brushface, indexarray_t* indexarray);
	void GenerateIndexedArrayForSurface(msurface_t* psurf, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrush(model_t* mod, vertexarray_t* vertexarray, indexarray_t* indexarray);
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

	CDynamicObject* CreateDynamicObject(CGameObject *obj, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable,
		btCollisionShape* collisionShape, const btVector3& localInertia);
	CStaticObject* CreateStaticObject(CGameObject *obj, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic);
	CPlayerObject* CreatePlayerObject(CGameObject *obj, float mass, btCollisionShape* collisionShape, const btVector3& localInertia);

	void AddGameObject(CGameObject *obj);

	bool CreateBrushModel(edict_t* ent);
	bool CreatePhysicSphere(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable);
	bool CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable);
	bool CreatePlayerBox(edict_t* ent);
	bool CreateSolidOptimizer(edict_t* ent, int boneindex, const Vector &mins, const Vector &maxs);
	//bool CreateSolidOptimizer(edict_t* ent, int boneindex, float radius);
	bool SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3);
	bool SetEntityPartialViewer(edict_t* ent, int partial_viewer_mask);
	bool SetEntitySuperPusher(edict_t* ent, bool enable);

	void EntityStartFrame(void);
	void EntityStartFrame_Post(void);
	void FreeEntityPrivateData(edict_t* ent);
	bool SetAbsBox(edict_t *pent);
	bool AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player);
	
	void PM_StartSemiClip(int playerIndex, edict_t *ent);
	void PM_EndSemiClip(int playerIndex, edict_t *ent);
	bool PM_ShouldCollide(int info);
	void PM_StartMove();
	void PM_EndMove(); 
	qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity);
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

	int m_solidPlayerMask;
};

extern CPhysicsManager gPhysicsManager;