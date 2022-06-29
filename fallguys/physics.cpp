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

CPhysicsManager gPhysicsManager;

CPhysicsManager::CPhysicsManager()
{
	m_collisionConfiguration = NULL;
	m_dispatcher = NULL;
	m_overlappingPairCache = NULL;
	m_solver = NULL;
	m_dynamicsWorld = NULL;
	m_worldVertexArray = NULL;
	m_gravity = 0;
	m_numDynamicObjects = 0;
	m_maxIndexGameObject = 0;
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

CDynamicObject* CPhysicsManager::CreateDynamicObject(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold,
	bool pushable, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	auto dynamicobj = new CDynamicObject(ent, g_engfuncs.pfnIndexOfEdict(ent), 
		pushable ? BMASK_ALL : BMASK_ALL_NONPLAYER, pushable ? BMASK_DYNAMIC_PUSHABLE : BMASK_DYNAMIC, //Dynamic object collide with all other stuffs when pushable, and all non-player stuffs when unpushable
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

	AddGameObject(dynamicobj);

	m_numDynamicObjects++;

	return dynamicobj;
}

CStaticObject* CPhysicsManager::CreateStaticObject(edict_t* ent, vertexarray_t* vertexarray, indexarray_t* indexarray, bool kinematic)
{
	if (!indexarray->vIndiceBuffer.size())
	{
		//todo: maybe use clipnode?
		auto staticobj = new CStaticObject(ent, g_engfuncs.pfnIndexOfEdict(ent), 
			BMASK_ALL, kinematic ? BMASK_BRUSH : BMASK_WORLD,//Static object collide with all other stuffs
			vertexarray, indexarray, kinematic);

		AddGameObject(staticobj);

		return staticobj;
	}

	auto staticobj = new CStaticObject(ent, g_engfuncs.pfnIndexOfEdict(ent), 
		BMASK_ALL, kinematic ? BMASK_BRUSH : BMASK_WORLD, 
		vertexarray, indexarray, kinematic);

	auto vertexArray = new btTriangleIndexVertexArray(
		indexarray->vIndiceBuffer.size() / 3, indexarray->vIndiceBuffer.data(), 3 * sizeof(int),
		vertexarray->vVertexBuffer.size(), (float*)vertexarray->vVertexBuffer.data(), sizeof(brushvertex_t));

	auto meshShape = new btBvhTriangleMeshShape(vertexArray, true, true);

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

	AddGameObject(staticobj);

	return staticobj;
}

CPlayerObject* CPhysicsManager::CreatePlayerObject(edict_t* ent, float mass, btCollisionShape* collisionShape, const btVector3& localInertia)
{
	auto playerobj = new CPlayerObject(ent, g_engfuncs.pfnIndexOfEdict(ent), 
		BMASK_DYNAMIC_PUSHABLE, BMASK_PLAYER,//Player only collides with pushable objects, and world..?(do we really need to collide with world ?)
		mass);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, new EntityMotionState(playerobj), collisionShape, localInertia);

	cInfo.m_friction = 1;
	cInfo.m_rollingFriction = 1;
	cInfo.m_restitution = 0;

	playerobj->SetRigidBody(new btRigidBody(cInfo));

	AddGameObject(playerobj);

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
	auto ent = GetPhysicObject()->GetEdict();

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
	//Nope, don't download player state !
	if (GetPhysicObject()->IsPlayer())
	{
		return;
	}

	auto ent = GetPhysicObject()->GetEdict();

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

			//Player or brush changed to non-solid?
			if (obj->IsKinematic() && ent->v.solid <= SOLID_TRIGGER)
			{
				RemoveGameObject(i);
				continue;
			}
			
			obj->StartFrame();
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
			body->StartFrame_Post();
		}
	}
}

void CDynamicObject::StartFrame()
{

}

void CDynamicObject::StartFrame_Post()
{
	//Download linear velocity from bullet engine

	btVector3 vecVelocity = m_rigbody->getLinearVelocity();

	Vector velocity(vecVelocity.x(), vecVelocity.y(), vecVelocity.z());
	
	Vec3BulletToGoldSrc(velocity);

	GetEdict()->v.vuser1 = velocity;
}

void CPlayerObject::StartFrame()
{
	//Upload to bullet engine before simulation
	btTransform trans;
	m_rigbody->getMotionState()->getWorldTransform(trans);
	m_rigbody->setWorldTransform(trans);

	//Do we really need this?
	btVector3 vecVelocity(GetEdict()->v.velocity.x, GetEdict()->v.velocity.y, GetEdict()->v.velocity.z);
	Vector3GoldSrcToBullet(vecVelocity);
	m_rigbody->setLinearVelocity(vecVelocity);
}

