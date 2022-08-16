#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <algorithm>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"

#include "physics.h"

//8192 for SvEngine and 512 for GoldSrc
int EngineGetMaxPrecacheModel(void)
{
	return 8192;
}

model_t* EngineGetPrecachedModelByIndex(int i)
{
	return (*sv_models)[i];
}

cvar_t bv_tickrate = {
	(char*)"bv_tickrate",
	(char*)"64",
	FCVAR_SERVER,
	64
};

cvar_t* bv_simrate = &bv_tickrate;

cvar_t bv_worldscale = {
	(char*)"bv_worldscale",
	(char*)"1",
	FCVAR_SERVER,
	1
};

cvar_t* bv_scale = &bv_worldscale;

btScalar G2BScale = 1;
btScalar B2GScale = 1 / G2BScale;

extern edict_t* r_worldentity;
extern model_t* r_worldmodel;

//GoldSrcToBullet Scaling

void FloatGoldSrcToBullet(float* trans)
{
	(*trans) *= G2BScale;
}

void TransformGoldSrcToBullet(btTransform& trans)
{
	auto& org = trans.getOrigin();

	org.m_floats[0] *= G2BScale;
	org.m_floats[1] *= G2BScale;
	org.m_floats[2] *= G2BScale;
}

void Vec3GoldSrcToBullet(vec3_t vec)
{
	vec[0] *= G2BScale;
	vec[1] *= G2BScale;
	vec[2] *= G2BScale;
}

void Vector3GoldSrcToBullet(btVector3& vec)
{
	vec.m_floats[0] *= G2BScale;
	vec.m_floats[1] *= G2BScale;
	vec.m_floats[2] *= G2BScale;
}

//BulletToGoldSrc Scaling

void TransformBulletToGoldSrc(btTransform& trans)
{
	trans.getOrigin().m_floats[0] *= B2GScale;
	trans.getOrigin().m_floats[1] *= B2GScale;
	trans.getOrigin().m_floats[2] *= B2GScale;
}

void Vec3BulletToGoldSrc(vec3_t vec)
{
	vec.x *= B2GScale;
	vec.y *= B2GScale;
	vec.z *= B2GScale;
}

void Vector3BulletToGoldSrc(btVector3& vec)
{
	vec.m_floats[0] *= B2GScale;
	vec.m_floats[1] *= B2GScale;
	vec.m_floats[2] *= B2GScale;
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
				Vec3GoldSrcToBullet(Vertexes[j].pos);
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
				Vec3GoldSrcToBullet(Vertexes[2].pos);

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

CDynamicObject* CPhysicsManager::CreateDynamicObject(CGameObject *obj, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold,
	bool pushable, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	//Dynamic object collide with all other stuffs when pushable, and all non-player stuffs when unpushable

	int mask = btBroadphaseProxy::AllFilter & (~btBroadphaseProxy::SensorTrigger);

	if (!pushable)
		mask &= ~FallGuysCollisionFilterGroups::PlayerFilter;

	auto dynamicobj = new CDynamicObject(
		obj,
		btBroadphaseProxy::DefaultFilter,
		mask,
		mass, pushable);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(dynamicobj), collisionShape, localInertia);
	cInfo.m_friction = friction;
	cInfo.m_rollingFriction = rollingFriction;
	cInfo.m_restitution = restitution;
	cInfo.m_linearSleepingThreshold = 0.1f;
	cInfo.m_angularSleepingThreshold = 0.001f;

	dynamicobj->SetRigidBody(new btRigidBody(cInfo));

	dynamicobj->GetRigidBody()->setCcdSweptSphereRadius(G2BScale * ccdRadius);
	dynamicobj->GetRigidBody()->setCcdMotionThreshold(G2BScale * ccdThreshold);

	return dynamicobj;
}

