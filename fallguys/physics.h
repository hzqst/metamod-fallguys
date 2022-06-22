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
CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicBody()
	{
		m_rigbody = NULL;
		m_entindex = -1;
		m_pent = NULL;
	}
	virtual ~CPhysicBody()
	{
		if (m_rigbody)
		{
			//wtf, why it's still here?
			//delete m_rigbody;
			m_rigbody = NULL;
		}
		m_entindex = -1;
		m_pent = NULL;
	}

	virtual bool IsDynamic() const = 0;

	virtual bool IsKinematic() const = 0;

	virtual bool IsPlayer() const = 0;

	virtual bool IsSuperPusher() const = 0;

	virtual void GetVelocity(vec3_t &vel) const = 0;

	virtual void StartFrame() = 0;

	virtual void StartFrame_Post() = 0;

	btRigidBody* m_rigbody;
	int m_entindex;
	edict_t *m_pent;
};

ATTRIBUTE_ALIGNED16(class)
CStaticBody : public CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CStaticBody() : CPhysicBody()
	{
		m_vertexarray = NULL;
		m_indexarray = NULL;
		m_kinematic = false;
		m_superpusher = false;
	}
	virtual ~CStaticBody()
	{
		CPhysicBody::~CPhysicBody();

		if (m_vertexarray->bIsDynamic)
		{
			delete m_vertexarray;
		}

		if (m_indexarray->bIsDynamic)
		{
			delete m_indexarray;
		}
		m_vertexarray = NULL;
		m_indexarray = NULL;
	}
	virtual bool IsDynamic() const {
		return false;
	}

	virtual bool IsKinematic() const {
		return m_kinematic;
	}

	virtual bool IsPlayer() const {
		return false;
	}
	virtual bool IsSuperPusher() const
	{
		return m_superpusher;
	}

	virtual void GetVelocity(vec3_t &vel) const
	{
	}

	virtual void StartFrame()
	{

	}

	virtual void StartFrame_Post()
	{

	}

	vertexarray_t* m_vertexarray;
	indexarray_t* m_indexarray;
	bool m_kinematic;
	bool m_superpusher;
};

ATTRIBUTE_ALIGNED16(class)
CDynamicBody : public CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CDynamicBody() : CPhysicBody()
	{
		m_mass = 0;
		m_pushable = false;
		m_superpusher = false;
		m_ignore_player_mask = 0;
	}	
	virtual ~CDynamicBody()
	{
		CPhysicBody::~CPhysicBody();
	}
	virtual bool IsDynamic() const {
		return true;
	}

	virtual bool IsKinematic() const {
		return false;
	}

	virtual bool IsPlayer() const {
		return false;
	}
	virtual bool IsSuperPusher() const
	{
		return m_superpusher;
	}

	virtual void GetVelocity(vec3_t &vel) const;

	virtual void StartFrame();

	virtual void StartFrame_Post();

	float m_mass;
	bool m_pushable;
	bool m_superpusher;

	unsigned long m_ignore_player_mask;
};

ATTRIBUTE_ALIGNED16(class)
CPlayerBody : public CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPlayerBody() : CPhysicBody()
	{

	}
	virtual ~CPlayerBody()
	{
		CPhysicBody::~CPhysicBody();
	}
	virtual bool IsDynamic() const {
		return false;
	}

	virtual bool IsKinematic() const {
		return true;
	}

	virtual bool IsPlayer() const {
		return true;
	}
	virtual bool IsSuperPusher() const
	{
		return false;
	}

	virtual void GetVelocity(vec3_t &vel) const
	{
	}

	virtual void StartFrame();

	virtual void StartFrame_Post();

};

ATTRIBUTE_ALIGNED16(class)
EntityMotionState : public btMotionState
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);

	EntityMotionState(CPhysicBody* body) : btMotionState()
	{
		m_body = body;
		m_worldTransform.setIdentity();
		m_worldTransformInitialized = false;
	}

	CPhysicBody* GetPhysicBody() const
	{
		return m_body;
	}

