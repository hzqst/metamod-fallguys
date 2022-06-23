#include <extdll.h>			// always

#include <meta_api.h>		// of course

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

void CDynamicBody::GetVelocity(vec3_t &vel) const
{
	auto vecVelocity = m_rigbody->getLinearVelocity();

	vel.x = vecVelocity.getX();
	vel.y = vecVelocity.getY();
	vel.z = vecVelocity.getZ();

	Vec3BulletToGoldSrc(vel);
}

void CPhysicsDebugDraw::drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1)
{
	
}

CPhysicsManager gPhysicsManager;

CPhysicsManager::CPhysicsManager()
{
	m_collisionConfiguration = NULL;
	m_dispatcher = NULL;
	m_overlappingPairCache = NULL;
	m_solver = NULL;
	m_dynamicsWorld = NULL;
	m_debugDraw = NULL;
	m_worldVertexArray = NULL;
	m_gravity = 0;
	m_numDynamicBody = 0;
	m_maxIndexPhysBody = 0;
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
	int vertpage;
	float* vec;
	float s, t;
	glpoly_t* poly;

	pedges = mod->edges;
	lnumverts = fa->numedges;
	vertpage = 0;

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

	int iNumFaces = 0;
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
				memcpy(&Vertexes[1], &Vertexes[2], sizeof(brushvertex_t));

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

CDynamicBody* CPhysicsManager::CreateDynamicBody(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold,
	bool pushable, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	auto dynamicbody = new CDynamicBody;
	dynamicbody->m_pent = ent;
	dynamicbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
	dynamicbody->m_mass = mass;
	dynamicbody->m_pushable = pushable;

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(dynamicbody), collisionShape, localInertia);

	cInfo.m_friction = friction;
	cInfo.m_rollingFriction = rollingFriction;
	cInfo.m_restitution = restitution;
	cInfo.m_linearSleepingThreshold = 0.1f;
	cInfo.m_angularSleepingThreshold = 0.001f;

	auto body = new btRigidBody(cInfo);

	dynamicbody->m_rigbody = body;

	body->setCcdSweptSphereRadius(G2BScale * ccdRadius);
	body->setCcdMotionThreshold(G2BScale * ccdThreshold);
	body->setUserPointer(dynamicbody);

	m_dynamicsWorld->addRigidBody(body, pushable ? BMASK_ALL : BMASK_ALL_NONPLAYER, pushable ? BMASK_DYNAMIC_PUSHABLE : BMASK_DYNAMIC);

	m_physBodies[dynamicbody->m_entindex] = dynamicbody;
	
	if (dynamicbody->m_entindex > m_maxIndexPhysBody)
		m_maxIndexPhysBody = dynamicbody->m_entindex;

	m_numDynamicBody++;

	return dynamicbody;
}

CStaticBody* CPhysicsManager::CreateStaticBody(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic)
{
	if (!indexarray->vIndiceBuffer.size())
	{
		auto staticbody = new CStaticBody;
		staticbody->m_pent = ent;
		staticbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
		staticbody->m_vertexarray = vertexarray;
		staticbody->m_indexarray = indexarray;

		staticbody->m_rigbody = NULL;

		if (kinematic)
		{
			staticbody->m_kinematic = true;
		}

		m_physBodies[staticbody->m_entindex] = staticbody;

		if (staticbody->m_entindex > m_maxIndexPhysBody)
			m_maxIndexPhysBody = staticbody->m_entindex;

		return staticbody;
	}

	auto staticbody = new CStaticBody;
	staticbody->m_pent = ent;
	staticbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
	staticbody->m_vertexarray = vertexarray;
	staticbody->m_indexarray = indexarray;

	auto vertexArray = new btTriangleIndexVertexArray(
		indexarray->vIndiceBuffer.size() / 3, indexarray->vIndiceBuffer.data(), 3 * sizeof(int),
		vertexarray->vVertexBuffer.size(), (float*)vertexarray->vVertexBuffer.data(), sizeof(brushvertex_t));

	auto meshShape = new btBvhTriangleMeshShape(vertexArray, true, true);

	btMotionState* motionState = NULL;
	if (kinematic)
		motionState = new EntityMotionState(staticbody);
	else
		motionState = new btDefaultMotionState();

	btRigidBody::btRigidBodyConstructionInfo cInfo(0, motionState, meshShape);
	
	auto body = new btRigidBody(cInfo);

	staticbody->m_rigbody = body;

	if (kinematic)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		staticbody->m_kinematic = true;
	}

	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);
	body->setUserPointer(staticbody);

	m_dynamicsWorld->addRigidBody(body, BMASK_ALL, kinematic ? BMASK_BRUSH : BMASK_WORLD);

	m_physBodies[staticbody->m_entindex] = staticbody;

	if (staticbody->m_entindex > m_maxIndexPhysBody)
		m_maxIndexPhysBody = staticbody->m_entindex;

	return staticbody;
}