CStaticObject* CPhysicsManager::CreateStaticObject(CGameObject *obj, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic)
{
	if (!indexarray->vIndiceBuffer.size())
	{
		//todo: maybe use clipnode?
		auto staticobj = new CStaticObject(
			obj,
			btBroadphaseProxy::StaticFilter, 
			btBroadphaseProxy::AllFilter & ~(btBroadphaseProxy::SensorTrigger | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter),
			vertexarray, indexarray, kinematic);

		return staticobj;
	}

	auto staticobj = new CStaticObject(
		obj,
		btBroadphaseProxy::StaticFilter,
		btBroadphaseProxy::AllFilter & ~(btBroadphaseProxy::SensorTrigger | btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter),
		vertexarray, indexarray, kinematic);

	auto bulletVertexArray = new btTriangleIndexVertexArray(
		indexarray->vIndiceBuffer.size() / 3, indexarray->vIndiceBuffer.data(), 3 * sizeof(int),
		vertexarray->vVertexBuffer.size(), (float*)vertexarray->vVertexBuffer.data(), sizeof(brushvertex_t));

	auto meshShape = new btBvhTriangleMeshShape(bulletVertexArray, true, true);

	meshShape->setUserPointer(bulletVertexArray);

	btMotionState* motionState = NULL;

	if (kinematic)
		motionState = new EntityMotionState(staticobj);
	else
		motionState = new btDefaultMotionState();

	btRigidBody::btRigidBodyConstructionInfo cInfo(0, motionState, meshShape);
	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;

	staticobj->SetRigidBody(new btRigidBody(cInfo));

	if (kinematic)
	{
		staticobj->GetRigidBody()->setCollisionFlags(staticobj->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		staticobj->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	}

	return staticobj;
}

CPlayerObject* CPhysicsManager::CreatePlayerObject(CGameObject *obj, float mass, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	//Player only collides with pushable objects, and world..? (do we really need to collide with world ?)
	auto playerobj = new CPlayerObject(
		obj,
		btBroadphaseProxy::DefaultFilter | FallGuysCollisionFilterGroups::PlayerFilter,
		btBroadphaseProxy::AllFilter & ~(FallGuysCollisionFilterGroups::PlayerFilter),
		mass);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(playerobj), collisionShape, localInertia);

	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 0;

	playerobj->SetRigidBody(new btRigidBody(cInfo));

	return playerobj;
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
	out_euler[0] = btAsin(in_matrix[2][0]);

	if (btFabs(in_matrix[2][0]) < (1 - 0.001f)) {
		out_euler[1] = btAtan2(in_matrix[1][0], in_matrix[0][0]);
		out_euler[2] = btAtan2(in_matrix[2][1], in_matrix[2][2]);
	}
	else {
		out_euler[1] = btAtan2(in_matrix[1][2], in_matrix[1][1]);
		out_euler[2] = 0;
	}

	out_euler *= SIMD_DEGS_PER_RAD;
}

//Upload GoldSrc origin and angles to bullet engine

void EntityMotionState::getWorldTransform(btTransform& worldTrans) const
{
	if (!GetPhysicObject()->GetGameObject())
		return;

	auto ent = GetPhysicObject()->GetGameObject()->GetEdict();

	if (!ent)
		return;

	//Player and brush upload origin and angles in normal way
	if (!GetPhysicObject()->IsDynamic())
	{
		btVector3 GoldSrcOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

		Vector3GoldSrcToBullet(GoldSrcOrigin);

		worldTrans = btTransform(btQuaternion(0, 0, 0, 1), GoldSrcOrigin);

		vec3_t GoldSrcAngles = ent->v.angles;

		if (GetPhysicObject()->IsPlayer())
		{
			GoldSrcAngles.x = 0;
			GoldSrcAngles.y = 0;
			GoldSrcAngles.z = 0;
		}

		btVector3 angles;
		GoldSrcAngles.CopyToArray(angles.m_floats);
		EulerMatrix(angles, worldTrans.getBasis());
	}
	else
	{
		if (m_worldTransformInitialized)
		{
			worldTrans = m_worldTransform;
		}
		else
		{
			btVector3 GoldSrcOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

			Vector3GoldSrcToBullet(GoldSrcOrigin);

			worldTrans = btTransform(btQuaternion(0, 0, 0, 1), GoldSrcOrigin);

			vec3_t GoldSrcAngles = ent->v.angles;

			btVector3 angles;
			GoldSrcAngles.CopyToArray(angles.m_floats);
			EulerMatrix(angles, worldTrans.getBasis());

			m_worldTransform = worldTrans;
			m_worldTransformInitialized = true;
		}
	}
}