private:
	CPhysicBody* m_body;

	//Avoid gimbal lock
	mutable btTransform m_worldTransform;
	mutable bool m_worldTransformInitialized;
};

ATTRIBUTE_ALIGNED16(class)
CPhysicsDebugDraw : public btIDebugDraw
{
	int m_debugMode;

	DefaultColors m_ourColors;

public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	CPhysicsDebugDraw() : m_debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits)
	{

	}

	virtual ~CPhysicsDebugDraw()
	{
	}
	virtual DefaultColors getDefaultColors() const
	{
		return m_ourColors;
	}
	///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
	virtual void setDefaultColors(const DefaultColors& colors)
	{
		m_ourColors = colors;
	}

	virtual void drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1);

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		drawLine(PointOnB, PointOnB + normalOnB * distance, color);
		btVector3 ncolor(0, 0, 0);
		drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
	}

	virtual void reportErrorWarning(const char* warningString)
	{
	}

	virtual void draw3dText(const btVector3& location, const char* textString)
	{
	}

	virtual void setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	virtual int getDebugMode() const
	{
		return m_debugMode;
	}
};

class CPlayerDynamicPush
{
public:
	CPlayerDynamicPush()
	{
		pusher = NULL;
		velocity = g_vecZero;
	}

	edict_t *pusher;
	vec3_t velocity;
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
	bool IsRunningPlayerMove();
	CPhysicBody* GetPhysicBody(int entindex); 
	CPhysicBody* GetPhysicBody(edict_t* ent);
	void RemovePhysicBody(int entindex);
	void RemoveAllPhysicBodies();

	bool IsSuperPusher(edict_t* ent);
	bool IsDynamicPhysicObject(edict_t* ent);
	bool ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin);

	//Physic Body
	CDynamicBody* CreateDynamicBody(edict_t* ent, float mass, float friction, float rollingfriction, float ccdRadius, float ccdThreshold, bool pushable, 
		btCollisionShape* collisionShape, const btVector3& localInertia);
	CStaticBody* CreateStaticBody(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic);
	bool CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingfriction, float ccdRadius, float ccdThreshold, bool pushable);
	CPlayerBody* CreatePlayerBody(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia);

	CStaticBody* CreateBrushModel(edict_t* ent);
	bool CreatePlayerBox(edict_t* ent);
	bool CreateSuperPusher(edict_t* ent);

	void PreTickCallback(btScalar timeStep);

	void EntityStartFrame();
	void EntityStartFrame_Post();
	void FreeEntityPrivateData(edict_t* ent);
	bool SetAbsBox(edict_t *pent);

	void PM_PrepareContext(playermove_t *pm);
	void PM_DestroyContext(playermove_t *pm); 
	void SV_PrepareContext();
	void SV_DestroyContext();
	void BoxTrace(const vec3_t start_, const vec3_t end_, const vec3_t angles_, const vec3_t mins_, const vec3_t maxs_, trace_t* results);
private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	CPhysicsDebugDraw* m_debugDraw;
	std::vector<CPhysicBody*> m_physBodies;
	int m_maxIndexPhysBody;
	int m_numDynamicBody;
	std::vector<CPhysicBody*> m_removeBodies;
	std::vector<indexarray_t*> m_brushIndexArray;
#if 0
	int *m_ChangeFlagsModelPointers[MAX_PHYSENTS];
	int m_ChangeFlagsModelOriginalValue[MAX_PHYSENTS];
	int m_ChangeFlagsModelCount;

	int *m_ChangeSolidPointers[512];
	int m_ChangeSolidOriginalValue[512];
	int m_ChangeSolidCount;
#endif
	CPlayerDynamicPush m_PlayerDynamicPush[33];

	vertexarray_t* m_worldVertexArray;
	float m_gravity;
	bool m_is_running_playermove;
};

extern CPhysicsManager gPhysicsManager;