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

cvar_t fg_simrate = {
	(char*)"fg_simrate",
	(char*)"64",
	FCVAR_SERVER,
	64
};

cvar_t* bv_simrate = &fg_simrate;

cvar_t fg_pretick = {
	(char*)"fg_pretick",
	(char*)"1",
	FCVAR_SERVER,
	1
};

cvar_t* bv_pretick = &fg_pretick;

cvar_t fg_scale = {
	(char*)"fg_scale",
	(char*)"1",
	FCVAR_SERVER,
	1
};

cvar_t* bv_scale = &fg_scale;


btScalar G2BScale = 1;
btScalar B2GScale = 1 / G2BScale;

extern edict_t* r_worldentity;
extern model_t* r_worldmodel;

const float r_identity_matrix[4][4] = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 1.0f}
};

void Matrix4x4_Transpose(float out[4][4], float in1[4][4])
{
	out[0][0] = in1[0][0];
	out[0][1] = in1[1][0];
	out[0][2] = in1[2][0];
	out[0][3] = in1[3][0];
	out[1][0] = in1[0][1];
	out[1][1] = in1[1][1];
	out[1][2] = in1[2][1];
	out[1][3] = in1[3][1];
	out[2][0] = in1[0][2];
	out[2][1] = in1[1][2];
	out[2][2] = in1[2][2];
	out[2][3] = in1[3][2];
	out[3][0] = in1[0][3];
	out[3][1] = in1[1][3];
	out[3][2] = in1[2][3];
	out[3][3] = in1[3][3];
}

void SinCos(float radians, float* sine, float* cosine)
{
	*sine = sinf(radians);
	*cosine = cosf(radians);
}

void Matrix3x4ToTransform(const float matrix3x4[3][4], btTransform& trans)
{
	float matrix4x4[4][4] = {
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	memcpy(matrix4x4, matrix3x4, sizeof(float[3][4]));

	float matrix4x4_transposed[4][4];
	Matrix4x4_Transpose(matrix4x4_transposed, matrix4x4);

	trans.setFromOpenGLMatrix((float*)matrix4x4_transposed);
}

void TransformToMatrix3x4(const btTransform& trans, float matrix3x4[3][4])
{
	float matrix4x4_transposed[4][4];
	trans.getOpenGLMatrix((float*)matrix4x4_transposed);

	float matrix4x4[4][4];
	Matrix4x4_Transpose(matrix4x4, matrix4x4_transposed);

	memcpy(matrix3x4, matrix4x4, sizeof(float[3][4]));
}

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
}

void CPhysicsManager::GenerateBrushIndiceArray(void)
{
	int maxNum = EngineGetMaxPrecacheModel();

	if ((int)m_brushIndexArray.size() < maxNum)
		m_brushIndexArray.resize(maxNum);

	for (int i = 0; i < maxNum; ++i)
	{
		if (m_brushIndexArray[i])
		{
			delete m_brushIndexArray[i];
			m_brushIndexArray[i] = NULL;
		}
	}

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

	for (size_t i = 0; i < m_allocPoly.size(); ++i)
	{
		free(m_allocPoly[i]);
	}
	m_allocPoly.clear();
}

void CPhysicsManager::BuildSurfaceDisplayList(model_t *mod, msurface_t* fa)
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

	m_allocPoly.emplace_back(poly);

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

void CPhysicsManager::GenerateWorldVerticeArray(void)
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

		BuildSurfaceDisplayList(r_worldmodel, &surf[i]);

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

CDynamicBody* CPhysicsManager::CreateDynamicBody(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	auto dynamicbody = new CDynamicBody;

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(ent, dynamicbody), collisionShape, localInertia);
	auto body = new btRigidBody(cInfo);

	dynamicbody->m_rigbody = body;
	dynamicbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
	dynamicbody->m_mass = mass;
	
	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);
	body->setCcdMotionThreshold(1e-7);
	body->setCcdSweptSphereRadius(0.5);

	btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

	Vector3GoldSrcToBullet(vecLinearVelocity);

	body->setLinearVelocity(vecLinearVelocity);

	btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

	body->setAngularVelocity(vecALinearVelocity);

	m_dynamicsWorld->addRigidBody(body);

	m_bodyMap[g_engfuncs.pfnIndexOfEdict(ent)] = dynamicbody;

	return dynamicbody;
}

CStaticBody* CPhysicsManager::CreateStaticBody(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic)
{
	if (!indexarray->vIndiceBuffer.size())
	{
		auto staticbody = new CStaticBody;

		staticbody->m_rigbody = NULL;
		staticbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
		staticbody->m_vertexarray = vertexarray;
		staticbody->m_indexarray = indexarray;

		m_bodyMap[g_engfuncs.pfnIndexOfEdict(ent)] = staticbody;

		return staticbody;
	}

	auto staticbody = new CStaticBody;

	auto vertexArray = new btTriangleIndexVertexArray(
		indexarray->vIndiceBuffer.size() / 3, indexarray->vIndiceBuffer.data(), 3 * sizeof(int),
		vertexarray->vVertexBuffer.size(), (float*)vertexarray->vVertexBuffer.data(), sizeof(brushvertex_t));

	auto meshShape = new btBvhTriangleMeshShape(vertexArray, true, true);

	btMotionState* motionState = NULL;
	if (kinematic)
		motionState = new EntityMotionState(ent, staticbody);
	else
		motionState = new btDefaultMotionState();

	btRigidBody::btRigidBodyConstructionInfo cInfo(0, motionState, meshShape);
	
	auto body = new btRigidBody(cInfo);

	m_dynamicsWorld->addRigidBody(body);

	staticbody->m_rigbody = body;
	staticbody->m_entindex = g_engfuncs.pfnIndexOfEdict(ent);
	staticbody->m_vertexarray = vertexarray;
	staticbody->m_indexarray = indexarray;

	if (kinematic)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);

		staticbody->m_kinematic = true;
	}

	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);

	m_bodyMap[g_engfuncs.pfnIndexOfEdict(ent)] = staticbody;

	return staticbody;
}