//Download GoldSrc origin and angles from bullet engine

void EntityMotionState::setWorldTransform(const btTransform& worldTrans)
{
	//Never download player state
	if (GetPhysicObject()->IsPlayer())
	{
		return;
	}

	if (!GetPhysicObject()->GetGameObject())
		return;

	auto ent = GetPhysicObject()->GetGameObject()->GetEdict();

	if (!ent)
		return;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	Vec3BulletToGoldSrc(origin);

	btVector3 btAngles;
	MatrixEuler(worldTrans.getBasis(), btAngles);
	Vector angles = Vector((float*)btAngles.m_floats);

	//Clamp to -3600~3600
	for (int i = 0; i < 3; i++)
	{
		if (angles[i] < -3600.0f || angles[i] > 3600.0f)
			angles[i] = fmod(angles[i], 3600.0f);
	}

	SET_ORIGIN(ent, origin);
	ent->v.angles = angles;
}

void CPhysicsManager::EntityStartFrame()
{
	m_solidPlayerMask = 0;

	for (int i = 1; i < gpGlobals->maxEntities; ++i)
	{
		auto ent = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!ent)
			continue;

		auto obj = m_gameObjects[i];

		if (!obj)
		{
			if (ent->free)
				continue;

			if (IsEntitySolidPusher(ent))
			{
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

		if (!ent)
			continue;

		if (ent->free)
			continue;

		auto body = m_gameObjects[i];

		if (body)
		{
			body->StartFrame_Post(m_dynamicsWorld);
		}
	}
}

void CDynamicObject::StartFrame(btDiscreteDynamicsWorld* world)
{

}

void CDynamicObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	//Download linear velocity from bullet engine

	auto vecVelocity = GetRigidBody()->getLinearVelocity();

	auto ent = GetGameObject()->GetEdict();

	Vector vel(vecVelocity.getX(), vecVelocity.getY(), vecVelocity.getZ());

	Vec3BulletToGoldSrc(vel);

	//auto vecAVelocity = GetRigidBody()->getAngularVelocity();

	//Vector avel(vecAVelocity.getX() * SIMD_DEGS_PER_RAD, vecAVelocity.getY() * SIMD_DEGS_PER_RAD, vecAVelocity.getZ() * SIMD_DEGS_PER_RAD);

	ent->v.velocity = g_vecZero;
	ent->v.avelocity = g_vecZero;
	ent->v.vuser1 = vel;
}

bool CDynamicObject::SetAbsBox(edict_t *ent)
{
	btVector3 aabbMins, aabbMaxs;

	GetRigidBody()->getAabb(aabbMins, aabbMaxs);

	Vector3BulletToGoldSrc(aabbMins);
	Vector3BulletToGoldSrc(aabbMaxs);

	ent->v.absmin.x = aabbMins.getX();
	ent->v.absmin.y = aabbMins.getY();
	ent->v.absmin.z = aabbMins.getZ();
	ent->v.absmax.x = aabbMaxs.getX();
	ent->v.absmax.y = aabbMaxs.getY();
	ent->v.absmax.z = aabbMaxs.getZ();

	//additional 1 unit ?
	ent->v.absmin.x -= 1;
	ent->v.absmin.y -= 1;
	ent->v.absmin.z -= 1;
	ent->v.absmax.x += 1;
	ent->v.absmax.y += 1;
	ent->v.absmax.z += 1;

	return true;
}