CPlayerBody* CPhysicsManager::CreatePlayerBody(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	auto playerbody = new CPlayerBody;
	playerbody->m_pent = ent;
	playerbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(playerbody), collisionShape, localInertia);

	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 0;

	auto body = new btRigidBody(cInfo);

	playerbody->m_rigbody = body;
	playerbody->m_mass = mass;

	body->setUserPointer(playerbody);

	m_dynamicsWorld->addRigidBody(body, BMASK_DYNAMIC_PUSHABLE, BMASK_PLAYER);

	m_physBodies[playerbody->m_entindex] = playerbody;

	if (playerbody->m_entindex > m_maxIndexPhysBody)
		m_maxIndexPhysBody = playerbody->m_entindex;

	return playerbody;
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
	auto ent = GetPhysicBody()->m_pent;

	//Player and brush upload origin and angles in normal way
	if (!GetPhysicBody()->IsDynamic())
	{
		btVector3 GoldSrcOrigin(ent->v.origin.x, ent->v.origin.y, ent->v.origin.z);

		Vector3GoldSrcToBullet(GoldSrcOrigin);

		worldTrans = btTransform(btQuaternion(0, 0, 0, 1), GoldSrcOrigin);

		vec3_t GoldSrcAngles = ent->v.angles;

		if (GetPhysicBody()->IsPlayer())
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
	//Nope, don't download player state !
	if (GetPhysicBody()->IsPlayer())
	{
		return;
	}

	auto ent = GetPhysicBody()->m_pent;

	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	Vec3BulletToGoldSrc(origin);

	btVector3 btAngles;
	MatrixEuler(worldTrans.getBasis(), btAngles);
	Vector angles = Vector((float*)btAngles.m_floats);

	SET_ORIGIN(ent, origin);

	//Clamp to -3600~3600
	for (int i = 0; i < 3; i++)
	{
		if (angles[i] < -3600.0f || angles[i] > 3600.0f)
			angles[i] = fmod(angles[i], 3600.0f);
	}

	ent->v.angles = angles;
}

void CPhysicsManager::EntityStartFrame()
{
	for (int i = 1; i < gpGlobals->maxEntities; ++i)
	{
		auto ent = g_engfuncs.pfnPEntityOfEntIndex(i);

		if (!ent)
			continue;

		auto body = m_physBodies[i];

		if (!body)
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
				RemovePhysicBody(i);
				continue;
			}

			//Player or brush changed to non-solid?
			if (body->IsKinematic() && ent->v.solid <= SOLID_TRIGGER)
			{
				RemovePhysicBody(i);
				continue;
			}
			
			body->StartFrame();
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

		auto body = m_physBodies[i];

		if (body)
		{
			body->StartFrame_Post();
		}
	}
}

void CDynamicBody::StartFrame()
{

}

void CDynamicBody::StartFrame_Post()
{
	//Download linear velocity from bullet engine

	btVector3 vecVelocity = m_rigbody->getLinearVelocity();

	Vector velocity(vecVelocity.x(), vecVelocity.y(), vecVelocity.z());
	
	Vec3BulletToGoldSrc(velocity);

	m_pent->v.vuser1 = velocity;
}

void CPlayerBody::StartFrame()
{
	//Upload to bullet engine before simulation
	btTransform trans;
	m_rigbody->getMotionState()->getWorldTransform(trans);
	m_rigbody->setWorldTransform(trans);

	//Do we really need this?
	btVector3 vecVelocity(m_pent->v.velocity.x, m_pent->v.velocity.y, m_pent->v.velocity.z);
	Vector3GoldSrcToBullet(vecVelocity);
	m_rigbody->setLinearVelocity(vecVelocity);
}

void CPlayerBody::StartFrame_Post()
{
	
}

