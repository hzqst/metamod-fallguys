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

const int BMASK_WORLD = 1;
const int BMASK_BRUSH = 2;
const int BMASK_PLAYER = 4;
const int BMASK_DYNAMIC = 8;
const int BMASK_DYNAMIC_PUSHABLE = 16;
const int BMASK_ALL = (BMASK_WORLD | BMASK_BRUSH | BMASK_PLAYER | BMASK_DYNAMIC | BMASK_DYNAMIC_PUSHABLE);
const int BMASK_ALL_NONPLAYER = (BMASK_WORLD | BMASK_BRUSH | BMASK_DYNAMIC | BMASK_DYNAMIC_PUSHABLE);

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
		bIsDynamic = false;
	}
	std::vector<brushvertex_t> vVertexBuffer;
	std::vector<brushface_t> vFaceBuffer;
	bool bIsDynamic;
}vertexarray_t;

typedef struct indexarray_s
{
	indexarray_s()
	{
		bIsDynamic = false;
	}
	std::vector<int> vIndiceBuffer;
	bool bIsDynamic;
}indexarray_t;

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
	}

	virtual ~CGameObject()
	{
		m_entindex = -1;
		m_ent = NULL;
	}

	virtual bool IsPhysic() const
	{
		return false;
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

	virtual bool IsSuperPusher() const
	{
		return false;
	}

	virtual void SetSuperPusher(bool superpusher)
	{

	}

	virtual void StartFrame()
	{
		
	}

	virtual void StartFrame_Post()
	{
	
	}

	virtual void AddToPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{

	}
	
	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		
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

	void ApplyLevelOfDetail(float distance, int *body, int *modelindex, float *scale)
	{
		if (m_lod_distance3 > 0 && distance > m_lod_distance3)
		{
			if ((m_lod_flags & LOD_BODY) && m_lod_body3 >= 0)
				*body = m_lod_body3;
			else if((m_lod_flags & LOD_MODELINDEX) && m_lod_body3 >= 0)
				*modelindex = m_lod_body3;
			
			if (m_lod_flags & LOD_SCALE_INTERP)
			{
				*scale = m_lod_scale3;
			}
			else if (m_lod_flags & LOD_SCALE)
			{
				*scale = m_lod_scale3;
			}
		}
		else if (m_lod_distance2 > 0 && distance > m_lod_distance2)
		{
			if ((m_lod_flags & LOD_BODY) && m_lod_body2 >= 0)
				*body = m_lod_body2;
			else if ((m_lod_flags & LOD_MODELINDEX) && m_lod_body2 >= 0)
				*modelindex = m_lod_body2;

			if (m_lod_flags & LOD_SCALE_INTERP)
			{
				*scale = m_lod_scale2 + (m_lod_scale3 - m_lod_scale2) * (distance - m_lod_distance2) / (m_lod_distance3 - m_lod_distance2);
			}
			else if (m_lod_flags & LOD_SCALE)
			{
				*scale = m_lod_scale2;
			}
		}
		else if (m_lod_distance1 > 0 && distance > m_lod_distance1)
		{
			if ((m_lod_flags & LOD_BODY) && m_lod_body1 >= 0)
				*body = m_lod_body1;
			else if ((m_lod_flags & LOD_MODELINDEX) && m_lod_body1 >= 0)
				*modelindex = m_lod_body1;

			if (m_lod_flags & LOD_SCALE_INTERP)
			{
				*scale = m_lod_scale1 + (m_lod_scale2 - m_lod_scale1) * (distance - m_lod_distance1) / (m_lod_distance2 - m_lod_distance1);
			}
			else if (m_lod_flags & LOD_SCALE)
			{
				*scale = m_lod_scale1;
			}
		}
		else
		{
			if ((m_lod_flags & LOD_BODY) && m_lod_body0 >= 0)
				*body = m_lod_body0;
			else if ((m_lod_flags & LOD_MODELINDEX) && m_lod_body0 >= 0)
				*modelindex = m_lod_body0;

			if (m_lod_flags & LOD_SCALE_INTERP)
			{
				*scale = m_lod_scale0 + (m_lod_scale1 - m_lod_scale0) * (distance - 0) / (m_lod_distance1 - 0);
			}
			else if (m_lod_flags & LOD_SCALE)
			{
				*scale = m_lod_scale0;
			}
		}
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

protected:
	edict_t *m_ent;
	int m_entindex;

private:
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

private:
	int m_partial_viewer_mask;
};

ATTRIBUTE_ALIGNED16(class)
CPhysicObject : public CGameObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicObject(edict_t *ent, int entindex, int group, int mask) : CGameObject(ent, entindex)
	{
		m_rigbody = NULL;
		m_group = group;
		m_mask = mask;
	}
	~CPhysicObject()
	{
		//Should be removed from world before free
		if (m_rigbody)
		{
			delete m_rigbody;
			m_rigbody = NULL;
		}
	}

	virtual bool IsPhysic() const
	{
		return true;
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
		rigbody->setUserPointer(this);
		m_rigbody = rigbody;
	}

	btRigidBody* GetRigidBody()
	{
		return m_rigbody;
	}

protected:
	btRigidBody* m_rigbody;
	int m_group, m_mask;
};

