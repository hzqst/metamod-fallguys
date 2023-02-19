#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <algorithm>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"

#include "physics.h"

extern edict_t* r_worldentity;
extern model_t* r_worldmodel;

//EnvStudioKeyframe

void EnvStudioKeyframe_ctor(EnvStudioKeyframe *pthis)
{
	pthis->frame = 0;
	pthis->renderamt = 255;
	pthis->scale = 1;
}

EnvStudioKeyframe::EnvStudioKeyframe()
{
	EnvStudioKeyframe_ctor(this);
}

void EnvStudioKeyframe_copyctor(EnvStudioKeyframe *a1, EnvStudioKeyframe *a2)
{
	a1->frame = a2->frame;
	a1->renderamt = a2->renderamt;
	a1->scale = a2->scale;
}

EnvStudioKeyframe * SC_SERVER_DECL EnvStudioKeyframe_opassign(EnvStudioKeyframe *a1, SC_SERVER_DUMMYARG EnvStudioKeyframe *a2)
{
	EnvStudioKeyframe_copyctor(a1, a2);

	return a1;
}

void EnvStudioKeyframe_dtor(EnvStudioKeyframe *pthis)
{

}

//PhysicPlayerConfigs

void PhysicPlayerConfigs_ctor(PhysicPlayerConfigs *pthis)
{
	pthis->mass = 20;
	pthis->maxPendingVelocity = 1000;
}

void PhysicPlayerConfigs_copyctor(PhysicPlayerConfigs *a1, PhysicPlayerConfigs *a2)
{
	a1->mass = a2->mass;
	a1->maxPendingVelocity = a2->maxPendingVelocity;
}

PhysicPlayerConfigs * SC_SERVER_DECL PhysicPlayerConfigs_opassign(PhysicPlayerConfigs *a1, SC_SERVER_DUMMYARG PhysicPlayerConfigs *a2)
{
	PhysicPlayerConfigs_copyctor(a1, a2);

	return a1;
}

void PhysicPlayerConfigs_dtor(PhysicPlayerConfigs *pthis)
{

}

void PhysicShapeParams_ctor(PhysicShapeParams *pthis)
{
	pthis->type = 0;
	pthis->direction = 0;
	pthis->origin = g_vecZero;
	pthis->angles = g_vecZero;
	pthis->size = g_vecZero;
	pthis->multispheres = NULL;
}

void PhysicShapeParams_copyctor(PhysicShapeParams *a1, PhysicShapeParams *a2)
{
	a1->type = a2->type;
	a1->direction = a2->direction;
	a1->origin = a2->origin;
	a1->angles = a2->angles;
	a1->size = a2->size;
	a1->multispheres = a2->multispheres;
}

PhysicShapeParams * SC_SERVER_DECL PhysicShapeParams_opassign(PhysicShapeParams *a1, SC_SERVER_DUMMYARG PhysicShapeParams *a2)
{
	PhysicShapeParams_copyctor(a1, a2);

	return a1;
}

void PhysicShapeParams_dtor(PhysicShapeParams *pthis)
{

}

void PhysicObjectParams_ctor(PhysicObjectParams *pthis)
{
	pthis->mass = 1;
	pthis->linearfriction = 1;
	pthis->rollingfriction = 1;
	pthis->restitution = 0;
	pthis->ccdradius = 0;
	pthis->ccdthreshold = 0;
	pthis->flags = 0;
	pthis->impactimpulse_threshold = 0;
	pthis->clippinghull_shapetype = PhysicShape_Box;
	pthis->clippinghull_shapedirection = PhysicShapeDirection_Z;
	pthis->clippinghull_size = g_vecZero;
}

void PhysicObjectParams_copyctor(PhysicObjectParams *a1, PhysicObjectParams *a2)
{
	a1->mass = a2->mass;
	a1->linearfriction = a2->linearfriction;
	a1->rollingfriction = a2->rollingfriction;
	a1->restitution = a2->restitution;
	a1->ccdradius = a2->ccdradius;
	a1->ccdthreshold = a2->ccdthreshold;
	a1->flags = a2->flags;
	a1->impactimpulse_threshold = a2->impactimpulse_threshold;
	a1->clippinghull_shapetype = a2->clippinghull_shapetype;
	a1->clippinghull_shapedirection = a2->clippinghull_shapedirection;
	a1->clippinghull_size = a2->clippinghull_size;
}

PhysicObjectParams * SC_SERVER_DECL PhysicObjectParams_opassign(PhysicObjectParams *a1, SC_SERVER_DUMMYARG PhysicObjectParams *a2)
{
	PhysicObjectParams_copyctor(a1, a2);

	return a1;
}

void PhysicObjectParams_dtor(PhysicObjectParams *pthis)
{

}

void PhysicWheelParams_ctor(PhysicWheelParams *pthis)
{
	pthis->ent = NULL;
	pthis->connectionPoint = g_vecZero;
	pthis->wheelDirection = g_vecZero;
	pthis->wheelAxle = g_vecZero;
	pthis->suspensionStiffness = 0;
	pthis->suspensionDamping = 0;
	pthis->flags = 0;
	pthis->index = 0;
}

void PhysicWheelParams_copyctor(PhysicWheelParams *a1, PhysicWheelParams *a2)
{
	a1->ent = a2->ent;
	a1->connectionPoint = a2->connectionPoint;
	a1->wheelDirection = a2->wheelDirection;
	a1->wheelAxle = a2->wheelAxle;
	a1->suspensionStiffness = a2->suspensionStiffness;
	a1->suspensionDamping = a2->suspensionDamping;
	a1->flags = a2->flags;
	a1->index = a2->index;
}

PhysicWheelParams * SC_SERVER_DECL PhysicWheelParams_opassign(PhysicWheelParams *a1, SC_SERVER_DUMMYARG PhysicWheelParams *a2)
{
	PhysicWheelParams_copyctor(a1, a2);

	return a1;
}

void PhysicWheelParams_dtor(PhysicWheelParams *pthis)
{

}

void PhysicVehicleParams_ctor(PhysicVehicleParams *pthis)
{
	pthis->suspensionStiffness = 5.88f;
	pthis->suspensionCompression = 0.83f;
	pthis->suspensionDamping = 0.88f;
	pthis->maxSuspensionTravelCm = 500;
	pthis->frictionSlip = 10.5f;
	pthis->maxSuspensionForce = 6000;
	pthis->flags = 0;
}

void PhysicVehicleParams_copyctor(PhysicVehicleParams *a1, PhysicVehicleParams *a2)
{
	a1->suspensionStiffness = a2->suspensionStiffness;
	a1->suspensionCompression = a2->suspensionCompression;
	a1->suspensionDamping = a2->suspensionDamping;
	a1->maxSuspensionTravelCm = a2->maxSuspensionTravelCm;
	a1->frictionSlip = a2->frictionSlip;
	a1->maxSuspensionForce = a2->maxSuspensionForce;
	a1->flags = a2->flags;
}

PhysicVehicleParams * SC_SERVER_DECL PhysicVehicleParams_opassign(PhysicVehicleParams *a1, SC_SERVER_DUMMYARG PhysicVehicleParams *a2)
{
	PhysicVehicleParams_copyctor(a1, a2);

	return a1;
}

void PhysicVehicleParams_dtor(PhysicVehicleParams *pthis)
{

}

PhysicPlayerConfigs::PhysicPlayerConfigs()
{
	PhysicPlayerConfigs_ctor(this);
}

PhysicShapeParams::PhysicShapeParams()
{
	PhysicShapeParams_ctor(this);
}

PhysicObjectParams::PhysicObjectParams()
{
	PhysicObjectParams_ctor(this);
}

PhysicWheelParams::PhysicWheelParams()
{
	PhysicWheelParams_ctor(this);
}

PhysicVehicleParams::PhysicVehicleParams()
{
	PhysicVehicleParams_ctor(this);
}

CPhysicsManager gPhysicsManager;

CPhysicsManager::CPhysicsManager()
{
	m_collisionConfiguration = NULL;
	m_dispatcher = NULL;
	m_overlappingPairCache = NULL;
	m_solver = NULL;
	m_dynamicsWorld = NULL;
	m_ghostPairCallback = NULL;
	m_overlapFilterCallback = NULL;

	m_worldVertexArray = NULL;
	m_gravity = 0;
	m_numDynamicObjects = 0;
	m_maxIndexGameObject = 0;

	m_solidPlayerMask = 0;
	m_simrate = 1 / 60.0f;
	m_playerMass = 20;
	m_playerMaxPendingVelocity = 1000;

	m_CurrentImpactImpulse = 0;
	m_CurrentImpactPoint = g_vecZero;
	m_CurrentImpactDirection = g_vecZero;
	m_CurrentImpactEntity = NULL;
}

void CPhysicsManager::GenerateBrushIndiceArray(std::vector<glpoly_t*> &glpolys)
{
	int maxNum = EngineGetMaxPrecacheModel();

	for (size_t i = 0; i < m_brushIndexArray.size(); ++i)
	{
		if (m_brushIndexArray[i])
		{
			delete m_brushIndexArray[i];
			m_brushIndexArray[i] = NULL;
		}
	}

	m_brushIndexArray.resize(maxNum);

	for (int i = 0; i < EngineGetMaxPrecacheModel(); ++i)
	{
		auto mod = EngineGetPrecachedModelByIndex(i);
		if (mod && mod->type == mod_brush && mod->name[0])
		{
			if (mod->needload == NL_PRESENT || mod->needload == NL_CLIENT)
			{
				m_brushIndexArray[i] = new indexarray_t;
				GenerateIndexedArrayForBrush(mod, m_worldVertexArray, m_brushIndexArray[i]);
			}
		}
	}

	//Free allocated glpoly_t

	auto surf = r_worldmodel->surfaces;

	for (int i = 0; i < r_worldmodel->numsurfaces; i++)
	{
		if ((surf[i].flags & (SURF_DRAWTURB | SURF_UNDERWATER | SURF_DRAWSKY)))
			continue;

		auto poly = surf[i].polys;

		if (poly)
		{
			surf[i].polys = NULL;
		}
	}

	for (size_t i = 0; i < glpolys.size(); ++i)
	{
		free(glpolys[i]);
	}
}

void CPhysicsManager::BuildSurfaceDisplayList(model_t *mod, msurface_t* fa, std::vector<glpoly_t*> &glpolys)
{
#define BLOCK_WIDTH 128
#define BLOCK_HEIGHT 128
	int i, lindex, lnumverts;
	medge_t* pedges, * r_pedge;
	float* vec;
	float s, t;
	glpoly_t* poly;

	pedges = mod->edges;
	lnumverts = fa->numedges;

	int allocSize = (int)sizeof(glpoly_t) + ((lnumverts - 4) * VERTEXSIZE * sizeof(float));

	if (allocSize < 0)
		return;

	poly = (glpoly_t *)malloc(allocSize);

	glpolys.emplace_back(poly);

	poly->next = fa->polys;
	poly->flags = fa->flags;
	fa->polys = poly;
	poly->numverts = lnumverts;
	poly->chain = NULL;

	for (i = 0; i < lnumverts; i++)
	{
		lindex = mod->surfedges[fa->firstedge + i];

		if (lindex > 0)
		{
			r_pedge = &pedges[lindex];
			vec = mod->vertexes[r_pedge->v[0]].position;
		}
		else
		{
			r_pedge = &pedges[-lindex];
			vec = mod->vertexes[r_pedge->v[1]].position;
		}

		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s /= fa->texinfo->texture->width;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t /= fa->texinfo->texture->height;

		poly->verts[i][0] = vec[0];
		poly->verts[i][1] = vec[1];
		poly->verts[i][2] = vec[2];
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		s = DotProduct(vec, fa->texinfo->vecs[0]) + fa->texinfo->vecs[0][3];
		s -= fa->texturemins[0];
		s += fa->light_s * 16;
		s += 8;
		s /= BLOCK_WIDTH * 16;

		t = DotProduct(vec, fa->texinfo->vecs[1]) + fa->texinfo->vecs[1][3];
		t -= fa->texturemins[1];
		t += fa->light_t * 16;
		t += 8;
		t /= BLOCK_HEIGHT * 16;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;
	}

	poly->numverts = lnumverts;
}