void CPhysicsManager::FreeEntityPrivateData(edict_t* ent)
{
	RemovePhysicBody(g_engfuncs.pfnIndexOfEdict(ent));
}

bool CPhysicsManager::SetAbsBox(edict_t *pent)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(pent)];

	if (body && body->IsDynamic())
	{
		auto dynamicbody = (CDynamicBody *)body;

		btVector3 aabbMins, aabbMaxs;
		dynamicbody->m_rigbody->getAabb(aabbMins, aabbMaxs);

		Vector3BulletToGoldSrc(aabbMins);
		Vector3BulletToGoldSrc(aabbMaxs);

		pent->v.absmin.x = aabbMins.getX();
		pent->v.absmin.y = aabbMins.getY();
		pent->v.absmin.z = aabbMins.getZ();
		pent->v.absmax.x = aabbMaxs.getX();
		pent->v.absmax.y = aabbMaxs.getY();
		pent->v.absmax.z = aabbMaxs.getZ();

		//wtf why?
		pent->v.absmin.x -= 1;
		pent->v.absmin.y -= 1;
		pent->v.absmin.z -= 1;
		pent->v.absmax.x += 1;
		pent->v.absmax.y += 1;
		pent->v.absmax.z += 1;

		return true;
	}

	return false;
}

void SV_CopyEdictToPhysent(physent_t *pe, int e, edict_t *check)
{
	model_t *pModel;

	pe->info = e;
	pe->origin = check->v.origin;

	if (e >= 1 && e <= gpGlobals->maxClients)
	{
		//wtf?
		//SV_GetTrueOrigin(e - 1, pe->origin);
		pe->player = e;
	}
	else
	{
		pe->player = 0;
	}

	pe->angles = check->v.angles;

	pe->studiomodel = NULL;
	pe->rendermode = check->v.rendermode;

	switch (check->v.solid)
	{
	case SOLID_NOT:
	{
		if (check->v.modelindex)
		{
			pe->model = (*sv_models)[check->v.modelindex];

			strncpy(pe->name, pe->model->name, sizeof(pe->name));
			pe->name[sizeof(pe->name) - 1] = 0;
		}
		else
		{
			pe->model = NULL;
		}

		break;
	}

	case SOLID_BBOX:
	{
		pe->model = NULL;

		if (check->v.modelindex)
		{
			pModel = (*sv_models)[check->v.modelindex];
		}
		else
		{
			pModel = NULL;
		}

		if (pModel)
		{
			//if (pModel->flags & FMODEL_TRACE_HITBOX)
				pe->studiomodel = pModel;

			strncpy(pe->name, pModel->name, sizeof(pe->name));
			pe->name[sizeof(pe->name) - 1] = 0;
		}

		pe->mins = check->v.mins;
		pe->maxs = check->v.maxs;
		break;
	}

	case SOLID_BSP:
	{
		pe->model = (*sv_models)[check->v.modelindex];

		strncpy(pe->name, pe->model->name, sizeof(pe->name));
		pe->name[sizeof(pe->name) - 1] = 0;
		break;
	}

	default:
	{
		pe->model = NULL;

		pe->mins = check->v.mins;
		pe->maxs = check->v.maxs;

		if (check->v.classname)
		{
			strncpy(pe->name, STRING(check->v.classname), sizeof(pe->name));
			pe->name[sizeof(pe->name) - 1] = 0;
		}
		else
		{
			pe->name[0] = '?';
			pe->name[1] = 0;
		}
	}
	}

	pe->solid = check->v.solid;
	pe->skin = check->v.skin;
	pe->frame = check->v.frame;
	pe->sequence = check->v.sequence;

	memcpy(pe->controller, check->v.controller, 4);
	memcpy(pe->blending, check->v.blending, 2);

	pe->movetype = check->v.movetype;
	pe->takedamage = DAMAGE_NO;
	pe->blooddecal = 0;

	pe->iuser1 = check->v.iuser1;
	pe->iuser2 = check->v.iuser2;
	pe->iuser3 = check->v.iuser3;
	pe->iuser4 = check->v.iuser4;
	pe->fuser1 = check->v.fuser1;
	pe->fuser2 = check->v.fuser2;
	pe->fuser3 = check->v.fuser3;
	pe->fuser4 = check->v.fuser4;

	pe->vuser1 = check->v.vuser1;
	pe->vuser2 = check->v.vuser2;
	pe->vuser3 = check->v.vuser3;
	pe->vuser4 = check->v.vuser4;
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
		return false;// pmove->Con_DPrintf("Too many entities were touched!\n");

	pmove->touchindex[pmove->numtouch++] = tr;
	return true;
}