void CPlayerObject::StartFrame_Post()
{
	
}

void CPhysicsManager::FreeEntityPrivateData(edict_t* ent)
{
	RemoveGameObject(g_engfuncs.pfnIndexOfEdict(ent));
}

bool CPhysicsManager::SetAbsBox(edict_t *ent)
{
	auto obj = GetGameObject(ent);

	if (obj && obj->IsDynamic())
	{
		auto dynamicobj = (CDynamicObject *)obj;

		btVector3 aabbMins, aabbMaxs;
		dynamicobj->GetRigidBody()->getAabb(aabbMins, aabbMaxs);

		Vector3BulletToGoldSrc(aabbMins);
		Vector3BulletToGoldSrc(aabbMaxs);

		ent->v.absmin.x = aabbMins.getX();
		ent->v.absmin.y = aabbMins.getY();
		ent->v.absmin.z = aabbMins.getZ();
		ent->v.absmax.x = aabbMaxs.getX();
		ent->v.absmax.y = aabbMaxs.getY();
		ent->v.absmax.z = aabbMaxs.getZ();

		//wtf why additional 1 unit ?
		ent->v.absmin.x -= 1;
		ent->v.absmin.y -= 1;
		ent->v.absmin.z -= 1;
		ent->v.absmax.x += 1;
		ent->v.absmax.y += 1;
		ent->v.absmax.z += 1;

		return true;
	}

	return false;
}

bool CPhysicsManager::AddToFullPack(struct entity_state_s *state, int entindex, edict_t *ent, edict_t *host, int hostflags, int player)
{
	auto obj = GetGameObject(ent);

	if (obj)
	{
		if (obj && obj->GetPartialViewerMask())
		{
			int hostindex = g_engfuncs.pfnIndexOfEdict(host);
			if ((obj->GetPartialViewerMask() & (1 << (hostindex - 1))) == 0)
			{
				return false;
			}
		}

		if (obj->GetLevelOfDetailFlags() != 0)
		{
			auto viewent = GetClientViewEntity(host);

			if (viewent)
			{
				float distance = (ent->v.origin - viewent->v.origin).Length();

				obj->ApplyLevelOfDetail(distance, &state->body, &state->modelindex, &state->scale);
			}
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
	if (!m_numDynamicObjects)
		return;

	pmtrace_t trace = { 0 };
	int hitent = pmove->PM_TestPlayerPosition(pmove->origin, &trace);
	if (hitent != -1)
	{
		auto blocker = &pmove->physents[hitent];

		auto body = gPhysicsManager.GetGameObject(blocker->info);

		if (body && body->IsDynamic())
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

void CPhysicsManager::AddGameObject(CGameObject *obj)
{
	obj->AddToPhysicWorld(m_dynamicsWorld, &m_numDynamicObjects);

	m_gameObjects[obj->GetEntIndex()] = obj;

	if (obj->GetEntIndex() > m_maxIndexGameObject)
		m_maxIndexGameObject = obj->GetEntIndex();
}

bool CPhysicsManager::CreateBrushModel(edict_t* ent)
{
	int modelindex = ent->v.modelindex;
	if (modelindex == -1)
	{
		return false;
	}

	if (!m_brushIndexArray[modelindex])
	{
		return false;
	}

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH)) ? true : false;

	auto staticobj = CreateStaticObject(ent, m_worldVertexArray, m_brushIndexArray[modelindex], bKinematic);
	
	if (staticobj)
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

	if (CreatePlayerObject(ent, mass, meshShape, localInertia))
	{
		return true;
	}

	return false;
}

bool CPhysicsManager::ApplyImpulse(edict_t* ent, const Vector& impulse, const Vector& origin)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		//no physic body found
		return false;
	}

	if (!obj->IsDynamic())
	{
		//no dynamic body found
		return false;
	}

	auto dynamicobj = (CDynamicObject *)obj;

	btVector3 vecImpulse;
	impulse.CopyToArray(vecImpulse.m_floats);
	Vector3GoldSrcToBullet(vecImpulse);

	vec3_t relpos;
	relpos = ent->v.origin - origin;

	btVector3 vecRelPos;
	relpos.CopyToArray(vecRelPos.m_floats);
	Vector3GoldSrcToBullet(vecRelPos);

	dynamicobj->GetRigidBody()->applyImpulse(vecImpulse, vecRelPos);

	return true;
}