void CPhysicsManager::GenerateWorldVerticeArray(std::vector<glpoly_t*> &glpolys)
{
	if (m_worldVertexArray) {
		delete m_worldVertexArray;
		m_worldVertexArray = NULL;
	}

	m_worldVertexArray = new vertexarray_t;

	brushvertex_t Vertexes[3];

	int iNumVerts = 0;

	auto surf = r_worldmodel->surfaces;

	m_worldVertexArray->vFaceBuffer.resize(r_worldmodel->numsurfaces);

	for (int i = 0; i < r_worldmodel->numsurfaces; i++)
	{
		if ((surf[i].flags & (SURF_DRAWTURB | SURF_UNDERWATER | SURF_DRAWSKY)))
			continue;

		BuildSurfaceDisplayList(r_worldmodel, &surf[i], glpolys);

		auto poly = surf[i].polys;

		if (!poly)
			continue;

		poly->flags = i;

		brushface_t* brushface = &m_worldVertexArray->vFaceBuffer[i];

		int iStartVert = iNumVerts;

		brushface->start_vertex = iStartVert;

		for (poly = surf[i].polys; poly; poly = poly->next)
		{
			auto v = poly->verts[0];

			for (int j = 0; j < 3; j++, v += VERTEXSIZE)
			{
				Vertexes[j].pos[0] = v[0];
				Vertexes[j].pos[1] = v[1];
				Vertexes[j].pos[2] = v[2];
			}

			m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[0]);
			m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[1]);
			m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[2]);
			iNumVerts += 3;

			for (int j = 0; j < (poly->numverts - 3); j++, v += VERTEXSIZE)
			{
				Vertexes[1].pos[0] = Vertexes[2].pos[0];
				Vertexes[1].pos[1] = Vertexes[2].pos[1];
				Vertexes[1].pos[2] = Vertexes[2].pos[2];
				//memcpy(&Vertexes[1], &Vertexes[2], sizeof(brushvertex_t));

				Vertexes[2].pos[0] = v[0];
				Vertexes[2].pos[1] = v[1];
				Vertexes[2].pos[2] = v[2];

				m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[0]);
				m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[1]);
				m_worldVertexArray->vVertexBuffer.emplace_back(Vertexes[2]);
				iNumVerts += 3;
			}
		}

		brushface->num_vertexes = iNumVerts - iStartVert;
	}
}

void CPhysicsManager::GenerateIndexedArrayForBrushface(brushface_t* brushface, indexarray_t* indexarray)
{
	int first = -1;
	int prv0 = -1;
	int prv1 = -1;
	int prv2 = -1;
	for (int i = 0; i < brushface->num_vertexes; i++)
	{
		if (prv0 != -1 && prv1 != -1 && prv2 != -1)
		{
			indexarray->vIndiceBuffer.emplace_back(brushface->start_vertex + first);
			indexarray->vIndiceBuffer.emplace_back(brushface->start_vertex + prv2);
		}

		indexarray->vIndiceBuffer.emplace_back(brushface->start_vertex + i);

		if (first == -1)
			first = i;

		prv0 = prv1;
		prv1 = prv2;
		prv2 = i;
	}
}

void CPhysicsManager::GenerateIndexedArrayForSurface(msurface_t* psurf, vertexarray_t* vertexarray, indexarray_t* indexarray)
{
	if (psurf->flags & SURF_DRAWTURB)
	{
		return;
	}

	if (psurf->flags & SURF_DRAWSKY)
	{
		return;
	}

	if (psurf->flags & SURF_UNDERWATER)
	{
		return;
	}

	GenerateIndexedArrayForBrushface(&vertexarray->vFaceBuffer[psurf->polys->flags], indexarray);
}

void CPhysicsManager::GenerateIndexedArrayRecursiveWorldNode(mnode_t* node, vertexarray_t* vertexarray, indexarray_t* indexarray)
{
	if (!node)
		return;

	if (node->contents == CONTENTS_SOLID)
		return;

	if (node->contents < 0)
		return;

	GenerateIndexedArrayRecursiveWorldNode(node->children[0], vertexarray, indexarray);

	auto c = node->numsurfaces;

	if (c)
	{
		auto psurf = r_worldmodel->surfaces + node->firstsurface;

		for (; c; c--, psurf++)
		{
			GenerateIndexedArrayForSurface(psurf, vertexarray, indexarray);
		}
	}

	GenerateIndexedArrayRecursiveWorldNode(node->children[1], vertexarray, indexarray);
}

void CPhysicsManager::GenerateIndexedArrayForBrush(model_t* mod, vertexarray_t* vertexarray, indexarray_t* indexarray)
{
	auto psurf = &mod->surfaces[mod->firstmodelsurface];
	for (int i = 0; i < mod->nummodelsurfaces; i++, psurf++)
	{
		GenerateIndexedArrayForSurface(psurf, vertexarray, indexarray);
	}
}

CDynamicObject* CPhysicsManager::CreateDynamicObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, int flags)
{
	//legacy -- dynamic object collide with all other stuffs when pushable, and all non-player stuffs when unpushable

	int mask = btBroadphaseProxy::AllFilter;// &(~btBroadphaseProxy::SensorTrigger);

	mask &= ~FallGuysCollisionFilterGroups::ClippingHullFilter;

	auto dynamicobj = new CDynamicObject(obj, btBroadphaseProxy::DefaultFilter | FallGuysCollisionFilterGroups::DynamicObjectFilter, mask, flags);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(dynamicobj), collisionShape, localInertia);
	cInfo.m_friction = friction;
	cInfo.m_rollingFriction = rollingFriction;
	cInfo.m_restitution = restitution;
	cInfo.m_linearSleepingThreshold = 0.1f;
	cInfo.m_angularSleepingThreshold = 0.001f;

	dynamicobj->SetRigidBody(new btRigidBody(cInfo));

	dynamicobj->GetRigidBody()->setCcdSweptSphereRadius(ccdRadius);
	dynamicobj->GetRigidBody()->setCcdMotionThreshold(ccdThreshold);
	dynamicobj->GetRigidBody()->setRestitution(restitution);

	return dynamicobj;
}

CStaticObject* CPhysicsManager::CreateStaticObject(CGameObject *obj, btCollisionShape *collisionShape, bool kinematic)
{
	auto staticobj = new CStaticObject(
		obj,
		btBroadphaseProxy::StaticFilter,
		btBroadphaseProxy::AllFilter & ~(btBroadphaseProxy::SensorTrigger | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter | FallGuysCollisionFilterGroups::ClippingHullFilter));

	btMotionState* motionState = NULL;

	if (kinematic)
		motionState = new EntityMotionState(staticobj);
	else
		motionState = new btDefaultMotionState();

	btRigidBody::btRigidBodyConstructionInfo cInfo(0, motionState, collisionShape);
	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 1;

	staticobj->SetRigidBody(new btRigidBody(cInfo));

	if (kinematic)
	{
		staticobj->GetRigidBody()->setCollisionFlags(staticobj->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		staticobj->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	}

	return staticobj;
}

CPlayerObject* CPhysicsManager::CreatePlayerObject(CGameObject *obj, btCollisionShape* collisionShape, float ccdRadius, float ccdThreshold, bool duck)
{
	btVector3 localInertia;
	collisionShape->calculateLocalInertia(m_playerMass, localInertia);

	//Player only collides with pushable objects, clipping hull objects, and world (do we really need to collide with world ?)
	auto playerobj = new CPlayerObject(
		obj,
		btBroadphaseProxy::DefaultFilter | FallGuysCollisionFilterGroups::PlayerFilter,
		btBroadphaseProxy::AllFilter & ~(FallGuysCollisionFilterGroups::PlayerFilter),
		m_playerMass,
		m_playerMaxPendingVelocity,
		duck);

	btRigidBody::btRigidBodyConstructionInfo cInfo(m_playerMass, new EntityMotionState(playerobj), collisionShape, localInertia);

	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 0;

	playerobj->SetRigidBody(new btRigidBody(cInfo));

	//Player clipping hull box never rotates
	playerobj->GetRigidBody()->setAngularFactor(0);
	playerobj->GetRigidBody()->setCcdSweptSphereRadius(ccdRadius);
	playerobj->GetRigidBody()->setCcdMotionThreshold(ccdThreshold);

	return playerobj;
}

CClippingHullObject* CPhysicsManager::CreateClippingHullObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, const btTransform& initTransfrm, float mass)
{
	//Cilpping hull only collides with players
	auto hullobj = new CClippingHullObject(
		obj,
		btBroadphaseProxy::DefaultFilter | FallGuysCollisionFilterGroups::ClippingHullFilter,
		FallGuysCollisionFilterGroups::PlayerFilter,
		mass);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new btDefaultMotionState(), collisionShape, localInertia);

	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 0;

	hullobj->SetRigidBody(new btRigidBody(cInfo));

	//Clipping hull box never rotates
	hullobj->GetRigidBody()->setAngularFactor(0);
	hullobj->GetRigidBody()->setWorldTransform(initTransfrm);

	return hullobj;
}

void EulerMatrix(const btVector3& in_euler, btMatrix3x3& out_matrix) {
	btVector3 angles = in_euler;
	angles *= SIMD_RADS_PER_DEG;

	btScalar c1(btCos(angles[0]));
	btScalar c2(btCos(angles[1]));
	btScalar c3(btCos(angles[2]));
	btScalar s1(btSin(angles[0]));
	btScalar s2(btSin(angles[1]));
	btScalar s3(btSin(angles[2]));

	out_matrix.setValue(c1 * c2, -c3 * s2 - s1 * s3 * c2, s3 * s2 - s1 * c3 * c2,
		c1 * s2, c3 * c2 - s1 * s3 * s2, -s3 * c2 - s1 * c3 * s2,
		s1, c1 * s3, c1 * c3);
}

void MatrixEuler(const btMatrix3x3& in_matrix, btVector3& out_euler) {
	
	/*float xyDist = btSqrt(in_matrix[0][0] * in_matrix[0][0] + in_matrix[1][0] * in_matrix[1][0]);

	if (xyDist > 0.001f)
	{
		out_euler[0] = btAtan2(-in_matrix[2][0], xyDist);
		out_euler[1] = btAtan2(in_matrix[1][0], in_matrix[0][0]);
		out_euler[2] = btAtan2(in_matrix[2][1], in_matrix[2][2]);
	}
	else
	{
		out_euler[0] = btAtan2(-in_matrix[2][0], xyDist);
		out_euler[1] = btAtan2(-in_matrix[0][1], in_matrix[1][1]);
		out_euler[2] = 0;
	}*/

	out_euler[0] = btAsin(in_matrix[2][0]);

	if (in_matrix[2][0] >= (1 - 0.002f) && in_matrix[2][0] < 1.002f) {
		out_euler[1] = btAtan2(in_matrix[1][0], in_matrix[0][0]);
		out_euler[2] = btAtan2(in_matrix[2][1], in_matrix[2][2]);
	}
	else if (btFabs(in_matrix[2][0]) < (1 - 0.002f)) {
		out_euler[1] = btAtan2(in_matrix[1][0], in_matrix[0][0]);
		out_euler[2] = btAtan2(in_matrix[2][1], in_matrix[2][2]);
	}
	else {
		out_euler[1] = btAtan2(in_matrix[1][2], in_matrix[1][1]);
		out_euler[2] = 0;
	}

	out_euler[3] = 0;

	out_euler *= SIMD_DEGS_PER_RAD;
}