void CPhysicsManager::PM_StartMove()
{

}

void CPhysicsManager::PM_EndMove()
{
	//Check if stuck?
	if (m_numDynamicBody)
	{
		pmtrace_t trace = { 0 };
		int hitent = pmove->PM_TestPlayerPosition(pmove->origin, &trace);
		if (hitent != -1)
		{
			auto blocker = &pmove->physents[hitent];

			auto physbody = gPhysicsManager.GetPhysicBody(blocker->info);

			if (physbody && physbody->IsDynamic())
			{
				vec3_t move;
				
				move = pmove->origin - blocker->origin;

				move.z += (blocker->maxs.z - blocker->mins.z) * 0.5f;

				move = move.Normalize();

				move = move * (1000.0f * pmove->frametime);

				auto backup_origin = pmove->origin;

				blocker->solid = SOLID_NOT;

				pmove->origin = pmove->origin + move;

				pmtrace_t trace2 = { 0 };
				auto hitent2 = pmove->PM_TestPlayerPosition(pmove->origin, &trace2);
				if (hitent2 != -1 && hitent2 != hitent)
				{
					//Blocked, don't move
					pmove->origin = backup_origin;
				}
				else
				{
					PM_AddToTouched(trace, move);
				}

				blocker->solid = SOLID_BBOX;
			}
		}
	}
}

bool CPhysicsManager::CreateBrushModel(edict_t* ent)
{
	int modelindex = ent->v.modelindex;
	if (modelindex == -1)
	{
		//invalid model index?
		UTIL_LogPrintf("CreateBrushModel: Invalid model index\n");
		return false;
	}

	if (!m_brushIndexArray[modelindex])
	{
		//invalid model index?
		UTIL_LogPrintf("CreateBrushModel: Invalid model index\n");
		return false;
	}

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH)) ? true : false;

	auto staticbody = CreateStaticBody(ent, m_worldVertexArray, m_brushIndexArray[modelindex], bKinematic);
	
	if (staticbody)
	{
		return true;
	}

	return false;
}

bool CPhysicsManager::CreatePlayerBox(edict_t* ent)
{
	btVector3 boxSize((ent->v.maxs.x - ent->v.mins.x) * 0.5f + 4, (ent->v.maxs.y - ent->v.mins.y) * 0.5f + 4, (ent->v.maxs.z - ent->v.mins.z) * 0.5f - 1.0f);

	auto meshShape = new btBoxShape(boxSize);

	float mass = 20;

	btVector3 localInertia;
	meshShape->calculateLocalInertia(mass, localInertia);

	if (CreatePlayerBody(ent, mass, meshShape, localInertia))
	{
		return true;
	}

	return false;
}

bool CPhysicsManager::ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];

	if (!body)
	{
		//no physic body found
		return false;
	}

	if (!body->IsDynamic())
	{
		//no dynamic body found
		return false;
	}

	btVector3 vecImpulse;
	impulse.CopyToArray(vecImpulse.m_floats);
	Vector3GoldSrcToBullet(vecImpulse);

	vec3_t relpos;
	relpos = ent->v.origin - origin;

	btVector3 vecRelPos;
	relpos.CopyToArray(vecRelPos.m_floats);
	Vector3GoldSrcToBullet(vecRelPos);

	body->m_rigbody->applyImpulse(vecImpulse, vecRelPos);

	return true;
}

bool CPhysicsManager::CreateSuperPusher(edict_t* ent)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];

	if(!body && IsEntitySolidPusher(ent))
	{
		if (CreateBrushModel(ent))
		{
			body = GetPhysicBody(ent);
		}
	}

	if (body)
	{
		if (body->IsKinematic())
		{
			auto staticbody = (CStaticBody *)body;
			staticbody->m_superpusher = true;

			return true;
		}
		else if (body->IsDynamic())
		{
			auto dynamicbody = (CDynamicBody *)body;
			dynamicbody->m_superpusher = true;

			return true;
		}
	}

	return false;
}