int CDynamicObject::PM_CheckStuck(int hitent, physent_t *blocker, vec3_t *impactvelocity)
{
	int result = 0;

	vec3_t move;

	move = pmove->origin - blocker->origin;

	move.z += (blocker->maxs.z - blocker->mins.z) * 0.5f;

	move = move.Normalize();

	move = move * (1000.0f * pmove->frametime);

	auto backup_origin = pmove->origin;

	int original_solid = blocker->solid;

	blocker->solid = SOLID_NOT;

	pmove->origin = pmove->origin + move;

	pmtrace_t trace2 = { 0 };
	auto hitent2 = pmove->PM_TestPlayerPosition(pmove->origin, &trace2);
	if (hitent2 != -1 && hitent2 != hitent)
	{
		//Blocked, don't move
		pmove->origin = backup_origin;

		result = 2;
	}
	else
	{
		result = 1;
	}

	blocker->solid = original_solid;

	return result;
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
	Vector3GoldSrcToBullet(vecVelocity);
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

		btTransform worldTrans;

		g_engfuncs.pfnGetBonePosition(ent, m_boneindex, bone_origin, bone_angles);

		m_cached_boneorigin = bone_origin;
		m_cached_boneangles = bone_angles;

		btVector3 GoldSrcOrigin(bone_origin.x, bone_origin.y, bone_origin.z);

		Vector3GoldSrcToBullet(GoldSrcOrigin);

		worldTrans = btTransform(btQuaternion(0, 0, 0, 1), GoldSrcOrigin);

		vec3_t GoldSrcAngles = bone_angles;

		btVector3 angles;
		GoldSrcAngles.CopyToArray(angles.m_floats);
		EulerMatrix(angles, worldTrans.getBasis());

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
	//Should do this before step simulation?

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

						if (physobj->IsPlayer())
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

bool CPhysicsManager::PM_ShouldCollide(int info)
{
	int playerIndex = pmove->player_index + 1;
	
	auto physent = &pmove->physents[info];

	if (physent->info > 0)
	{
		auto obj = GetGameObject(physent->info);
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
	//Check if stuck?
	if (!m_numDynamicObjects)
		return;

	pmtrace_t trace = { 0 };
	int hitent = pmove->PM_TestPlayerPosition(pmove->origin, &trace);
	if (hitent != -1)
	{
		auto blocker = &pmove->physents[hitent];

		auto obj = gPhysicsManager.GetGameObject(blocker->info);

		if (obj)
		{
			vec3_t impactvelocity;
			if (obj->PM_CheckStuck(hitent, blocker, &impactvelocity) == 1)
			{
				PM_AddToTouched(trace, impactvelocity);
			}
		}
	}
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

	int modelindex = ent->v.modelindex;
	if (modelindex == -1)
	{
		return false;
	}

	if (!m_brushIndexArray[modelindex])
	{
		return false;
	}

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH && ent->v.solid == SOLID_BSP)) ? true : false;

	auto staticobj = CreateStaticObject(obj, m_worldVertexArray, m_brushIndexArray[modelindex], bKinematic);
	
	if (staticobj)
	{
		obj->AddPhysicObject(staticobj, m_dynamicsWorld, &m_numDynamicObjects);

		return true;
	}

	return false;
}

bool CPhysicsManager::CreatePlayerBox(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	btVector3 boxSize((ent->v.maxs.x - ent->v.mins.x) * 0.5f + 4, (ent->v.maxs.y - ent->v.mins.y) * 0.5f + 4, (ent->v.maxs.z - ent->v.mins.z) * 0.5f - 1.0f);

	auto meshShape = new btBoxShape(boxSize);

	float mass = 20;

	btVector3 localInertia;
	meshShape->calculateLocalInertia(mass, localInertia);

	auto playerobj = CreatePlayerObject(obj, mass, meshShape, localInertia);

	if (playerobj)
	{
		obj->AddPhysicObject(playerobj, m_dynamicsWorld, &m_numDynamicObjects);

		return true;
	}

	return false;
}