void EntityMotionState::ResetWorldTransform(const Vector &origin, const Vector &angles)
{
	btVector3 vecOrigin(origin.x, origin.y, origin.z);

	btTransform worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

	btVector3 vecAngles(angles.x, angles.y, angles.z);

	if (GetPhysicObject()->IsPlayerObject())
	{
		vecAngles.setX(0);
		vecAngles.setY(0);
		vecAngles.setZ(0);
	}

	EulerMatrix(vecAngles, worldTrans.getBasis());

	m_worldTransform = worldTrans;
	m_worldTransformInitialized = true;
}

/*
	Upload GoldSrc origin and angles to bullet engine
*/
void EntityMotionState::getWorldTransform(btTransform& worldTrans) const
{
	if (!GetPhysicObject()->GetGameObject())
		return;

	auto ent = GetPhysicObject()->GetGameObject()->GetEdict();

	if (!ent || ent->free)
		return;

	//Player and brush upload origin and angles in normal way

	if (!GetPhysicObject()->IsDynamicObject())
	{
		btVector3 vecOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

		worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

		btVector3 vecAngles(ent->v.angles.x, ent->v.angles.y, ent->v.angles.z);

		//Brush uses reverted pitch
		if (ent->v.solid == SOLID_BSP)
		{
			vecAngles.setX(-vecAngles.x());
		}

		if (GetPhysicObject()->IsPlayerObject())
		{
			vecAngles.setX(0);
			vecAngles.setY(0);
			vecAngles.setZ(0);
		}

		EulerMatrix(vecAngles, worldTrans.getBasis());
	}
	else
	{
		if (m_worldTransformInitialized)
		{
			worldTrans = m_worldTransform;
		}
		else
		{
			btVector3 vecOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

			worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

			btVector3 vecAngles(ent->v.angles.x, ent->v.angles.y, ent->v.angles.z);

			if (ent->v.solid == SOLID_BSP)
			{
				vecAngles.setX(-vecAngles.x());
			}

			EulerMatrix(vecAngles, worldTrans.getBasis());

			m_worldTransform = worldTrans;
			m_worldTransformInitialized = true;
		}
	}
}

/*
	Download GoldSrc origin and angles from bullet engine
*/
void EntityMotionState::setWorldTransform(const btTransform& worldTrans)
{
	//Never download player state
	if (GetPhysicObject()->IsPlayerObject())
	{
		return;
	}

	if (!GetPhysicObject()->GetGameObject())
		return;

	auto ent = GetPhysicObject()->GetGameObject()->GetEdict();

	if (!ent || ent->free)
		return;

	auto &vecOrigin = worldTrans.getOrigin();

	Vector origin = Vector(vecOrigin.getX(), vecOrigin.getY(), vecOrigin.getZ());

	btVector3 vecAngles;
	MatrixEuler(worldTrans.getBasis(), vecAngles);

	Vector angles = Vector(vecAngles.getX(), vecAngles.getY(), vecAngles.getZ());

	/*if (strstr((*sv_models)[ent->v.modelindex]->name, "log.mdl"))
	{
		ALERT(ALERT_TYPE::at_console, "matrix[0] %f %f %f\n", worldTrans.getBasis()[0][0], worldTrans.getBasis()[0][1], worldTrans.getBasis()[0][2]);
		ALERT(ALERT_TYPE::at_console, "matrix[1] %f %f %f\n", worldTrans.getBasis()[1][0], worldTrans.getBasis()[1][1], worldTrans.getBasis()[1][2]);
		ALERT(ALERT_TYPE::at_console, "matrix[2] %f %f %f\n", worldTrans.getBasis()[2][0], worldTrans.getBasis()[2][1], worldTrans.getBasis()[2][2]);
		ALERT(ALERT_TYPE::at_console, "angles %f %f %f\n", angles.x, angles.y, angles.z);
	}*/

	//Clamp to -3600~3600
	for (int i = 0; i < 3; i++)
	{
		if (angles[i] < -3600.0f || angles[i] > 3600.0f)
			angles[i] = fmod(angles[i], 3600.0f);
	}

	SET_ORIGIN(ent, origin);

	if (ent->v.solid == SOLID_BSP)
	{
		angles.x = -angles.x;
	}

	ent->v.angles = angles;
}

void CPhysicsManager::EntityStartFrame()
{
	m_solidPlayerMask = 0;

	for (int i = 1; i < gpGlobals->maxEntities; ++i)
	{
		auto ent = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!ent || ent->free || (ent->v.flags & FL_KILLME))
			continue;

		auto obj = m_gameObjects[i];

		if (!obj)
		{
			if (IsEntitySolidPusher(ent))
			{
				if (g_bIsFallGuysSeason1 && Legacy_IsEntitySuperPusher(ent))
				{
					SetEntitySuperPusher(ent, true);
				}

				CreateBrushModel(ent);
				continue;
			}
			if (IsEntitySolidPlayer(i, ent))
			{
				CreatePlayerBox(ent);
				continue;
			}
		}
		else
		{
			//entity freed?
			if (ent->free)
			{
				RemoveGameObject(i);
				continue;
			}
			//Just in case not created?
			if (IsEntitySolidPusher(ent))
			{
				CreateBrushModel(ent);
			}

			if (IsEntitySolidPlayer(i, ent))
			{
				m_solidPlayerMask |= (1 << (i - 1));
			}
			
			obj->StartFrame(m_dynamicsWorld);
		}
	}
}

void CPhysicsManager::EntityStartFrame_Post()
{
	for (int i = 1; i < gpGlobals->maxEntities; ++i)
	{
		auto ent = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!ent || ent->free || (ent->v.flags & FL_KILLME))
			continue;

		auto body = m_gameObjects[i];

		if (body)
		{
			body->StartFrame_Post(m_dynamicsWorld);
		}
	}
}

void CPhysicsManager::EntityEndFrame()
{
	for (int i = 1; i < gpGlobals->maxEntities; ++i)
	{
		auto ent = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!ent || ent->free || (ent->v.flags & FL_KILLME))
			continue;

		auto body = m_gameObjects[i];

		if (body)
		{
			body->EndFrame(m_dynamicsWorld);
		}
	}
}

void CDynamicObject::StartFrame(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	//Upload linear velocity to bullet engine (if changed by user code)

	if (!IsKinematic())
	{
		btVector3 vecLinearVelocity(ent->v.vuser1.x, ent->v.vuser1.y, ent->v.vuser1.z);

		GetRigidBody()->setLinearVelocity(vecLinearVelocity);

		btVector3 vecAngularVelocity(ent->v.vuser2.x, ent->v.vuser2.y, ent->v.vuser2.z);

		GetRigidBody()->setAngularVelocity(vecAngularVelocity);
	}
}

void CDynamicObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	//Download linear velocity from bullet engine

	if (!IsKinematic())
	{
		auto vecLinearVelocity = GetRigidBody()->getLinearVelocity();
		auto vecAngularVelocity = GetRigidBody()->getAngularVelocity();

		Vector vel(vecLinearVelocity.getX(), vecLinearVelocity.getY(), vecLinearVelocity.getZ());
		Vector avel(vecAngularVelocity.getX(), vecAngularVelocity.getY(), vecAngularVelocity.getZ());

		ent->v.basevelocity = g_vecZero;
		ent->v.velocity = g_vecZero;
		ent->v.avelocity = g_vecZero;
		ent->v.vuser1 = vel;
		ent->v.vuser2 = avel;
	}

	//Dispatch impact event
	if (m_ImpactEntity)
	{
		if (gpGlobals->time > m_flImpactTime + 0.1f)
		{
			gPhysicsManager.SetCurrentImpactEntity(m_ImpactEntity, m_ImpactPoint, m_ImpactDirection, m_flImpactImpulse);

			gpGamedllFuncs->dllapi_table->pfnTouch(ent, m_ImpactEntity);

			gPhysicsManager.SetCurrentImpactEntity(NULL, g_vecZero, g_vecZero, 0);

			m_flImpactTime = gpGlobals->time;
		}
		m_ImpactEntity = NULL;
	}
}

void CDynamicObject::EndFrame(btDiscreteDynamicsWorld* world)
{

}

void CDynamicObject::SetPhysicTransform(const Vector &origin, const Vector &angles)
{
	/*auto MotionState = (EntityMotionState *)GetRigidBody()->getMotionState();

	MotionState->ResetWorldTransform(origin, angles);*/

	btVector3 vecOrigin(origin.x, origin.y, origin.z);

	btTransform worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

	btVector3 vecAngles(angles.x, angles.y, angles.z);

	EulerMatrix(vecAngles, worldTrans.getBasis());

	GetRigidBody()->setWorldTransform(worldTrans);
}

void CDynamicObject::SetPhysicFreeze(bool freeze)
{
	if (freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else if (!freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		GetRigidBody()->forceActivationState(ACTIVE_TAG);
	}
}

void CClippingHullObject::SetPhysicTransform(const Vector &origin, const Vector &angles)
{

}

void CClippingHullObject::SetPhysicFreeze(bool freeze)
{
	if (freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else if (!freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		GetRigidBody()->forceActivationState(ACTIVE_TAG);
	}
}

void CPlayerObject::EndFrame(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	if (ent->v.solid <= SOLID_TRIGGER)
		return;

	if (ent->v.movetype == MOVETYPE_NOCLIP)
		return;

	if (!IsDuck() && ent->v.bInDuck)
		return;

	if (IsDuck() && !ent->v.bInDuck)
		return;

	m_TickCount++;

	auto hitent = SV_TestEntityPositionEx(ent);

	if (hitent)
	{
		m_BlockingTickCount = m_TickCount;

		auto hitentindex = g_engfuncs.pfnIndexOfEdict(hitent);

		auto hitobj = gPhysicsManager.GetGameObject(hitentindex);

		if (hitobj && hitobj->HasPhysObjects())
		{
			vec3_t old_origin, new_origin, move;

			old_origin = ent->v.origin;

			btVector3 vecNewOrigin = GetRigidBody()->getWorldTransform().getOrigin();

			new_origin.x = vecNewOrigin.getX();
			new_origin.y = vecNewOrigin.getY();
			new_origin.z = vecNewOrigin.getZ();

			new_origin.z += 1.5f;

			move = new_origin - old_origin;

			//Probably not going to push player aside
			if (move.x == 0 && move.y == 0)
			{
				move = (old_origin - hitent->v.origin).Normalize();
			}

			int original_solid = hitent->v.solid;
			hitent->v.solid = SOLID_NOT;

			g_PusherEntity = hitent;
			g_bIsPushPhysics = true;

			if (m_ImpactEntity == hitent)
			{
				gPhysicsManager.SetCurrentImpactEntity(m_ImpactEntity, m_ImpactPoint, m_ImpactDirection, m_flImpactImpulse);

				NewSV_PushEntity(ent, &move);

				gPhysicsManager.SetCurrentImpactEntity(NULL, g_vecZero, g_vecZero, 0);
			}
			else
			{
				NewSV_PushEntity(ent, &move);
			}

			g_bIsPushPhysics = false;
			g_PusherEntity = NULL;

			hitent->v.solid = original_solid;

			auto block = SV_TestEntityPositionEx(ent);
			if (block)
			{
				if (block == hitent)
				{
					//Save pending velocity and apply when unblocked

					vec3_t new_velocity;

					btVector3 vecLinearVelocity = GetRigidBody()->getLinearVelocity();

					new_velocity.x = vecLinearVelocity.getX();
					new_velocity.y = vecLinearVelocity.getY();
					new_velocity.z = vecLinearVelocity.getZ();

					m_PendingVelocity = new_velocity;
				}
				else
				{
					SET_ORIGIN(ent, old_origin);

					gpDllFunctionsTable->pfnBlocked(hitent, ent);
				}
			}
			else
			{
				vec3_t new_velocity;

				btVector3 vecLinearVelocity = GetRigidBody()->getLinearVelocity();

				new_velocity.x = vecLinearVelocity.getX();
				new_velocity.y = vecLinearVelocity.getY();
				new_velocity.z = vecLinearVelocity.getZ();

				ent->v.velocity = new_velocity;
			}
		}
	}
	else
	{
		//Apply pending velocity when unblocked
		if (m_TickCount == m_BlockingTickCount + 1)
		{
			auto vecNewVelocity = ent->v.velocity + m_PendingVelocity;

			if (m_flMaxPendingVelocity > 0 && vecNewVelocity.Length() > m_flMaxPendingVelocity)
			{
				vecNewVelocity = vecNewVelocity.Normalize() * m_flMaxPendingVelocity;
			}

			ent->v.velocity = vecNewVelocity;

			m_PendingVelocity = g_vecZero;
		}
	}

	m_ImpactEntity = NULL;
	m_flImpactImpulse = 0;
	m_ImpactPoint = g_vecZero;
}

bool CDynamicObject::SetAbsBox(edict_t *ent)
{
	btVector3 aabbMins, aabbMaxs;

	GetRigidBody()->getAabb(aabbMins, aabbMaxs);

	ent->v.absmin.x = aabbMins.getX();
	ent->v.absmin.y = aabbMins.getY();
	ent->v.absmin.z = aabbMins.getZ();
	ent->v.absmax.x = aabbMaxs.getX();
	ent->v.absmax.y = aabbMaxs.getY();
	ent->v.absmax.z = aabbMaxs.getZ();

	//additional 16 unit ?
	ent->v.absmin.x -= 16;
	ent->v.absmin.y -= 16;
	ent->v.absmin.z -= 16;
	ent->v.absmax.x += 16;
	ent->v.absmax.y += 16;
	ent->v.absmax.z += 16;

	return true;
}

void CDynamicObject::DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent)
{
	if ((m_PhysicObjectFlags & PhysicObject_HasImpactImpulse) && impulse > m_flImpactImpulseThreshold && impulse > m_flImpactImpulse)
	{
		vec3_t ImpactPoint(worldpos_on_source.getX(), worldpos_on_source.getY(), worldpos_on_source.getZ());
		vec3_t ImpactDirection(normal.getX(), normal.getY(), normal.getZ());

		m_flImpactImpulse = impulse;
		m_ImpactPoint = ImpactPoint;
		m_ImpactDirection = ImpactDirection;
		m_ImpactEntity = hitent;
	}
}

void CPlayerObject::DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent)
{
	if (impulse > m_flImpactImpulse)
	{
		vec3_t ImpactPoint(worldpos_on_source.getX(), worldpos_on_source.getY(), worldpos_on_source.getZ());
		vec3_t ImpactDirection(normal.getX(), normal.getY(), normal.getZ());

		m_flImpactImpulse = impulse;
		m_ImpactPoint = ImpactPoint;
		m_ImpactDirection = ImpactDirection;
		m_ImpactEntity = hitent;
	}
}