ATTRIBUTE_ALIGNED16(class)
CStaticObject : public CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CStaticObject(edict_t *ent, int entindex, int group, int mask, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic) : CPhysicObject(ent, entindex, group, mask)
	{
		m_vertexarray = vertexarray;
		m_indexarray = indexarray;
		m_kinematic = kinematic;
		m_superpusher = false;
	}
	~CStaticObject()
	{
		if (m_vertexarray->bIsDynamic)
		{
			delete m_vertexarray;
			m_vertexarray = NULL;
		}

		if (m_indexarray->bIsDynamic)
		{
			delete m_indexarray;
			m_indexarray = NULL;
		}
	}

	virtual bool IsKinematic() const
	{
		return m_kinematic;
	}

	virtual bool IsStatic() const
	{
		return !m_kinematic;
	}

	virtual bool IsSuperPusher() const
	{
		return m_superpusher;
	}

	virtual void SetSuperPusher(bool superpusher)
	{
		m_superpusher = superpusher;
	}

protected:
	vertexarray_t* m_vertexarray;
	indexarray_t* m_indexarray;
	bool m_kinematic;
	bool m_superpusher;
};

ATTRIBUTE_ALIGNED16(class)
CDynamicObject : public CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CDynamicObject(edict_t *ent, int entindex, int group, int mask, float mass, bool pushable) : CPhysicObject(ent, entindex, group, mask)
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
		CPhysicObject::AddToPhysicWorld(world, numDynamicObjects);

		*numDynamicObjects ++ ;
	}

	virtual void RemoveFromPhysicWorld(btDiscreteDynamicsWorld* world, int *numDynamicObjects)
	{
		CPhysicObject::RemoveFromPhysicWorld(world, numDynamicObjects);

		*numDynamicObjects --;
	}

	virtual void StartFrame();

	virtual void StartFrame_Post();

protected:
	float m_mass;
	bool m_pushable;
};

ATTRIBUTE_ALIGNED16(class)
CPlayerObject : public CPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPlayerObject(edict_t *ent, int entindex, int group, int mask, float mass) : CPhysicObject(ent, entindex, group, mask)
	{
		m_mass = mass;
	}

	virtual bool IsPlayer() const
	{
		return true;
	}

	virtual void StartFrame();

	virtual void StartFrame_Post();

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

	int GetNumDynamicBodies();
	CGameObject* GetGameObject(int entindex);
	CGameObject* GetGameObject(edict_t* ent);
	void RemoveGameObject(int entindex);
	void RemoveAllGameBodies();

	bool IsEntitySuperPusher(edict_t* ent);
	bool IsEntityDynamicPhysicObject(edict_t* ent);
	bool ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin);

	CDynamicObject* CreateDynamicObject(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable,
		btCollisionShape* collisionShape, const btVector3& localInertia);
	CStaticObject* CreateStaticObject(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic);
	CPlayerObject* CreatePlayerObject(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia);
	
	void AddGameObject(CGameObject *obj);

	bool CreateBrushModel(edict_t* ent);
	bool CreatePhysicSphere(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable);
	bool CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable);
	bool CreatePlayerBox(edict_t* ent);
	bool SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3);
	bool SetEntityPartialViewer(edict_t* ent, int partial_viewer_mask);
	bool SetEntitySuperPusher(edict_t* ent, bool enable);

	void EntityStartFrame();
	void EntityStartFrame_Post();
	void FreeEntityPrivateData(edict_t* ent);
	bool SetAbsBox(edict_t *pent);
	bool AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player);

	void PM_StartMove();
	void PM_EndMove(); 
	qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity);
private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;

	std::vector<CGameObject*> m_gameObjects;
	int m_maxIndexGameObject;
	int m_numDynamicObjects;
	std::vector<indexarray_t*> m_brushIndexArray;
	vertexarray_t* m_worldVertexArray;
	float m_gravity;
};

extern CPhysicsManager gPhysicsManager;