bool CPhysicsManager::ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	//TODO: WTF?

	/*auto dynamicobj = (CDynamicObject *)obj;

	btVector3 vecImpulse;
	impulse.CopyToArray(vecImpulse.m_floats);
	Vector3GoldSrcToBullet(vecImpulse);

	vec3_t relpos;
	relpos = ent->v.origin - origin;

	btVector3 vecRelPos;
	relpos.CopyToArray(vecRelPos.m_floats);
	Vector3GoldSrcToBullet(vecRelPos);

	dynamicobj->GetRigidBody()->applyImpulse(vecImpulse, vecRelPos);*/

	return true;
}

bool CPhysicsManager::SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetLevelOfDetail(flags, body_0, scale_0, body_1, scale_1, distance_1, body_2, scale_2, distance_2, body_3, scale_3, distance_3);

	return false;
}

bool CPhysicsManager::SetEntityPartialViewer(edict_t* ent, int partial_viewer_mask)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetPartialViewer(partial_viewer_mask);

	return false;
}

bool CPhysicsManager::SetEntitySuperPusher(edict_t* ent, bool enable)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetSuperPusherEnabled(enable);

	return false;
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

	//flags FMODEL_TRACE_HITBOX is required or not?
	//if (!(mod->flags & FMODEL_TRACE_HITBOX))
	//{
	//	return false;
	//}

#if 1

	btVector3 boxSize(halfext.x, halfext.y, halfext.z);
	
	Vector3GoldSrcToBullet(boxSize);

	if (boxSize.x() <= 0 || boxSize.y() <= 0 || boxSize.z() <= 0)
	{
		//Must be valid size
		return false;
	}

	btVector3 boxSize2(halfext2.x, halfext2.y, halfext2.z);

	Vector3GoldSrcToBullet(boxSize2);

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

#endif
	obj->AddSolidOptimizer(boneindex, 0);

	return true;
}

bool CPhysicsManager::CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
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

	if(!mod)
	{
		//Must have a model
		return false;
	}

	if (mod->type != mod_studio)
	{
		//Must be studio
		return false;
	}

	//flags FMODEL_TRACE_HITBOX is required
	//if (!(mod->flags & FMODEL_TRACE_HITBOX))
	//{
	//	return false;
	//}

	btVector3 boxSize((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f);

	Vector3GoldSrcToBullet(boxSize);

	if(boxSize.x() <= 0 || boxSize.y() <= 0 || boxSize.z() <= 0)
	{
		//Must be valid size
		return false;
	}

	auto shape = new btBoxShape(boxSize);

	if (mass <= 0)
		mass = 1;

	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	auto dynamicobj = CreateDynamicObject(obj, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable, shape, localInertia);

	if (dynamicobj)
	{
		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		Vector3GoldSrcToBullet(vecLinearVelocity);

		dynamicobj->GetRigidBody()->setLinearVelocity(vecLinearVelocity);

		btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		dynamicobj->GetRigidBody()->setAngularVelocity(vecALinearVelocity);

		ent->v.velocity = g_vecZero;
		ent->v.avelocity = g_vecZero;

		obj->AddPhysicObject(dynamicobj, m_dynamicsWorld, &m_numDynamicObjects);

		return true;
	}

	return false;
}