void CPlayerObject::StartFrame(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	//Upload to bullet engine before simulation
	btTransform trans;
	GetRigidBody()->getMotionState()->getWorldTransform(trans);
	GetRigidBody()->setWorldTransform(trans);

	//Do we really need this?
	btVector3 vecVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

	GetRigidBody()->setLinearVelocity(vecVelocity);
}

void CPlayerObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	
}

void CSolidOptimizerGhostPhysicObject::StartFrame(btDiscreteDynamicsWorld* world)
{
	vec3_t bone_origin, bone_angles;

	auto ent = GetGameObject()->GetEdict();

	if ( ent->v.origin != m_cached_origin || ent->v.sequence != m_cached_sequence || ent->v.frame != m_cached_frame)
	{
		m_cached_origin = ent->v.origin;
		m_cached_sequence = ent->v.sequence;
		m_cached_frame = ent->v.frame;

		g_engfuncs.pfnGetBonePosition(ent, m_boneindex, bone_origin, bone_angles);

		m_cached_boneorigin = bone_origin;
		m_cached_boneangles = bone_angles;

		btVector3 vecOrigin(bone_origin.x, bone_origin.y, bone_origin.z);

		btTransform	worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

		btVector3 vecAngles(bone_angles.x, bone_angles.y, bone_angles.z);

		EulerMatrix(vecAngles, worldTrans.getBasis());

		GetGhostObject()->setWorldTransform(worldTrans);
	}
	else
	{
		bone_origin = m_cached_boneorigin;
		bone_angles = m_cached_boneangles;
	}
}

void CSolidOptimizerGhostPhysicObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	btManifoldArray ManifoldArray;
	btBroadphasePairArray& PairArray = GetGhostObject()->getOverlappingPairCache()->getOverlappingPairArray();

	for (int i = 0; i < PairArray.size(); i++)
	{
		ManifoldArray.clear();

		btBroadphasePair* CollisionPair = world->getPairCache()->findPair(PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

		if (!CollisionPair)
		{
			continue;
		}

		if (CollisionPair->m_algorithm)
		{
			CollisionPair->m_algorithm->getAllContactManifolds(ManifoldArray);
		}

		for (int j = 0; j < ManifoldArray.size(); j++)
		{
			for (int p = 0; p < ManifoldArray[j]->getNumContacts(); p++)
			{
				const btManifoldPoint& Point = ManifoldArray[j]->getContactPoint(p);

				//if (Point.getDistance() < 0.0f)
				{
					auto rigidbody = btRigidBody::upcast(ManifoldArray[j]->getBody0());

					if (!rigidbody)
					{
						rigidbody = btRigidBody::upcast(ManifoldArray[j]->getBody1());
					}

					if (rigidbody)
					{
						auto physobj = (CPhysicObject *)rigidbody->getUserPointer();

						if (physobj->IsPlayerObject())
						{
							int playerIndex = physobj->GetGameObject()->GetEntIndex();

							GetGameObject()->RemoveSemiClipMask((1 << (playerIndex - 1)));
						}
					}
				}
			}
		}
	}
}

void CCachedBoneSolidOptimizer::StartFrame(CGameObject *obj)
{
	vec3_t bone_origin, bone_angles;

	auto ent = obj->GetEdict();

	if (ent->v.origin != m_cached_origin || ent->v.sequence != m_cached_sequence || ent->v.frame != m_cached_frame)
	{
		m_cached_origin = ent->v.origin;
		m_cached_sequence = ent->v.sequence;
		m_cached_frame = ent->v.frame;

		btTransform worldTrans;

		g_engfuncs.pfnGetBonePosition(ent, m_boneindex, bone_origin, bone_angles);

		m_cached_boneorigin = bone_origin;
		m_cached_boneangles = bone_angles;
	}
	else
	{
		bone_origin = m_cached_boneorigin;
		bone_angles = m_cached_boneangles;
	}
}

void CPhysicTriggerGhostPhysicObject::StartFrame(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	btVector3 vecOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

	btTransform worldTrans = btTransform(btQuaternion(0, 0, 0, 1), vecOrigin);

	btVector3 vecAngles(ent->v.angles.x, ent->v.angles.y, ent->v.angles.z);

	if (ent->v.solid == SOLID_BSP)
	{
		vecAngles.setX(-vecAngles.x());
	}

	EulerMatrix(vecAngles, worldTrans.getBasis());

	GetGhostObject()->setWorldTransform(worldTrans);
}

void CPhysicTriggerGhostPhysicObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	btManifoldArray ManifoldArray;
	btBroadphasePairArray& PairArray = GetGhostObject()->getOverlappingPairCache()->getOverlappingPairArray();

	for (int i = 0; i < PairArray.size(); i++)
	{
		ManifoldArray.clear();

		btBroadphasePair* CollisionPair = world->getPairCache()->findPair(PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

		if (!CollisionPair)
		{
			continue;
		}

		if (CollisionPair->m_algorithm)
		{
			CollisionPair->m_algorithm->getAllContactManifolds(ManifoldArray);
		}

		for (int j = 0; j < ManifoldArray.size(); j++)
		{
			for (int p = 0; p < ManifoldArray[j]->getNumContacts(); p++)
			{
				const btManifoldPoint& Point = ManifoldArray[j]->getContactPoint(p);

				//if (Point.getDistance() < 0.0f)
				{
					auto rigidbody = btRigidBody::upcast(ManifoldArray[j]->getBody0());

					if (!rigidbody)
					{
						rigidbody = btRigidBody::upcast(ManifoldArray[j]->getBody1());
					}

					if (rigidbody)
					{
						auto physobj = (CPhysicObject *)rigidbody->getUserPointer();

						if (physobj->IsDynamicObject())
						{
							auto physent = physobj->GetGameObject()->GetEdict();

							gpGamedllFuncs->dllapi_table->pfnTouch(ent, physent);
						}
					}
				}
			}
		}
	}
}


void CPhysicsManager::FreeEntityPrivateData(edict_t* ent)
{
	RemoveGameObject(g_engfuncs.pfnIndexOfEdict(ent));
}

bool CPhysicsManager::SetAbsBox(edict_t *ent)
{
	auto obj = GetGameObject(ent);

	if (obj)
	{
		return obj->SetAbsBox(ent);
	}

	return false;
}

bool CPhysicsManager::AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player)
{
	auto obj = GetGameObject(ent);

	if (obj)
	{
		if (!obj->AddToFullPack(state, entindex, ent, host, hostflags, player))
		{
			return false;
		}
	}

	return true;
}

int CPhysicsManager::PM_FlyMove()
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	vec3_t primal_velocity, original_velocity;
	vec3_t new_velocity;
	int i, j;
	pmtrace_t trace;
	vec3_t end;
	float time_left, allFraction;
	int blocked;

	numbumps = 4;

	blocked = 0;
	numplanes = 0;

	original_velocity = pmove->velocity;
	primal_velocity = pmove->velocity;

	allFraction = 0;
	time_left = pmove->frametime;

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (!pmove->velocity[0] && !pmove->velocity[1] && !pmove->velocity[2])
			break;

		for (i = 0; i < 3; i++)
			end[i] = pmove->origin[i] + time_left * pmove->velocity[i];

		trace = pmove->PM_PlayerTrace(pmove->origin, end, PM_NORMAL, -1);

		allFraction += trace.fraction;

		if (trace.allsolid)
		{
			pmove->velocity = g_vecZero;
			return 4;
		}

		if (trace.fraction > 0)
		{
			pmove->origin = trace.endpos;
			original_velocity = pmove->velocity;
			numplanes = 0;
		}

		if (trace.fraction == 1)
			break;

		//PM_AddToTouched(trace, pmove->velocity);

		if (trace.plane.normal[2] > 0.7)
			blocked |= 1;

		if (!trace.plane.normal[2])
			blocked |= 2;

		time_left -= trace.fraction;

		if (numplanes >= MAX_CLIP_PLANES)
		{
			pmove->velocity = g_vecZero;
			break;
		}

		planes[numplanes] = trace.plane.normal;;
		numplanes++;

		if (pmove->movetype == MOVETYPE_WALK && ((pmove->onground == -1) || (pmove->friction != 1)))
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i][2] > 0.7)
				{
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1);
					original_velocity = new_velocity;
				}
				else
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1.0f + pmove->movevars->bounce * (1 - pmove->friction));
			}

			pmove->velocity = new_velocity;
			original_velocity = new_velocity;
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				PM_ClipVelocity(original_velocity, planes[i], pmove->velocity, 1);

				for (j = 0; j < numplanes; j++)
				{
					if (j != i)
					{
						if (DotProduct(pmove->velocity, planes[j]) < 0)
							break;
					}
				}

				if (j == numplanes)
					break;
			}

			if (i != numplanes)
			{
			}
			else
			{
				if (numplanes != 2)
				{
					pmove->velocity = g_vecZero;
					break;
				}

				dir = CrossProduct(planes[0], planes[1]);
				d = DotProduct(dir, pmove->velocity);
				pmove->velocity = dir * d;
			}

			if (DotProduct(pmove->velocity, primal_velocity) <= 0)
			{
				pmove->velocity = g_vecZero;
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		pmove->velocity = g_vecZero;
	}

	return blocked;
}