bool CPhysicsManager::SetEntityLevelOfDetail(edict_t* ent, int flags, int body_0, float scale_0, int body_1, float scale_1, float distance_1, int body_2, float scale_2, float distance_2, int body_3, float scale_3, float distance_3)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		obj->SetLevelOfDetail(flags, body_0, scale_0, body_1, scale_1, distance_1, body_2, scale_2, distance_2, body_3, scale_3, distance_3);

		AddGameObject(obj);
	}
	else
	{
		obj->SetLevelOfDetail(flags, body_0, scale_0, body_1, scale_1, distance_1, body_2, scale_2, distance_2, body_3, scale_3, distance_3);
	}

	return false;
}

bool CPhysicsManager::SetEntityPartialViewer(edict_t* ent, int partial_viewer_mask)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		obj->SetPartialViewer(partial_viewer_mask);

		AddGameObject(obj);
	}
	else
	{
		obj->SetPartialViewer(partial_viewer_mask);
	}

	return false;
}

bool CPhysicsManager::SetEntitySuperPusher(edict_t* ent, bool enable)
{
	auto obj = GetGameObject(ent);

	if(!obj && IsEntitySolidPusher(ent))
	{
		if (CreateBrushModel(ent))
		{
			obj = GetGameObject(ent);
		}
	}

	if (obj)
	{
		if (obj->IsKinematic())
		{
			obj->SetSuperPusher(enable);

			return true;
		}
	}

	return false;
}

bool CPhysicsManager::CreatePhysicBox(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	auto obj = GetGameObject(ent);

	if (obj)
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

	auto dynamicobj = CreateDynamicObject(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable, meshShape, localInertia);

	if (dynamicobj)
	{
		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		Vector3GoldSrcToBullet(vecLinearVelocity);

		dynamicobj->GetRigidBody()->setLinearVelocity(vecLinearVelocity);

		btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		dynamicobj->GetRigidBody()->setAngularVelocity(vecALinearVelocity);

		ent->v.velocity = g_vecZero;
		ent->v.avelocity = g_vecZero;

		return true;
	}

	return false;
}

bool CPhysicsManager::CreatePhysicSphere(edict_t* ent, float mass, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, bool pushable)
{
	auto obj = GetGameObject(ent);

	if (obj)
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

	//mod->flags |= FMODEL_TRACE_HITBOX;//always use hitbox as hull

	float radius = (ent->v.maxs.x - ent->v.mins.x) * 0.5f;

	if (radius <= 0)
	{
		//Must be valid size
		return false;
	}

	FloatGoldSrcToBullet(&radius);

	auto meshShape = new btSphereShape(radius);

	if (mass <= 0)
		mass = 1;

	btVector3 localInertia;
	meshShape->calculateLocalInertia(mass, localInertia);

	auto dynamicobj = CreateDynamicObject(ent, mass, friction, rollingFriction, restitution, ccdRadius, ccdThreshold, pushable, meshShape, localInertia);

	if (dynamicobj)
	{
		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		Vector3GoldSrcToBullet(vecLinearVelocity);

		dynamicobj->GetRigidBody()->setLinearVelocity(vecLinearVelocity);

		btVector3 vecALinearVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y * SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		dynamicobj->GetRigidBody()->setAngularVelocity(vecALinearVelocity);

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
			auto body0 = (btRigidBody *)proxy0->m_clientObject;
			auto body1 = (btRigidBody *)proxy1->m_clientObject;

			auto physobj0 = (CPhysicObject *)body0->getUserPointer();
			auto physobj1 = (CPhysicObject *)body1->getUserPointer();

			if (physobj0->IsKinematic() && physobj0->GetEdict() && physobj0->GetEdict()->v.solid <= SOLID_TRIGGER)
			{
				return false;
			}

			if (physobj1->IsKinematic() && physobj1->GetEdict() && physobj1->GetEdict()->v.solid <= SOLID_TRIGGER)
			{
				return false;
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
	m_dynamicsWorld->getPairCache()->setOverlapFilterCallback(new GameFilterCallback());
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
	return m_numDynamicObjects;
}

CGameObject *CPhysicsManager::GetGameObject(int entindex)
{
	if (entindex >= (int)m_gameObjects.size())
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
		obj->RemoveFromPhysicWorld(m_dynamicsWorld, &m_numDynamicObjects);

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

	if (obj && obj->IsSuperPusher())
	{
		return true;
	}

	return false;
}

bool CPhysicsManager::IsEntityDynamicPhysicObject(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (obj && obj->IsDynamic())
	{
		return true;
	}

	return false;
}