void CPhysicsManager::CreateWater(edict_t* ent)
{

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

EntityMotionState::EntityMotionState(edict_t* ent, CPhysicBody * body)
{
	pent = ent;
	pbody = body;
}

void EntityMotionState::getWorldTransform(btTransform& worldTrans) const
{
	btVector3 origin(pent->v.origin.x, pent->v.origin.y, pent->v.origin.z);

	Vector3GoldSrcToBullet(origin);

	worldTrans = btTransform(btQuaternion(0, 0, 0, 1), origin);

	btVector3 angles;
	pent->v.angles.CopyToArray(angles.m_floats);
	EulerMatrix(angles, worldTrans.getBasis());
}

void EntityMotionState::setWorldTransform(const btTransform& worldTrans)
{
	Vector origin = Vector((float*)(worldTrans.getOrigin().m_floats));

	Vec3BulletToGoldSrc(origin);

	btVector3 btAngles;
	MatrixEuler(worldTrans.getBasis(), btAngles);
	Vector angles = Vector((float*)btAngles.m_floats);

	SET_ORIGIN(pent, origin);
	pent->v.angles = angles;
}

void CPhysicsManager::CreateBrushModel(edict_t* ent)
{
	auto itor = m_bodyMap.find(g_engfuncs.pfnIndexOfEdict(ent));

	if (itor != m_bodyMap.end())
	{
		//Already created?
		return;
	}

	int modelindex = ent->v.modelindex;
	if (modelindex == -1)
	{
		//invalid model index?
		UTIL_LogPrintf("CreateBrushModel: Invalid model index\n");
		return;
	}

	if (!m_brushIndexArray[modelindex])
	{
		//invalid model index?
		UTIL_LogPrintf("CreateBrushModel: Invalid model index\n");
		return;
	}

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH)) ? true : false;

	CreateStaticBody(ent, m_worldVertexArray, m_brushIndexArray[modelindex], bKinematic);
}

void CPhysicsManager::CreatePhysBox(edict_t* ent)
{
	auto itor = m_bodyMap.find(g_engfuncs.pfnIndexOfEdict(ent));

	if (itor != m_bodyMap.end())
	{
		//Already created?
		return;
	}

	btVector3 boxSize((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f);

	auto meshShape = new btBoxShape(boxSize);

	float mass = ent->v.flFallVelocity;
	if (mass <= 0)
		mass = 1;

	btVector3 localInertia;
	meshShape->calculateLocalInertia(mass, localInertia);

	CreateDynamicBody(ent, mass, meshShape, localInertia);
}

void CPhysicsManager::NewMap(void)
{
	G2BScale = CVAR_GET_FLOAT("fg_scale");
	B2GScale = 1 / G2BScale;

	ReloadConfig();
	RemoveAllStatics();

	r_worldentity = g_engfuncs.pfnPEntityOfEntIndex(0);

	r_worldmodel = EngineGetPrecachedModelByIndex(r_worldentity->v.modelindex);

	GenerateWorldVerticeArray();
	GenerateBrushIndiceArray();

	CreateBrushModel(r_worldentity);
}

void CPhysicsManager::PreTickCallback(btScalar timeStep)
{
	if (bv_pretick->value)
	{
		
	}
}

void _PreTickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	gPhysicsManager.PreTickCallback(timeStep);
}

void CPhysicsManager::Init(void)
{
	CVAR_REGISTER(&fg_simrate);
	CVAR_REGISTER(&fg_pretick);
	CVAR_REGISTER(&fg_scale);

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	//m_debugDraw = new CPhysicsDebugDraw;

	m_dynamicsWorld->setGravity(btVector3(0, 0, 0));

	m_dynamicsWorld->setInternalTickCallback(_PreTickCallback, this, true);
}

void CPhysicsManager::DebugDraw(void)
{
	
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
	m_dynamicsWorld->stepSimulation(frametime, 16, 1.0f / bv_simrate->value);
}

void CPhysicsManager::SetGravity(float velocity)
{
	m_gravity = -velocity;

	FloatGoldSrcToBullet(&m_gravity);

	m_dynamicsWorld->setGravity(btVector3(0, 0, m_gravity));
}

void CPhysicsManager::ReloadConfig(void)
{
	
}

void CPhysicsManager::RemoveAllStatics()
{
	for (auto p : m_bodyMap)
	{
		auto body = p.second;
		if (body)
		{
			m_dynamicsWorld->removeRigidBody(body->m_rigbody);

			delete body;
		}
	}

	m_bodyMap.clear();
}