int CPhysicsManager::PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
#define STOP_EPSILON 0.1

	float backoff;
	float change;
	float angle;
	int i, blocked;

	angle = normal[2];

	blocked = 0x00;

	if (angle > 0)
		blocked |= 0x01;

	if (!angle)
		blocked |= 0x02;

	backoff = DotProduct(in, normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}


qboolean CPhysicsManager::PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity)
{
	int i;

	for (i = 0; i < pmove->numtouch; i++)
	{
		if (pmove->touchindex[i].ent == tr.ent)
			break;
	}

	if (i != pmove->numtouch)
		return false;

	//mark me as super-pusher
	tr.deltavelocity = impactvelocity;
	tr.hitgroup = 1;

	if (pmove->numtouch >= MAX_PHYSENTS)
	{
		//pmove->Con_DPrintf("Too many entities were touched!\n");
		return false;
	}

	pmove->touchindex[pmove->numtouch++] = tr;
	return true;
}

bool CPhysicsManager::PM_ShouldCollide(int entindex)
{
	if (entindex > 0)
	{
		int playerIndex = pmove->player_index + 1;

		auto obj = GetGameObject(entindex);

		if (obj && obj->IsSolidOptimizerEnabled())
		{
			if ((obj->GetSemiClipMask() & (playerIndex - 1)))
			{
				return false;
			}
		}
	}

	return true;
}

void CPhysicsManager::PM_StartMove()
{
	std::remove_if(pmove->physents, pmove->physents + pmove->numphysent, [this](const physent_t& ps) {
		return !PM_ShouldCollide(ps.info);
	});
}

void CPhysicsManager::PM_EndMove()
{

}

void CPhysicsManager::AddGameObject(CGameObject *obj)
{
	m_gameObjects[obj->GetEntIndex()] = obj;

	if (obj->GetEntIndex() > m_maxIndexGameObject)
		m_maxIndexGameObject = obj->GetEntIndex();
}

bool CPhysicsManager::CreateBrushModel(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	//Already created as physic brush entity?
	if (obj->GetNumPhysicObject() > 0)
		return false;

	auto shape = CreateTriMeshShapeFromBrushModel(ent);

	if (!shape)
		return false;

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH && ent->v.solid == SOLID_BSP)) ? true : false;

	auto staticObject = CreateStaticObject(obj, shape, bKinematic);
	
	obj->AddPhysicObject(staticObject, m_dynamicsWorld, &m_numDynamicObjects);

	if (ent->v.flags & FL_CONVEYOR)
	{
		staticObject->GetRigidBody()->setCollisionFlags(staticObject->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	return true;
}

bool CPhysicsManager::CreatePlayerBox(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	vec3_t hull_player(16, 16, 36);
	
	vec3_t hull_duck(16, 16, 18);

	if (1)
	{
		//4 units extended to push other physic objects, and -1 unit to prevent player from crushing objects into ground

		auto shape = new btBoxShape(btVector3(hull_player.x + 4, hull_player.y + 4, hull_player.z - 1.0f));

		auto playerobj = CreatePlayerObject(obj, shape, 36, 400 * m_simrate, false);

		obj->AddPhysicObject(playerobj, m_dynamicsWorld, &m_numDynamicObjects);
	}

	if (1)
	{
		auto shape = new btBoxShape(btVector3(hull_duck.x + 4, hull_duck.y + 4, hull_duck.z - 1.0f));

		auto playerobj = CreatePlayerObject(obj, shape, 18, 400 * m_simrate, true);

		obj->AddPhysicObject(playerobj, m_dynamicsWorld, &m_numDynamicObjects);
	}
	
	return true;
}

bool CPhysicsManager::ApplyPhysicImpulse(edict_t* ent, const Vector& impulse, const Vector& origin)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	for (size_t i = 0; i < obj->GetNumPhysicObject(); ++i)
	{
		auto physicObject = obj->GetPhysicObjectByIndex(i);

		if (physicObject->IsDynamicObject())
		{
			btVector3 vecImpulse(impulse.x, impulse.y, impulse.z);

			btVector3 vecOrigin(origin.x, origin.y, origin.z);

			auto dynamicObject = (CDynamicObject *)physicObject;

			btVector3 vecRelPos = vecOrigin - dynamicObject->GetRigidBody()->getCenterOfMassPosition();

			dynamicObject->GetRigidBody()->applyImpulse(vecImpulse, vecRelPos);

			return true;
		}
	}

	return false;
}

bool CPhysicsManager::ApplyPhysicForce(edict_t* ent, const Vector& force, const Vector& origin)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	for (size_t i = 0; i < obj->GetNumPhysicObject(); ++i)
	{
		auto physicObject = obj->GetPhysicObjectByIndex(i);

		if (physicObject->IsDynamicObject())
		{
			btVector3 vecImpulse(force.x, force.y, force.z);

			btVector3 vecOrigin(origin.x, origin.y, origin.z);

			auto dynamicObject = (CDynamicObject *)physicObject;

			btVector3 vecRelPos = vecOrigin - dynamicObject->GetRigidBody()->getCenterOfMassPosition();

			dynamicObject->GetRigidBody()->applyForce(vecImpulse, vecRelPos);

			return true;
		}
	}

	return false;
}

bool CPhysicsManager::SetVehicleEngine(edict_t* ent, int wheelIndex, bool enableMotor, float angularVelcoity, float maxMotorForce)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!obj->GetNumPhysicObject())
		return false;

	auto physObject = obj->GetPhysicObjectByIndex(0);

	if(!physObject->IsDynamicObject())
		return false;

	auto dynObject = (CDynamicObject *)physObject;

	if (!dynObject->GetVehicleManager())
		return false;

	auto pConstraint = dynObject->GetVehicleManager()->GetConstraint(wheelIndex);

	pConstraint->enableMotor(3, enableMotor);
	pConstraint->setTargetVelocity(3, angularVelcoity);
	pConstraint->setMaxMotorForce(3, maxMotorForce);

	return true;
}

bool CPhysicsManager::SetVehicleSteering(edict_t* ent, int wheelIndex, float angularTarget, float angularVelocity, float maxMotorForce)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!obj->GetNumPhysicObject())
		return false;

	auto physObject = obj->GetPhysicObjectByIndex(0);

	if (!physObject->IsDynamicObject())
		return false;

	auto dynObject = (CDynamicObject *)physObject;

	if (!dynObject->GetVehicleManager())
		return false;

	auto pConstraint = dynObject->GetVehicleManager()->GetConstraint(wheelIndex);

	pConstraint->setServoTarget(5, angularTarget);
	pConstraint->setTargetVelocity(5, angularVelocity);
	pConstraint->setMaxMotorForce(5, maxMotorForce);

	return true;
}

bool CPhysicsManager::SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetLevelOfDetail(flags, body_0, scale_0, body_1, scale_1, distance_1, body_2, scale_2, distance_2, body_3, scale_3, distance_3);

	return false;
}

bool CPhysicsManager::SetEntitySemiVisible(edict_t* ent, int player_mask)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetSemiVisibleMask(player_mask);

	return true;
}

bool CPhysicsManager::SetPhysicObjectTransform(edict_t* ent, const Vector &origin, const Vector &angles)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	for (size_t i = 0; i < obj->GetNumPhysicObject(); ++i)
	{
		auto physobj = obj->GetPhysicObjectByIndex(i);

		physobj->SetPhysicTransform(origin, angles);
	}

	SET_ORIGIN(ent, origin);

	ent->v.angles = angles;

	return true;
}

bool CPhysicsManager::SetPhysicObjectFreeze(edict_t* ent, bool freeze)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	for (size_t i = 0; i < obj->GetNumPhysicObject(); ++i)
	{
		auto physobj = obj->GetPhysicObjectByIndex(i);

		physobj->SetPhysicFreeze(freeze);
	}

	return true;
}

bool CPhysicsManager::SetEntityFollow(edict_t* ent, edict_t* follow, int flags, const Vector &origin_offset, const Vector &angles_offset)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetEntityFollow(follow, flags, origin_offset, angles_offset);

	return true;
}

bool CPhysicsManager::SetEntityEnvStudioAnim(edict_t* ent, int flags, float overrideCurFrame, float overrideMaxFrame, EnvStudioKeyframe **keyframes, size_t numKeyframes)
{
	if (ent->free)
		return false;

	if (!ent->v.modelindex)
	{
		//Must have a model
		return false;
	}

	auto mod = (*sv_models)[ent->v.modelindex];

	if (!mod)
	{
		//Must have a model
		return false;
	}

	if (mod->type != mod_studio && mod->type != mod_sprite)
	{
		//Must be studio or sprite
		return false;
	}

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetEntityEnvStudioAnim(flags, overrideCurFrame, overrideMaxFrame, keyframes, numKeyframes);

	return true;
}

bool CPhysicsManager::SetEntitySuperPusher(edict_t* ent, bool enable)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetSuperPusherEnabled(enable);

	return true;
}

bool CPhysicsManager::SetEntityCustomMoveSize(edict_t* ent, const Vector &mins, const Vector &maxs )
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetEntityCustomMoveSize(mins, maxs);

	return true;
}
bool CPhysicsManager::CreatePhysicVehicle(edict_t* ent, PhysicWheelParams **wheelParamArray, size_t numWheelParam, PhysicVehicleParams *vehicleParams)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!obj->GetNumPhysicObject())
	{
		return false;
	}
	
	auto physObject = obj->GetPhysicObjectByIndex(0);

	if(!physObject->IsDynamicObject())
	{
		return false;
	}

	auto dynObject = (CDynamicObject *)physObject;

	dynObject->SetVehicleManager(new CPhysicVehicleManager());

	for (size_t i = 0; i < numWheelParam; ++i)
	{
		auto wheelEnt = wheelParamArray[i]->ent;

		auto wheelObject = GetGameObject(wheelEnt);

		if (wheelObject->GetNumPhysicObject() > 0)
		{
			auto wheelPhysObject = wheelObject->GetPhysicObjectByIndex(0);

			if (wheelPhysObject->IsDynamicObject())
			{
				auto wheelDynObject = (CDynamicObject *)wheelPhysObject;

				btVector3 anchor(wheelParamArray[i]->connectionPoint.x, wheelParamArray[i]->connectionPoint.y, wheelParamArray[i]->connectionPoint.z);
				btVector3 parentAxis(wheelParamArray[i]->wheelDirection.x, wheelParamArray[i]->wheelDirection.y, wheelParamArray[i]->wheelDirection.z);
				btVector3 childAxis(wheelParamArray[i]->wheelAxle.x, wheelParamArray[i]->wheelAxle.y, wheelParamArray[i]->wheelAxle.z);
				btScalar suspensionStiffness(wheelParamArray[i]->suspensionStiffness);
				btScalar suspensionDamping(wheelParamArray[i]->suspensionDamping);

				auto pConstraint = new btHinge2Constraint(*dynObject->GetRigidBody(), *wheelDynObject->GetRigidBody(), anchor, parentAxis, childAxis);

				pConstraint->setParam(BT_CONSTRAINT_CFM, 0.15f, 2);
				pConstraint->setParam(BT_CONSTRAINT_ERP, 0.35f, 2);

				pConstraint->setStiffness(2, suspensionStiffness);
				pConstraint->setDamping(2, suspensionDamping);

				if (wheelParamArray[i]->flags & PhysicWheel_Engine)
				{
					pConstraint->enableMotor(3, true);
					pConstraint->setMaxMotorForce(3, 0);
					pConstraint->setTargetVelocity(3, 0);
				}

				if (wheelParamArray[i]->flags & PhysicWheel_Steering)
				{
					pConstraint->enableMotor(5, true);
					pConstraint->setMaxMotorForce(5, 0);
					pConstraint->setTargetVelocity(5, 10);
					pConstraint->setServo(5, true);
					pConstraint->setServoTarget(5, 0);
				}
				else if (wheelParamArray[i]->flags & PhysicWheel_NoSteering)
				{
					pConstraint->setLimit(5, 0, 0);
				}

				obj->AddConstraint(pConstraint, m_dynamicsWorld, true);

				dynObject->GetVehicleManager()->SetConstraint(wheelParamArray[i]->index, pConstraint);
			}
		}	
	}

	return true;
}