bool CPhysicsManager::CreatePhysicSphere(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
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

	//flags FMODEL_TRACE_HITBOX is required or not?
	//if (!(mod->flags & FMODEL_TRACE_HITBOX))
	//{
	//	return false;
	//}

	float radius = (ent->v.maxs.x - ent->v.mins.x) * 0.5f;

	FloatGoldSrcToBullet(&radius);

	if (radius <= 0)
	{
		//Must be valid size
		return false;
	}

	auto shape = new btSphereShape(radius);

	if (mass <= 0)
		mass = 1;

	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	auto dynamicobj = CreateDynamicObject(obj, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable, shape, localInertia);

	if (dynamicobj)
	{
		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		Vector3GoldSrcToBullet(vecLinearVelocity);

		dynamicobj->GetRigidBody()->setLinearVelocity(vecLinearVelocity);

		btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		dynamicobj->GetRigidBody()->setAngularVelocity(vecALinearVelocity);

		ent->v.velocity = g_vecZero;
		ent->v.avelocity = g_vecZero;

		obj->AddPhysicObject(dynamicobj, m_dynamicsWorld, &m_numDynamicObjects);

		return true;
	}

	return false;
}

void CPhysicsManager::NewMap(edict_t *ent)
{
	G2BScale = CVAR_GET_FLOAT("bv_worldscale");
	B2GScale = 1 / G2BScale;

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

			if (physobj0->IsKinematic() || physobj0->IsPlayer())
			{
				auto ent0 = physobj0->GetGameObject()->GetEdict();

				if(ent0->v.solid <= SOLID_TRIGGER)
					return false;
			}

			if (physobj1->IsKinematic() || physobj1->IsPlayer())
			{
				auto ent1 = physobj1->GetGameObject()->GetEdict();

				if (ent1->v.solid <= SOLID_TRIGGER)
					return false;
			}

			if (physobj0->IsSolidOptimizerGhost() && physobj1->IsPlayer())
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

void CPhysicsManager::Init(void)
{
	CVAR_REGISTER(&bv_tickrate);
	CVAR_REGISTER(&bv_worldscale);

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
	if (bv_simrate->value < 32)
	{
		g_engfuncs.pfnCVarSetFloat("bv_tickrate", 32);
	}
	else if (bv_simrate->value > 128)
	{
		g_engfuncs.pfnCVarSetFloat("bv_tickrate", 128);
	}

	if (!gPhysicsManager.GetNumDynamicBodies())
		return;

	m_dynamicsWorld->stepSimulation((btScalar)frametime, 3, (btScalar)(1.0f / bv_simrate->value));
}

void CPhysicsManager::SetGravity(float velocity)
{
	m_gravity = -velocity;

	FloatGoldSrcToBullet(&m_gravity);

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

#if 0
		for (size_t i = 0; i < m_solid_optimizer.size(); ++i)
		{
			m_solid_optimizer[i].StartFrame(this);
		}
#endif

		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			m_physics[i]->StartFrame(world);
		}

		return;
	}

	for (size_t i = 0; i < m_physics.size(); ++i)
	{
		m_physics[i]->StartFrame(world);
	}
}

void CGameObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	if (IsSolidOptimizerEnabled())
	{
		for (size_t i = 0; i < m_physics.size(); ++i)
		{
			m_physics[i]->StartFrame_Post(world);
		}

		//if (GetSemiClipMask() == gPhysicsManager.GetSolidPlayerMask())
		//	GetEdict()->v.solid = SOLID_NOT;
		//else
		//	GetEdict()->v.solid = SOLID_BBOX;

		return;
	}

	for (size_t i = 0; i < m_physics.size(); ++i)
	{
		m_physics[i]->StartFrame_Post(world);
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
	if (GetPartialViewerMask())
	{
		int hostindex = g_engfuncs.pfnIndexOfEdict(host);
		if ((GetPartialViewerMask() & (1 << (hostindex - 1))) == 0)
		{
			return false;
		}
	}

	if (GetSemiClipMask())
	{
		int hostindex = g_engfuncs.pfnIndexOfEdict(host);
		if ((GetSemiClipMask() & (1 << (hostindex - 1))) != 0)
		{
			state->solid = SOLID_NOT;
			//test
			//state->renderamt = 128;
			//state->rendermode = kRenderTransTexture;
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