bool CPhysicsManager::CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];

	if (body)
	{
		//Already created
		return false;
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

	//mod->flags |= FMODEL_TRACE_HITBOX;//always use hitbox as hull

	btVector3 boxSize((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f);

	if(boxSize.x() <= 0 || boxSize.y() <= 0 || boxSize.z() <= 0)
	{
		//Must be valid size
		return false;
	}

	Vector3GoldSrcToBullet(boxSize);

	auto meshShape = new btBoxShape(boxSize);

	if (mass <= 0)
		mass = 1;

	btVector3 localInertia;
	meshShape->calculateLocalInertia(mass, localInertia);

	auto dynamicbody = CreateDynamicBody(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable, meshShape, localInertia);

	if (dynamicbody)
	{
		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		Vector3GoldSrcToBullet(vecLinearVelocity);

		dynamicbody->m_rigbody->setLinearVelocity(vecLinearVelocity);

		btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		dynamicbody->m_rigbody->setAngularVelocity(vecALinearVelocity);

		ent->v.velocity = g_vecZero;
		ent->v.avelocity = g_vecZero;

		return true;
	}

	return false;
}

void CPhysicsManager::NewMap(edict_t *ent)
{
	G2BScale = CVAR_GET_FLOAT("bv_worldscale");
	B2GScale = 1 / G2BScale;

	m_maxIndexPhysBody = 0;
	m_physBodies.resize(gpGlobals->maxEntities);

	r_worldentity = ent;

	r_worldmodel = EngineGetPrecachedModelByIndex(r_worldentity->v.modelindex);

	std::vector<glpoly_t*> glpolys;
	GenerateWorldVerticeArray(glpolys);
	GenerateBrushIndiceArray(glpolys);

	CreateBrushModel(r_worldentity);
}

void CPhysicsManager::Shutdown(void)
{
	RemoveAllPhysicBodies();

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
}

void CPhysicsManager::Init(void)
{
	CVAR_REGISTER(&bv_tickrate);
	CVAR_REGISTER(&bv_worldscale);

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	m_dynamicsWorld->setGravity(btVector3(0, 0, 0));
}

void CPhysicsManager::StepSimulation(double frametime)
{
	if (bv_simrate->value < 32)
	{
		g_engfuncs.pfnCVarSetFloat("bv_simrate", 32);
	}
	else if (bv_simrate->value > 128)
	{
		g_engfuncs.pfnCVarSetFloat("bv_simrate", 128);
	}

	if (!gPhysicsManager.GetNumDynamicBodies())
		return;

	m_dynamicsWorld->stepSimulation(frametime, 3, 1.0f / bv_simrate->value);
}

void CPhysicsManager::SetGravity(float velocity)
{
	m_gravity = -velocity;

	FloatGoldSrcToBullet(&m_gravity);

	m_dynamicsWorld->setGravity(btVector3(0, 0, m_gravity));
}

int CPhysicsManager::GetNumDynamicBodies()
{
	return m_numDynamicBody;
}

CPhysicBody *CPhysicsManager::GetPhysicBody(int entindex)
{
	return m_physBodies[entindex];
}

CPhysicBody* CPhysicsManager::GetPhysicBody(edict_t *ent)
{
	return m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];
}

void CPhysicsManager::RemovePhysicBody(int entindex)
{
	if (entindex >= (int)m_physBodies.size())
		return;

	auto body = m_physBodies[entindex];
	if (body)
	{
		if (body->IsDynamic())
			m_numDynamicBody--;

		if (body->m_rigbody)
		{
			m_dynamicsWorld->removeRigidBody(body->m_rigbody);
			delete body->m_rigbody;
			body->m_rigbody = NULL;
		}

		delete body;

		m_physBodies[entindex] = NULL;
	}
}

void CPhysicsManager::RemoveAllPhysicBodies()
{
	for (int i = 0;i <= m_maxIndexPhysBody; ++i)
	{
		RemovePhysicBody(i);
	}

	m_physBodies.clear();
}

bool CPhysicsManager::IsSuperPusher(edict_t* ent)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];
	if (body && body->IsSuperPusher())
	{
		return true;
	}

	return false;
}

bool CPhysicsManager::IsDynamicPhysicObject(edict_t* ent)
{
	auto body = m_physBodies[g_engfuncs.pfnIndexOfEdict(ent)];
	if (body && body->IsDynamic())
	{
		return true;
	}

	return false;
}