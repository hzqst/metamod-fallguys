#pragma once

#include <unordered_map>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <studio.h>
#include <com_model.h>

ATTRIBUTE_ALIGNED16(class)
CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicBody()
	{
		m_entindex = -1;
		m_rigbody = NULL;
	}
	virtual ~CPhysicBody()
	{
		delete m_rigbody;
		m_entindex = -1;
		m_rigbody = NULL;
	}

	virtual bool IsDynamic() const = 0;

	virtual bool IsKinematic() const = 0;

	int m_entindex;
	btRigidBody* m_rigbody;
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
CStaticBody : public CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CStaticBody() : CPhysicBody()
	{
		m_vertexarray = NULL;
		m_indexarray = NULL;
		m_kinematic = false;
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

	vertexarray_t* m_vertexarray;
	indexarray_t* m_indexarray;
	bool m_kinematic;
};

ATTRIBUTE_ALIGNED16(class)
CDynamicBody : public CPhysicBody
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CDynamicBody() : CPhysicBody()
	{
		m_mass = 0;
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
	float m_mass;
};

ATTRIBUTE_ALIGNED16(class)
EntityMotionState : public btMotionState
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	EntityMotionState(edict_t* ent, CPhysicBody *body);
	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);

	edict_t* pent;
	CPhysicBody* pbody;
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

class CPhysicsManager
{
public:
	CPhysicsManager();
	void Init(void);
	void NewMap(void);
	void DebugDraw(void);
	void BuildSurfaceDisplayList(model_t* mod, msurface_t* fa);
	void GenerateBrushIndiceArray(void);
	void GenerateWorldVerticeArray(void);
	void GenerateIndexedArrayRecursiveWorldNode(mnode_t* node, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrushface(brushface_t* brushface, indexarray_t* indexarray);
	void GenerateIndexedArrayForSurface(msurface_t* psurf, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void GenerateIndexedArrayForBrush(model_t* mod, vertexarray_t* vertexarray, indexarray_t* indexarray);
	void SetGravity(float velocity);
	void StepSimulation(double framerate);
	void ReloadConfig(void);
	
	void RemoveAllStatics();
	CStaticBody* CreateStaticBody(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic);
	CDynamicBody* CreateDynamicBody(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia);
	void CreatePhysBox(edict_t* ent);
	void CreateBrushModel(edict_t* ent);
	void CreateWater(edict_t* ent);
	void PreTickCallback(btScalar timeStep);
private:

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	CPhysicsDebugDraw* m_debugDraw;
	std::unordered_map<int, CPhysicBody*> m_bodyMap;
	std::vector<indexarray_t*> m_brushIndexArray;
	vertexarray_t* m_worldVertexArray;
	float m_gravity;
	std::vector<glpoly_t*> m_allocPoly;
};

extern CPhysicsManager gPhysicsManager;