void CPhysicsManager::SetPhysicPlayerConfig(PhysicPlayerConfigs *configs)
{
	m_playerMass = configs->mass;
	m_playerMaxPendingVelocity = configs->maxPendingVelocity;
}

bool CPhysicsManager::CreateSolidOptimizer(edict_t* ent, int boneindex, const Vector& halfext, const Vector& halfext2)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!ent->v.modelindex)
	{
		//Must have a model
		return false;
	}

	auto mod = (*sv_models)[ent->v.modelindex];

	if (!mod)
	{
		//Must have a model
		return false;
	}

	if (mod->type != mod_studio)
	{
		//Must be studio
		return false;
	}

	btVector3 boxSize(halfext.x, halfext.y, halfext.z);
	
	if (boxSize.x() <= 0 || boxSize.y() <= 0 || boxSize.z() <= 0)
	{
		//Must be valid size
		return false;
	}

	btVector3 boxSize2(halfext2.x, halfext2.y, halfext2.z);

	if (boxSize2.x() <= 0 || boxSize2.y() <= 0 || boxSize2.z() <= 0)
	{
		//Must be valid size
		return false;
	}

	if (1)
	{
		auto ghost = new CSolidOptimizerGhostPhysicObject(obj, boneindex, 0);

		ghost->SetGhostObject(new btPairCachingGhostObject());
		ghost->GetGhostObject()->setCollisionShape(new btBoxShape(boxSize));
		ghost->GetGhostObject()->setCollisionFlags(ghost->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		obj->AddPhysicObject(ghost, m_dynamicsWorld, &m_numDynamicObjects);
	}

	if (1)
	{
		auto ghost = new CSolidOptimizerGhostPhysicObject(obj, boneindex, 1);

		ghost->SetGhostObject(new btPairCachingGhostObject());
		ghost->GetGhostObject()->setCollisionShape(new btBoxShape(boxSize2));
		ghost->GetGhostObject()->setCollisionFlags(ghost->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		obj->AddPhysicObject(ghost, m_dynamicsWorld, &m_numDynamicObjects);
	}

	obj->AddSolidOptimizer(boneindex, 0);

	return true;
}

btBvhTriangleMeshShape *CPhysicsManager::CreateTriMeshShapeFromBrushModel(edict_t *ent)
{
	int modelindex = ent->v.modelindex;
	if (modelindex == -1)
	{
		return NULL;
	}

	auto mod = (*sv_models)[modelindex];

	if (!mod)
	{
		//Must have a model
		return NULL;
	}

	if (mod->type != mod_brush)
	{
		//Must be brush
		return NULL;
	}

	if (!m_brushIndexArray[modelindex])
	{
		return NULL;
	}

	auto vertexarray = m_worldVertexArray;
	auto indexarray = m_brushIndexArray[modelindex];

	if (!indexarray->vIndiceBuffer.size())
	{
		return NULL;
	}

	auto bulletVertexArray = new btTriangleIndexVertexArray(
		indexarray->vIndiceBuffer.size() / 3, indexarray->vIndiceBuffer.data(), 3 * sizeof(int),
		vertexarray->vVertexBuffer.size(), (float*)vertexarray->vVertexBuffer.data(), sizeof(brushvertex_t));

	auto meshShape = new btBvhTriangleMeshShape(bulletVertexArray, true, true);

	meshShape->setUserPointer(bulletVertexArray);

	return meshShape;
}

/*

Create a ghost object that fire pfnTouch event when colliding with physic objects

*/

bool CPhysicsManager::CreatePhysicTrigger(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	btCollisionShape *shape = CreateTriMeshShapeFromBrushModel(ent);

	if (!shape)
	{
		shape = new btBoxShape(btVector3((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f));
	}

	auto ghostobj = new CPhysicTriggerGhostPhysicObject(obj);

	ghostobj->SetGhostObject(new btPairCachingGhostObject());
	ghostobj->GetGhostObject()->setCollisionShape(shape);
	ghostobj->GetGhostObject()->setCollisionFlags(ghostobj->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	obj->AddPhysicObject(ghostobj, m_dynamicsWorld, &m_numDynamicObjects);

	return true;
}

btCollisionShape *CPhysicsManager::CreateCollisionShapeFromParams(CGameObject *obj, PhysicShapeParams *shapeParams)
{
	btCollisionShape *shape = NULL;

	switch (shapeParams->type)
	{
	case PhysicShape_Box:
	{
		shape = new btBoxShape(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		break;
	}
	case PhysicShape_Sphere:
	{
		shape = new btSphereShape(btScalar(shapeParams->size.x));
		break;
	}
	case PhysicShape_Capsule:
	{
		if (shapeParams->direction == PhysicShapeDirection_X)
			shape = new btCapsuleShapeX(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));
		else if (shapeParams->direction == PhysicShapeDirection_Y)
			shape = new btCapsuleShape(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));
		else if (shapeParams->direction == PhysicShapeDirection_Z)
			shape = new btCapsuleShapeZ(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));

		break;
	}
	case PhysicShape_Cylinder:
	{
		if (shapeParams->direction == PhysicShapeDirection_X)
			shape = new btCylinderShapeX(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		else if (shapeParams->direction == PhysicShapeDirection_Y)
			shape = new btCylinderShape(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		else if (shapeParams->direction == PhysicShapeDirection_Z)
			shape = new btCylinderShapeZ(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));

		break;
	}
	case PhysicShape_MultiSphere:
	{
		if (shapeParams->multispheres && shapeParams->multispheres->size() >= 4 && (shapeParams->multispheres->size() % 4) == 0)
		{
			std::vector<btVector3> vPointArray;
			std::vector<btScalar> vRadiusArray;

			size_t numSphere = shapeParams->multispheres->size() / 4;
			float *pdata = (float *)shapeParams->multispheres->data();

			for (size_t i = 0; i < numSphere; ++i)
			{
				btVector3 vPoint(pdata[i * 4 + 0], pdata[i * 4 + 1], pdata[i * 4 + 2]);

				btScalar vRadius = pdata[i * 4 + 3];

				vPointArray.emplace_back(vPoint);

				vRadiusArray.emplace_back(vRadius);
			}

			shape = new btMultiSphereShape(vPointArray.data(), vRadiusArray.data(), numSphere);
		}

		break;
	}
	}
	return shape;
}

bool CPhysicsManager::CreatePhysicObjectPost(edict_t *ent, CGameObject *obj, btCollisionShape *shape, PhysicObjectParams *objectParams)
{
	btVector3 localInertia;
	shape->calculateLocalInertia(objectParams->mass, localInertia);

	auto dynamicobj = CreateDynamicObject(obj, shape, localInertia, 
		objectParams->mass, 
		objectParams->linearfriction,
		objectParams->rollingfriction,
		objectParams->restitution,
		objectParams->ccdradius, 
		objectParams->ccdthreshold,
		objectParams->flags);

	btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

	dynamicobj->GetRigidBody()->setLinearVelocity(vecLinearVelocity);

	btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

	dynamicobj->GetRigidBody()->setAngularVelocity(vecALinearVelocity);

	ent->v.vuser1 = ent->v.velocity;
	ent->v.basevelocity = g_vecZero;
	ent->v.velocity = g_vecZero;
	ent->v.avelocity = g_vecZero;

	obj->AddPhysicObject(dynamicobj, m_dynamicsWorld, &m_numDynamicObjects);

	if (objectParams->flags & PhysicObject_HasImpactImpulse)
	{
		dynamicobj->SetImpactImpulseThreshold(objectParams->impactimpulse_threshold);
	}

	if (objectParams->flags & PhysicObject_Freeze)
	{
		dynamicobj->SetPhysicFreeze(true);
	}

	if (objectParams->flags & PhysicObject_HasClippingHull)
	{
		btCollisionShape * hullshape = NULL;

		if (objectParams->clippinghull_shapetype == PhysicShape_Box)
		{
			hullshape = new btBoxShape(btVector3(objectParams->clippinghull_size.x, objectParams->clippinghull_size.y, objectParams->clippinghull_size.z));
		}
		else if (objectParams->clippinghull_shapetype == PhysicShape_Cylinder)
		{
			if(objectParams->clippinghull_shapedirection == PhysicShapeDirection_X)
				hullshape = new btCylinderShapeX(btVector3(objectParams->clippinghull_size.x, objectParams->clippinghull_size.y, objectParams->clippinghull_size.z));
			else if (objectParams->clippinghull_shapedirection == PhysicShapeDirection_Y)
				hullshape = new btCylinderShape(btVector3(objectParams->clippinghull_size.x, objectParams->clippinghull_size.y, objectParams->clippinghull_size.z));
			else if (objectParams->clippinghull_shapedirection == PhysicShapeDirection_Z)
				hullshape = new btCylinderShapeZ(btVector3(objectParams->clippinghull_size.x, objectParams->clippinghull_size.y, objectParams->clippinghull_size.z));
		}

		if (hullshape)
		{
			float hullmass = 0.001f * objectParams->mass;

			btVector3 localInertiaHull;
			hullshape->calculateLocalInertia(hullmass, localInertiaHull);

			auto hullobj = CreateClippingHullObject(obj, hullshape, localInertiaHull, dynamicobj->GetRigidBody()->getWorldTransform(), hullmass);

			if (hullobj)
			{
				auto constraint = new btPoint2PointConstraint(*dynamicobj->GetRigidBody(), *hullobj->GetRigidBody(), btVector3(0, 0, 0), btVector3(0, 0, 0));

				obj->AddPhysicObject(hullobj, m_dynamicsWorld, &m_numDynamicObjects);

				obj->AddConstraint(constraint, m_dynamicsWorld, true);

				if (objectParams->flags & PhysicObject_Freeze)
				{
					hullobj->SetPhysicFreeze(true);
				}
			}
		}
	}

	return true;
}

bool CPhysicsManager::CreateCompoundPhysicObject(edict_t* ent, PhysicShapeParams *shapeParams, size_t numShapeParams, PhysicObjectParams *objectParams)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!ent->v.modelindex)
	{
		//Must have a model
		return false;
	}

	auto mod = (*sv_models)[ent->v.modelindex];

	if (!mod)
	{
		//Must have a model
		return false;
	}

	if (mod->type != mod_studio && mod->type != mod_brush)
	{
		//Must be studio or brush
		return false;
	}

	//Invalid parameters
	if (objectParams->mass <= 0)
		return false;

	btCompoundShape *compound = new btCompoundShape();

	for (size_t i = 0; i < numShapeParams; ++i)
	{
		btCollisionShape *shape = CreateCollisionShapeFromParams(obj, &shapeParams[i]);

		if (shape)
		{
			btTransform trans;
			trans.setIdentity();

			EulerMatrix(btVector3(shapeParams->angles.x, shapeParams->angles.y, shapeParams->angles.z), trans.getBasis());

			trans.setOrigin(btVector3(shapeParams->origin.x, shapeParams->origin.y, shapeParams->origin.z));

			compound->addChildShape(trans, shape);
		}
	}

	//Invalid shape
	if (!compound->getNumChildShapes())
	{
		delete compound;
		return false;
	}

	return CreatePhysicObjectPost(ent, obj, compound, objectParams);
}

bool CPhysicsManager::CreatePhysicObject(edict_t* ent, PhysicShapeParams *shapeParams, PhysicObjectParams *objectParams)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	if (!ent->v.modelindex)
	{
		//Must have a model
		return false;
	}

	auto mod = (*sv_models)[ent->v.modelindex];

	if (!mod)
	{
		//Must have a model
		return false;
	}

	if (mod->type != mod_studio && mod->type != mod_brush)
	{
		//Must be studio or brush
		return false;
	}

	//Invalid parameters
	if (objectParams->mass <= 0)
		return false;

	btCollisionShape *shape = CreateCollisionShapeFromParams(obj, shapeParams);

	//Invalid shape
	if (!shape)
		return false;

	return CreatePhysicObjectPost(ent, obj, shape, objectParams);
}

void CPhysicsManager::PreSpawn(edict_t *ent)
{
	m_simrate = 1 / 60.0f;
}

void CPhysicsManager::PostSpawn(edict_t *ent)
{
	m_maxIndexGameObject = 0;
	m_gameObjects.resize(gpGlobals->maxEntities);

	r_worldentity = ent;

	r_worldmodel = EngineGetPrecachedModelByIndex(r_worldentity->v.modelindex);

	std::vector<glpoly_t*> glpolys;
	GenerateWorldVerticeArray(glpolys);
	GenerateBrushIndiceArray(glpolys);

	CreateBrushModel(r_worldentity);
}

struct GameFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const
	{
		bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
	
		if (collides)
		{
			auto body0 = (btCollisionObject *)proxy0->m_clientObject;
			auto body1 = (btCollisionObject *)proxy1->m_clientObject;

			auto physobj0 = (CPhysicObject *)body0->getUserPointer();
			auto physobj1 = (CPhysicObject *)body1->getUserPointer();

			if (physobj0->UseEdictSolid())
			{
				auto ent0 = physobj0->GetGameObject()->GetEdict();

				if(ent0->v.solid <= SOLID_TRIGGER)
					return false;
			}

			if (physobj1->UseEdictSolid())
			{
				auto ent1 = physobj1->GetGameObject()->GetEdict();

				if (ent1->v.solid <= SOLID_TRIGGER)
					return false;
			}

			if (physobj0->IsPlayerObject())
			{
				auto ent0 = physobj0->GetGameObject()->GetEdict();
				auto playerobj0 = (CPlayerObject *)physobj0;
				if(playerobj0->IsDuck() && !ent0->v.bInDuck)
					return false;
				else if (!playerobj0->IsDuck() && ent0->v.bInDuck)
					return false;
			}
			else if (physobj1->IsPlayerObject())
			{
				auto ent1 = physobj1->GetGameObject()->GetEdict();
				auto playerobj1 = (CPlayerObject *)physobj1;
				if (playerobj1->IsDuck() && !ent1->v.bInDuck)
					return false;
				else if (!playerobj1->IsDuck() && ent1->v.bInDuck)
					return false;
			}

			if (physobj0->IsSolidOptimizerGhost() && physobj1->IsPlayerObject())
			{
				auto optimizer0 = (CSolidOptimizerGhostPhysicObject *)physobj0;
				auto player1 = (CPlayerObject *)physobj1;

				btTransform body0_worldTrans = optimizer0->GetGhostObject()->getWorldTransform();
				btTransform player1_worldTrans = player1->GetRigidBody()->getWorldTransform();
				btVector3 player1_velocity = player1->GetRigidBody()->getLinearVelocity();
				btVector3 origin_diff = body0_worldTrans.getOrigin() - player1_worldTrans.getOrigin();
				origin_diff.normalize();
				if (player1_velocity.dot(origin_diff) > 350)
				{
					if (optimizer0->GetOptimizerType() == 0)
						return false;
				}
				else
				{
					if (optimizer0->GetOptimizerType() == 1)
						return false;
				}
			}
		}
		return collides;
	}
};

static void InternalTickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	auto dispatcher = world->getDispatcher();

	int numManifolds = dispatcher->getNumManifolds();

	for (int i = 0; i < numManifolds; i++)
	{
		auto manifold = dispatcher->getManifoldByIndexInternal(i);
		if (!manifold->getNumContacts())
			continue;

		auto body0 = manifold->getBody0();
		auto body1 = manifold->getBody1();

		auto physobj0 = (CPhysicObject *)body0->getUserPointer();
		auto physobj1 = (CPhysicObject *)body1->getUserPointer();

		if (physobj0->IsDynamicObject() && (physobj1->IsPlayerObject() || physobj1->IsStaticObject() || physobj1->IsDynamicObject() || physobj1->IsClippingHullObject()))
		{
			auto ent0 = physobj0->GetGameObject()->GetEdict();
			auto ent1 = physobj1->GetGameObject()->GetEdict();

			for (int p = 0; p < manifold->getNumContacts(); p++)
			{
				auto &cp = manifold->getContactPoint(p);

				float flImpulse = cp.getAppliedImpulse() / timeStep;
				
				physobj0->DispatchImpact(flImpulse, cp.getPositionWorldOnA(), cp.getPositionWorldOnB(), cp.m_normalWorldOnB, ent1);

				if (physobj1->IsPlayerObject())
					physobj1->DispatchImpact(flImpulse, cp.getPositionWorldOnB(), cp.getPositionWorldOnA(), cp.m_normalWorldOnB, ent0);
			}
		}
		else if (physobj1->IsDynamicObject() && (physobj0->IsPlayerObject() || physobj0->IsStaticObject() || physobj0->IsDynamicObject() || physobj1->IsClippingHullObject()))
		{
			auto ent0 = physobj0->GetGameObject()->GetEdict();
			auto ent1 = physobj1->GetGameObject()->GetEdict();

			for (int p = 0; p < manifold->getNumContacts(); p++)
			{
				auto &cp = manifold->getContactPoint(p);

				float flImpulse = cp.getAppliedImpulse() / timeStep;
				
				physobj1->DispatchImpact(flImpulse, cp.getPositionWorldOnB(), cp.getPositionWorldOnA(), cp.m_normalWorldOnB, ent0);

				if (physobj0->IsPlayerObject())
					physobj0->DispatchImpact(flImpulse, cp.getPositionWorldOnB(), cp.getPositionWorldOnA(), cp.m_normalWorldOnB, ent1);
			}
		}
	}
}

static bool CustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
	auto body0 = colObj0Wrap->getCollisionObject();
	auto body1 = colObj1Wrap->getCollisionObject();

	auto physobj0 = (CPhysicObject *)body0->getUserPointer();
	auto physobj1 = (CPhysicObject *)body1->getUserPointer();

	if (physobj0->IsPlayerObject() || physobj1->IsPlayerObject())
		return true;

	if (physobj0->IsStaticObject())
	{
		auto ent0 = physobj0->GetGameObject()->GetEdict();

		if (ent0->v.flags & FL_CONVEYOR)
		{
			cp.m_contactPointFlags |= BT_CONTACT_FLAG_LATERAL_FRICTION_INITIALIZED;

			///choose a target velocity in the friction dir1 direction, for a conveyor belt effect
			cp.m_lateralFrictionDir1.setValue(ent0->v.movedir.x, ent0->v.movedir.y, ent0->v.movedir.z);

			cp.m_contactMotion1 = ent0->v.speed;
		}
	}
	else if (physobj1->IsStaticObject())
	{
		auto ent1 = physobj1->GetGameObject()->GetEdict();

		if (ent1->v.flags & FL_CONVEYOR)
		{
			cp.m_contactPointFlags |= BT_CONTACT_FLAG_LATERAL_FRICTION_INITIALIZED;

			///choose a target velocity in the friction dir1 direction, for a conveyor belt effect
			cp.m_lateralFrictionDir1.setValue(ent1->v.movedir.x, ent1->v.movedir.y, ent1->v.movedir.z);

			cp.m_contactMotion1 = ent1->v.speed;
		}
	}

	return true;
}


void CPhysicsManager::Init(void)
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	m_overlapFilterCallback = new GameFilterCallback();
	m_dynamicsWorld->getPairCache()->setOverlapFilterCallback(m_overlapFilterCallback);

	m_ghostPairCallback = new btGhostPairCallback();
	m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	m_dynamicsWorld->setGravity(btVector3(0, 0, 0));

	m_dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_ENABLE_FRICTION_DIRECTION_CACHING;

	gContactAddedCallback = CustomMaterialCombinerCallback;
	m_dynamicsWorld->setInternalTickCallback(InternalTickCallback);
}

void CPhysicsManager::Shutdown(void)
{
	RemoveAllGameBodies();

	for (size_t i = 0; i < m_brushIndexArray.size(); ++i)
	{
		if (m_brushIndexArray[i])
		{
			delete m_brushIndexArray[i];
			m_brushIndexArray[i] = NULL;
		}
	}

	m_brushIndexArray.clear();

	if (m_worldVertexArray) {
		delete m_worldVertexArray;
		m_worldVertexArray = NULL;
	}

	if (m_dynamicsWorld)
	{
		delete m_dynamicsWorld;
		m_dynamicsWorld = NULL;
	}

	if (m_overlapFilterCallback)
	{
		delete m_overlapFilterCallback;
		m_overlapFilterCallback = NULL;
	}
	if (m_ghostPairCallback)
	{
		delete m_ghostPairCallback;
		m_ghostPairCallback = NULL;
	}

	if (m_collisionConfiguration)
	{
		delete m_collisionConfiguration;
		m_collisionConfiguration = NULL;
	}
	if (m_dispatcher)
	{
		delete m_dispatcher;
		m_dispatcher = NULL;
	}
	if (m_overlappingPairCache)
	{
		delete m_overlappingPairCache;
		m_overlappingPairCache = NULL;
	}
	if (m_solver)
	{
		delete m_solver;
		m_solver = NULL;
	}
}

void CPhysicsManager::StepSimulation(double frametime)
{
	if (!gPhysicsManager.GetNumDynamicBodies())
		return;

	m_dynamicsWorld->stepSimulation((btScalar)frametime, 2, m_simrate);
}

void CPhysicsManager::SetSimRate(float rate)
{
	m_simrate = rate;
}

void CPhysicsManager::SetGravity(float velocity)
{
	m_gravity = -velocity;

	m_dynamicsWorld->setGravity(btVector3(0, 0, m_gravity));
}

int CPhysicsManager::GetSolidPlayerMask()
{
	return m_solidPlayerMask;
}

int CPhysicsManager::GetNumDynamicBodies()
{
	return m_numDynamicObjects;
}

CGameObject *CPhysicsManager::GetGameObject(int entindex)
{
	if (entindex < 0 || entindex >= (int)m_gameObjects.size())
		return NULL;

	return m_gameObjects[entindex];
}

CGameObject* CPhysicsManager::GetGameObject(edict_t *ent)
{
	return GetGameObject(g_engfuncs.pfnIndexOfEdict(ent));
}

void CPhysicsManager::RemoveGameObject(int entindex)
{
	if (entindex >= (int)m_gameObjects.size())
		return;

	auto obj = m_gameObjects[entindex];

	if (obj)
	{
		obj->RemoveAllConstraints(m_dynamicsWorld, &m_numDynamicObjects);
		obj->RemoveAllPhysicObjects(m_dynamicsWorld, &m_numDynamicObjects);

		delete obj;

		m_gameObjects[entindex] = NULL;
	}
}

void CPhysicsManager::RemoveAllGameBodies()
{
	for (int i = 0;i <= m_maxIndexGameObject; ++i)
	{
		RemoveGameObject(i);
	}

	m_gameObjects.clear();
	m_maxIndexGameObject = 0;
}

bool CPhysicsManager::IsEntitySuperPusher(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (obj && obj->IsSuperPusherEnabled())
	{
		return true;
	}

	return false;
}

void CGameObject::StartFrame(btDiscreteDynamicsWorld* world)
{
	if (IsSolidOptimizerEnabled())
	{
		SetSemiClipMask(gPhysicsManager.GetSolidPlayerMask());
	}

	for (size_t i = 0; i < m_physics.size(); ++i)
	{
		m_physics[i]->StartFrame(world);
	}
}

void CGameObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	auto ent = GetEdict();

	for (size_t i = 0; i < m_physics.size(); ++i)
	{
		m_physics[i]->StartFrame_Post(world);
	}

	if (HasEntityCustomMoveSize())
	{
		m_backup_mins = ent->v.mins;
		m_backup_maxs = ent->v.maxs;
		ent->v.mins = m_custom_movemins;
		ent->v.maxs = m_custom_movemaxs;
	}
}

void CGameObject::EndFrame(btDiscreteDynamicsWorld* world)
{
	auto ent = GetEdict();

	if (HasEntityCustomMoveSize())
	{
		ent->v.mins = m_backup_mins;
		ent->v.maxs = m_backup_maxs;
	}

	if (gPhysicsManager.GetNumDynamicBodies() > 0)
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			m_physics[i]->EndFrame(world);
		}
	}

	if (m_anim_flags & EnvStudioAnim_AnimatedStudio)
	{
		StudioFrameAdvance();

		UpdateEnvStudioKeyframeAnim();
	}
	else if (m_anim_flags & EnvStudioAnim_AnimatedSprite)
	{
		SpriteFrameAdvance();

		UpdateEnvStudioKeyframeAnim();
	}

	if (m_follow_ent)
	{
		ApplyEntityFollow();
	}
}

void CGameObject::SetEntityFollow(edict_t* follow, int flags, const Vector &origin_offset, const Vector &angles_offset)
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

void CGameObject::SetEntityEnvStudioAnim(int flags, float overrideCurFrame, float overrideMaxFrame, EnvStudioKeyframe **keyframes, size_t numKeyframes)
{
	auto ent = GetEdict();

	m_anim_flags = flags;
	m_anim_curframe = 0;
	m_anim_lasttime = gpGlobals->time;
	m_anim_maxframe = 0;

	if (flags & EnvStudioAnim_AnimatedStudio)
	{
		m_anim_maxframe = 256;
	}
	else  if (flags & EnvStudioAnim_AnimatedSprite)
	{
		for (size_t i = 0; i < numKeyframes; ++i)
		{
			m_anim_keyframes.emplace_back(*keyframes[i]);

			if (keyframes[i]->frame > m_anim_maxframe)
				m_anim_maxframe = keyframes[i]->frame;
		}
	}

	if (overrideCurFrame > 0)
		m_anim_curframe = overrideCurFrame;

	if (overrideMaxFrame > 0)
		m_anim_maxframe = overrideMaxFrame;

	if (flags & EnvStudioAnim_AnimatedStudio)
	{
		auto pmodel = GET_MODEL_PTR(ent);

		studiohdr_t *pstudiohdr = (studiohdr_t *)pmodel;

		if (pstudiohdr)
		{
			if (ent->v.sequence >= pstudiohdr->numseq)
			{
				m_anim_studio_frametime = 0;
				m_anim_studio_seqflags = 0;
			}
			else
			{
				mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)ent->v.sequence;

				m_anim_studio_seqflags = pseqdesc->flags;

				if (pseqdesc->numframes > 1)
				{
					m_anim_studio_frametime = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
				}
				else
				{
					m_anim_studio_frametime = 256;
				}
			}
		}
	}

}

void CGameObject::SpriteFrameAdvance()
{
	m_anim_curframe += GetEdict()->v.framerate * (gpGlobals->time - m_anim_lasttime);

	if (m_anim_curframe > m_anim_maxframe)
		m_anim_curframe = m_anim_maxframe;

	m_anim_lasttime = gpGlobals->time;
}

void CGameObject::StudioFrameAdvance()
{
	auto ent = GetEdict();

	float flInterval = (gpGlobals->time - m_anim_lasttime);

	if (flInterval <= 0.001)
	{
		m_anim_lasttime = gpGlobals->time;
		return;
	}

	if (!m_anim_lasttime)
		flInterval = 0;

	m_anim_curframe += flInterval * m_anim_studio_frametime * ent->v.framerate;
	m_anim_lasttime = gpGlobals->time;

	if (m_anim_curframe < 0 || m_anim_curframe >= 256)
	{
		if (m_anim_studio_seqflags & STUDIO_LOOPING)
			m_anim_curframe -= (int)(m_anim_curframe / 256.0f) * 256.0f;
		else
			m_anim_curframe = (m_anim_curframe < 0) ? 0.0f : 255.0f;

		if (m_anim_flags & EnvStudioAnim_RemoveOnAnimFinished)
		{
			ent->v.flags |= FL_KILLME;
		}
	}
}

void CGameObject::UpdateEnvStudioKeyframeAnim()
{
	auto ent = GetEdict();

	if (m_anim_keyframes.size() > 0)
	{
		for (int i = 1; i < int(m_anim_keyframes.size()); ++i)
		{
			float startframe = m_anim_keyframes[i - 1].frame;
			float endframe = m_anim_keyframes[i].frame;
			float totalframe = endframe - startframe;
			if (m_anim_curframe >= startframe && m_anim_curframe < endframe)
			{
				float factor = (m_anim_curframe - startframe) / totalframe;

				if (m_anim_flags & EnvStudioAnim_AnimatedRenderAmt)
				{
					ent->v.renderamt = (1.0f - factor) * m_anim_keyframes[i - 1].renderamt + factor * m_anim_keyframes[i].renderamt;
				}

				if (m_anim_flags & EnvStudioAnim_AnimatedScale)
				{
					ent->v.scale = (1.0f - factor) * m_anim_keyframes[i - 1].scale + factor * m_anim_keyframes[i].scale;
				}

				break;
			}
		}
	}

	if (m_anim_flags & EnvStudioAnim_RemoveOnBoundExcceeded)
	{
		if (ent->v.origin.x > ent->v.vuser2.x || ent->v.origin.y > ent->v.vuser2.y || ent->v.origin.z > ent->v.vuser2.z ||
			ent->v.origin.x < ent->v.vuser1.x || ent->v.origin.y < ent->v.vuser1.y || ent->v.origin.z < ent->v.vuser1.z)
		{
			ent->v.flags |= FL_KILLME;
		}
	}
	else if (m_anim_curframe >= m_anim_maxframe)
	{
		if (m_anim_flags & EnvStudioAnim_RemoveOnAnimFinished)
		{
			ent->v.flags |= FL_KILLME;
		}
	}
	else if (!(m_anim_flags & EnvStudioAnim_StaticFrame))
	{
		ent->v.frame = m_anim_curframe;

		if (m_anim_flags & EnvStudioAnim_AnimatedStudio)
		{
			ent->v.animtime = m_anim_lasttime;
		}
	}
}

void CGameObject::ApplyEntityFollow()
{
	if (m_follow_ent->free || (m_follow_ent->v.flags & FL_KILLME))
	{
		SetEntityFollow(NULL, 0, g_vecZero, g_vecZero);
	}
	else
	{
		auto ent = GetEdict();
		if (m_follow_flags & FollowEnt_CopyOriginX)
		{
			ent->v.origin.x = m_follow_ent->v.origin.x + m_follow_origin_offet.x;
		}
		if (m_follow_flags & FollowEnt_CopyOriginY)
		{
			ent->v.origin.y = m_follow_ent->v.origin.y + m_follow_origin_offet.y;
		}
		if (m_follow_flags & FollowEnt_CopyOriginZ)
		{
			ent->v.origin.z = m_follow_ent->v.origin.z + m_follow_origin_offet.z;
		}
		if (m_follow_flags & FollowEnt_CopyAnglesP)
		{
			ent->v.angles.x = m_follow_ent->v.angles.x + m_follow_angles_offet.x;
		}
		if (m_follow_flags & FollowEnt_CopyAnglesY)
		{
			ent->v.angles.y = m_follow_ent->v.angles.y + m_follow_angles_offet.y;
		}
		if (m_follow_flags & FollowEnt_CopyAnglesR)
		{
			ent->v.angles.z = m_follow_ent->v.angles.z + m_follow_angles_offet.z;
		}
		if (m_follow_flags & FollowEnt_CopyNoDraw)
		{
			if ((ent->v.effects & EF_NODRAW) && !(m_follow_ent->v.effects & EF_NODRAW))
			{
				ent->v.effects &= ~EF_NODRAW;
			}
			else if (!(GetEdict()->v.effects & EF_NODRAW) && (m_follow_ent->v.effects & EF_NODRAW))
			{
				ent->v.effects |= EF_NODRAW;
			}
		}
		if (m_follow_flags & FollowEnt_CopyRenderMode)
		{
			ent->v.rendermode = m_follow_ent->v.rendermode;
		}
		if (m_follow_flags & FollowEnt_CopyRenderAmt)
		{
			ent->v.renderamt = m_follow_ent->v.renderamt;
		}
		if (m_follow_flags & FollowEnt_ApplyLinearVelocity)
		{
			m_follow_origin_offet = m_follow_origin_offet + ent->v.velocity * (float)(*host_frametime);
		}
		if (m_follow_flags & FollowEnt_ApplyAngularVelocity)
		{
			m_follow_angles_offet = m_follow_angles_offet + ent->v.avelocity * (float)(*host_frametime);
		}
	}
}


void CGameObject::ApplyLevelOfDetail(float distance, int *body, int *modelindex, float *scale)
{
	if (m_lod_distance3 > 0 && distance > m_lod_distance3)
	{
		if ((m_lod_flags & LOD_BODY) && m_lod_body3 >= 0)
			*body = m_lod_body3;
		else if ((m_lod_flags & LOD_MODELINDEX) && m_lod_body3 >= 0)
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

bool CGameObject::AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player)
{
	if (GetSemiVisibleMask() != 0)
	{
		int hostindex = g_engfuncs.pfnIndexOfEdict(host);

		if ((GetSemiVisibleMask() & (1 << (hostindex - 1))) == 0)
		{
			return false;
		}
	}

	if (GetSemiClipMask() != 0)
	{
		int hostindex = g_engfuncs.pfnIndexOfEdict(host);

		if ((GetSemiClipMask() & (1 << (hostindex - 1))) != 0)
		{
			state->solid = SOLID_NOT;
		}
	}

	if (GetLevelOfDetailFlags() != 0)
	{
		auto viewent = GetClientViewEntity(host);

		if (viewent)
		{
			float distance = (ent->v.origin - viewent->v.origin).Length();

			ApplyLevelOfDetail(distance, &state->body, &state->modelindex, &state->scale);
		}
	}

	return true;
}

edict_t* CPhysicsManager::GetCurrentImpactEntity(Vector *vecImpactPoint, Vector *vecImpactDirection, float *flImpactImpulse)
{
	if (vecImpactPoint)
		*vecImpactPoint = m_CurrentImpactPoint;
	if (vecImpactDirection)
		*vecImpactDirection = m_CurrentImpactDirection;
	if (flImpactImpulse)
		*flImpactImpulse = m_CurrentImpactImpulse;

	return m_CurrentImpactEntity;
}

void CPhysicsManager::SetCurrentImpactEntity(edict_t* pImpactEntity, const Vector &vecImpactPoint, const Vector &vecImpactDirection, float flImpactImpulse)
{
	m_CurrentImpactEntity = pImpactEntity;
	m_CurrentImpactPoint = vecImpactPoint;
	m_CurrentImpactDirection = vecImpactDirection;
	m_CurrentImpactImpulse = flImpactImpulse;
}