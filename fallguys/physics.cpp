#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <algorithm>

#include "enginedef.h"
#include "serverdef.h"
#include "fallguys.h"

#include "physics.h"
#include "ehandle.h"

extern edict_t* r_worldentity;
extern model_t* r_worldmodel;

//Utils
#ifndef PATHSEPARATOR
#if defined( _WIN32 ) || defined( WIN32 )
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else	//_WIN32
#define PATHSEPARATOR(c) ((c) == '/')
#endif	//_WIN32
#endif

const char* V_GetFileExtension(const char* path)
{
	const char* src;

	src = path + strlen(path) - 1;

	//
	// back up until a . or the start
	//
	while (src != path && *(src - 1) != '.')
		src--;

	// check to see if the '.' is part of a pathname
	if (src == path || PATHSEPARATOR(*src))
	{
		return NULL;  // no extension
	}

	return src;
}

std::string UTIL_GetAbsoluteModelName(model_t* mod)
{
	if (mod->type == mod_brush)
	{
		if (mod->name[0] == '*')
		{
			auto worldmodel = EngineFindWorldModelBySubModel(mod);

			if (worldmodel)
			{
				std::string fullname = worldmodel->name;
				fullname += "/";
				fullname += mod->name;
				return fullname;
			}
		}
	}

	return mod->name;
}

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

//GoldSrcToBullet Scaling

void FloatGoldSrcToBullet(float* v)
{
	(*v) *= G2BScale;
}

void FloatBulletToGoldSrc(float* v)
{
	(*v) *= B2GScale;
}

void TransformGoldSrcToBullet(btTransform& trans)
{
	auto& org = trans.getOrigin();

	org.m_floats[0] *= G2BScale;
	org.m_floats[1] *= G2BScale;
	org.m_floats[2] *= G2BScale;
}

void Vec3GoldSrcToBullet(vec3_t &vec)
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
	vec[0] *= B2GScale;
	vec[1] *= B2GScale;
	vec[2] *= B2GScale;
}

void Vector3BulletToGoldSrc(btVector3& vec)
{
	vec.m_floats[0] *= B2GScale;
	vec.m_floats[1] *= B2GScale;
	vec.m_floats[2] *= B2GScale;
}

Vector GetVectorFromBtVector3(const btVector3 &v)
{
	return Vector(v.getX(), v.getY(), v.getZ());
}

Vector GetBtVector3FromVector(const Vector &v)
{
	return Vector(v.x, v.y, v.z);
}

float GetSignedDistanceToSurface_GoldSrc(const vec3_t &inPoint, const vec3_t &inSurfaceNormal, const float inSurfacePlane)
{
	return DotProduct(inPoint, inSurfaceNormal) - inSurfacePlane;
}

btScalar GetSignedDistanceToSurface(const btVector3 &inPoint, const btVector3 &inSurfaceNormal, const btScalar inSurfacePlane)
{
	return inPoint.dot(inSurfaceNormal) - inSurfacePlane;
}

btScalar CalcVolumeForSphereShape(const btScalar size)
{
	return (4.0f / 3.0f * (float)(M_PI)) * (size * size * size);
}

btScalar CalcVolumeForBoxShape(const btVector3 &size)
{
	return size.getX() * size.getY() * size.getZ();
}

btScalar CalcVolumeForCapsuleShape(btScalar radius, btScalar height)
{
	// Calculate the volume of the cylindrical part
	btScalar cylinderVolume = (float)(M_PI)* radius * radius * height;

	// Calculate the volume of the hemispherical ends
	btScalar hemisphereVolume = (4.0f / 3.0f) * (float)(M_PI)* radius * radius * radius;

	// Sum the volumes to get the total volume
	btScalar totalVolume = cylinderVolume + hemisphereVolume;

	return totalVolume;
}

btScalar CalcVolumeForCylinderShapeX(const btVector3 &halfExtents)
{
	// Calculate the full height and radius of the cylinder
	btScalar height = halfExtents.getX() * 2.0f;
	btScalar radius = halfExtents.getY(); // or halfExtents.getZ(), they should be the same

	// Calculate the volume of the cylinder
	btScalar volume = (float)(M_PI)* radius * radius * height;

	return volume;
}

btScalar CalcVolumeForCylinderShapeY(const btVector3 &halfExtents)
{
	// Calculate the full height and radius of the cylinder
	btScalar height = halfExtents.getY() * 2.0f;
	btScalar radius = halfExtents.getZ(); // or halfExtents.getZ(), they should be the same

	// Calculate the volume of the cylinder
	btScalar volume = (float)(M_PI)* radius * radius * height;

	return volume;
}

btScalar CalcVolumeForCylinderShapeZ(const btVector3 &halfExtents)
{
	// Calculate the full height and radius of the cylinder
	btScalar height = halfExtents.getZ() * 2.0f;
	btScalar radius = halfExtents.getY(); // or halfExtents.getX(), they should be the same

	// Calculate the volume of the cylinder
	btScalar volume = (float)(M_PI)* radius * radius * height;

	return volume;
}

CBulletCollisionShapeSharedUserData* GetSharedUserDataFromCollisionShape(btCollisionShape* pCollisionShape)
{
	return (CBulletCollisionShapeSharedUserData*)pCollisionShape->getUserPointer();
}

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
	pthis->density = 1;
	pthis->maxPendingVelocity = 1000;
}

void PhysicPlayerConfigs_copyctor(PhysicPlayerConfigs *a1, PhysicPlayerConfigs *a2)
{
	a1->mass = a2->mass;
	a1->density = a2->density;
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
	pthis->density = 1;
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
	pthis->centerofmass = g_vecZero;
}

void PhysicObjectParams_copyctor(PhysicObjectParams *a1, PhysicObjectParams *a2)
{
	a1->mass = a2->mass;
	a1->density = a2->density;
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
	a1->centerofmass = a2->centerofmass;
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
	pthis->suspensionLowerLimit = -1;
	pthis->suspensionUpperLimit = 1;
	pthis->rayCastHeight = 16;
	pthis->flags = 0;
	pthis->index = 0;
	pthis->springIndex = 2;
	pthis->engineIndex = 3;
	pthis->steerIndex = 5;
}

void PhysicWheelParams_copyctor(PhysicWheelParams *a1, PhysicWheelParams *a2)
{
	a1->ent = a2->ent;
	a1->connectionPoint = a2->connectionPoint;
	a1->wheelDirection = a2->wheelDirection;
	a1->wheelAxle = a2->wheelAxle;
	a1->suspensionStiffness = a2->suspensionStiffness;
	a1->suspensionDamping = a2->suspensionDamping;
	a1->suspensionLowerLimit = a2->suspensionLowerLimit;
	a1->suspensionUpperLimit = a2->suspensionUpperLimit;
	a1->rayCastHeight = a2->rayCastHeight;
	a1->flags = a2->flags;
	a1->index = a2->index;
	a1->springIndex = a2->springIndex;
	a1->engineIndex = a2->engineIndex;
	a1->steerIndex = a2->steerIndex;
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
	pthis->type = 0;
	pthis->flags = 0;
	pthis->idleEngineForce = 0;
	pthis->idleSteeringForce = 0;
	pthis->idleSteeringSpeed = 0;
}

void PhysicVehicleParams_copyctor(PhysicVehicleParams *a1, PhysicVehicleParams *a2)
{
	a1->type = a2->type;
	a1->flags = a2->flags;
	a1->idleEngineForce = a2->idleEngineForce;
	a1->idleSteeringForce = a2->idleSteeringForce;
	a1->idleSteeringSpeed = a2->idleSteeringSpeed;
}

PhysicVehicleParams * SC_SERVER_DECL PhysicVehicleParams_opassign(PhysicVehicleParams *a1, SC_SERVER_DUMMYARG PhysicVehicleParams *a2)
{
	PhysicVehicleParams_copyctor(a1, a2);

	return a1;
}

void PhysicVehicleParams_dtor(PhysicVehicleParams *pthis)
{

}

void PhysicWheelRuntimeInfo_ctor(PhysicWheelRuntimeInfo *pthis)
{
	pthis->hitGround = 0;
	pthis->hitNormalInWorld = g_vecZero;
	pthis->hitPointInWorld = g_vecZero;
	pthis->rpm = 0;
	pthis->waterVolume = 0;
	pthis->totalVolume = 0;
}

void PhysicWheelRuntimeInfo_copyctor(PhysicWheelRuntimeInfo *a1, PhysicWheelRuntimeInfo *a2)
{
	a1->hitGround = a2->hitGround;
	a1->hitNormalInWorld = a2->hitNormalInWorld;
	a1->hitPointInWorld = a2->hitPointInWorld;
	a1->rpm = a2->rpm;
	a1->waterVolume = a2->waterVolume;
	a1->totalVolume = a2->totalVolume;
}

PhysicWheelRuntimeInfo * SC_SERVER_DECL PhysicWheelRuntimeInfo_opassign(PhysicWheelRuntimeInfo *a1, SC_SERVER_DUMMYARG PhysicWheelRuntimeInfo *a2)
{
	PhysicWheelRuntimeInfo_copyctor(a1, a2);

	return a1;
}

void PhysicWheelRuntimeInfo_dtor(PhysicWheelRuntimeInfo *pthis)
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

PhysicWheelRuntimeInfo::PhysicWheelRuntimeInfo()
{
	PhysicWheelRuntimeInfo_ctor(this);
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

	m_gravityAcceleration = 0;
	m_maxIndexGameObject = 0;

	m_solidPlayerMask = 0;
	m_simrate = 1 / 60.0f;
	m_playerMass = 20;
	m_playerMaxPendingVelocity = 1000;

	m_CurrentImpactImpulse = 0;
	m_CurrentImpactPoint = g_vecZero;
	m_CurrentImpactDirection = g_vecZero;
	m_CurrentImpactEntity = NULL;

	m_bEnabled = false;
}

void CPhysicsManager::BuildSurfaceDisplayList(model_t *mod, msurface_t* fa, std::deque<glpoly_t*> &glpolys)
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

	glpolys.push_front(poly);

	poly->next = NULL;
	poly->flags = fa->flags;
	//fa->polys = poly;
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

std::shared_ptr<CPhysicVertexArray> CPhysicsManager::GenerateWorldVertexArray(model_t* mod)
{
	std::string worldModelName;

	if (mod->name[0] == '*')
	{
		auto worldmodel = EngineFindWorldModelBySubModel(mod);

		if (!worldmodel)
		{
			ALERT(at_console, "CBasePhysicManager::GenerateWorldVertexArray: Failed to find worldmodel for submodel \"%s\"!\n", mod->name);
			return nullptr;
		}

		worldModelName = worldmodel->name;
	}
	else
	{
		worldModelName = mod->name;
	}

	auto found = m_worldVertexResources.find(worldModelName);

	if (found != m_worldVertexResources.end())
	{
		return found->second;
	}

	auto worldVertexArray = std::make_shared<CPhysicVertexArray>();

	CPhysicBrushVertex Vertexes[3];

	int iNumFaces = 0;
	int iNumVerts = 0;

	worldVertexArray->vFaceBuffer.resize(mod->numsurfaces);

	for (int i = 0; i < mod->numsurfaces; i++)
	{
		auto surf = EngineGetSurfaceByIndex(mod, i);

		if ((surf->flags & (SURF_DRAWTURB | SURF_UNDERWATER | SURF_DRAWSKY)))
			continue;

		std::deque<glpoly_t*> glpolys;

		BuildSurfaceDisplayList(mod, surf, glpolys);

		if (glpolys.empty())
			continue;

		auto brushface = &worldVertexArray->vFaceBuffer[i];

		int iStartVert = iNumVerts;

		brushface->start_vertex = iStartVert;
		brushface->plane_normal = surf->plane->normal;
		brushface->plane_dist = surf->plane->dist;
		brushface->plane_flags = surf->flags;

		if (surf->flags & SURF_PLANEBACK)
		{
			brushface->plane_normal = brushface->plane_normal * (-1);
			brushface->plane_dist = brushface->plane_dist * (-1);
		}

		for (const auto& poly : glpolys)
		{
			auto v = poly->verts[0];

			for (int j = 0; j < 3; j++, v += VERTEXSIZE)
			{
				Vertexes[j].pos[0] = v[0];
				Vertexes[j].pos[1] = v[1];
				Vertexes[j].pos[2] = v[2];

				Vec3GoldSrcToBullet(Vertexes[j].pos);
			}

			worldVertexArray->vVertexBuffer.emplace_back(Vertexes[0]);
			worldVertexArray->vVertexBuffer.emplace_back(Vertexes[1]);
			worldVertexArray->vVertexBuffer.emplace_back(Vertexes[2]);

			iNumVerts += 3;

			for (int j = 0; j < (poly->numverts - 3); j++, v += VERTEXSIZE)
			{
				Vertexes[1] = Vertexes[2];

				Vertexes[2].pos[0] = v[0];
				Vertexes[2].pos[1] = v[1];
				Vertexes[2].pos[2] = v[2];

				Vec3GoldSrcToBullet(Vertexes[2].pos);

				worldVertexArray->vVertexBuffer.emplace_back(Vertexes[0]);
				worldVertexArray->vVertexBuffer.emplace_back(Vertexes[1]);
				worldVertexArray->vVertexBuffer.emplace_back(Vertexes[2]);

				iNumVerts += 3;
			}
		}

		for (auto& poly : glpolys)
		{
			free(poly);
		}

		brushface->num_vertexes = iNumVerts - iStartVert;
	}

	//Always shrink to save system memory
	worldVertexArray->vVertexBuffer.shrink_to_fit();

	m_worldVertexResources[worldModelName] = worldVertexArray;

	return worldVertexArray;
}

std::shared_ptr<CPhysicIndexArray> CPhysicsManager::GenerateBrushIndexArray(model_t* mod, const std::shared_ptr<CPhysicVertexArray>& pWorldVertexArray)
{
	auto pIndexArray = std::make_shared<CPhysicIndexArray>();
	pIndexArray->flags |= PhysicIndexArrayFlag_FromBSP;
	pIndexArray->pVertexArray = pWorldVertexArray;

	GenerateIndexArrayForBrushModel(mod, pIndexArray.get());

	auto name = UTIL_GetAbsoluteModelName(mod);

	m_indexArrayResources[name] = pIndexArray;

	return pIndexArray;
}

void CPhysicsManager::FreeAllIndexArrays(int withflags, int withoutflags)
{
	for (auto itor = m_indexArrayResources.begin(); itor != m_indexArrayResources.end(); )
	{
		const auto& pIndexArray = itor->second;

		if ((pIndexArray->flags & withflags) && !(pIndexArray->flags & withoutflags))
		{
			itor = m_indexArrayResources.erase(itor);
			continue;
		}

		itor++;
	}
}

void CPhysicsManager::GenerateIndexArrayForBrushModel(model_t* mod, CPhysicIndexArray* pIndexArray)
{
	if (mod == r_worldmodel)
	{
		GenerateIndexArrayRecursiveWorldNode(mod, mod->nodes, pIndexArray);
	}
	else
	{
		for (int i = 0; i < mod->nummodelsurfaces; i++)
		{
			auto surf = EngineGetSurfaceByIndex(mod, mod->firstmodelsurface + i);

			GenerateIndexArrayForSurface(mod, surf, pIndexArray);
		}
	}

	//Always shrink to save system memory
	pIndexArray->vIndexBuffer.shrink_to_fit();
}

void CPhysicsManager::GenerateIndexArrayForSurface(model_t* mod, msurface_t* surf, CPhysicIndexArray* pIndexArray)
{
	if (surf->flags & SURF_DRAWTURB)
	{
		return;
	}

	if (surf->flags & SURF_DRAWSKY)
	{
		return;
	}

	if (surf->flags & SURF_UNDERWATER)
	{
		return;
	}

	auto surfIndex = EngineGetSurfaceIndex(mod, surf);

	GenerateIndexArrayForBrushface(&pIndexArray->pVertexArray->vFaceBuffer[surfIndex], pIndexArray);
}

void CPhysicsManager::GenerateIndexArrayRecursiveWorldNode(model_t *mod, mnode_t* node, CPhysicIndexArray* pIndexArray)
{
	if (node->contents == CONTENTS_SOLID)
		return;

	if (node->contents < 0)
		return;

	GenerateIndexArrayRecursiveWorldNode(mod, node->children[0], pIndexArray);

	for (int i = 0; i < node->numsurfaces; ++i)
	{
		auto surf = EngineGetSurfaceByIndex(mod, node->firstsurface + i);

		GenerateIndexArrayForSurface(mod, surf, pIndexArray);
	}

	GenerateIndexArrayRecursiveWorldNode(mod, node->children[1], pIndexArray);
}

void CPhysicsManager::GenerateIndexArrayForBrushface(CPhysicBrushFace* brushface, CPhysicIndexArray* pIndexArray)
{
	int first = -1;
	int prv0 = -1;
	int prv1 = -1;
	int prv2 = -1;

	for (int i = 0; i < brushface->num_vertexes; i++)
	{
		if (prv0 != -1 && prv1 != -1 && prv2 != -1)
		{
			pIndexArray->vIndexBuffer.emplace_back(brushface->start_vertex + first);
			pIndexArray->vIndexBuffer.emplace_back(brushface->start_vertex + prv2);
		}

		pIndexArray->vIndexBuffer.emplace_back(brushface->start_vertex + i);

		if (first == -1)
			first = i;

		prv0 = prv1;
		prv1 = prv2;
		prv2 = i;
	}

	pIndexArray->vFaceBuffer.emplace_back(*brushface);
}

std::shared_ptr<CPhysicIndexArray> CPhysicsManager::LoadIndexArrayFromResource(const std::string& resourcePath)
{
	auto found = m_indexArrayResources.find(resourcePath);

	if (found != m_indexArrayResources.end())
	{
		return found->second;
	}

	const auto extension = V_GetFileExtension(resourcePath.c_str());

	if (0 == strcasecmp(extension, "obj"))
	{
		//really needis shit ???
	}

	ALERT(at_console, "LoadIndexArrayFromResource: Could not load \"%s\", unsupported file extension!\n", resourcePath.c_str());
	return nullptr;
}

CDynamicObject* CPhysicsManager::CreateDynamicObject(CGameObject *obj, btCollisionShape* collisionShape, const btVector3& localInertia, float mass, float density, float friction, float rollingFriction, float restitution, float ccdRadius, float ccdThreshold, int flags)
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

	dynamicobj->SetDensity(density);

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
		return;

	if (!GetPhysicObject()->GetGameObject())
		return;

	auto ent = GetPhysicObject()->GetGameObject()->GetEdict();

	if (!ent || ent->free)
		return;

	auto &vecOrigin = worldTrans.getOrigin();

	Vector origin = GetVectorFromBtVector3(vecOrigin);

	btVector3 vecAngles;
	MatrixEuler(worldTrans.getBasis(), vecAngles);

	Vector angles = GetVectorFromBtVector3(vecAngles);

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
	if (!m_bEnabled)
		return;

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

				CreatePhysicObjectForBrushModel(ent);
				continue;
			}

			if (IsEntitySolidPlayer(i, ent))
			{
				CreateSolidPlayer(ent);
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
				CreatePhysicObjectForBrushModel(ent);
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
	if (!m_bEnabled)
		return;

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
	if (!m_bEnabled)
		return;

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

		if (ent->v.gravity > 0)
		{
			GetRigidBody()->setGravity(btVector3(0, 0, (-1.0f) * gPhysicsManager.GetGravityAcceleration() * ent->v.gravity));
		}
	}

	if (GetVehicleBehaviour())
	{
		GetVehicleBehaviour()->StartFrame(world);
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

		Vector vel = GetVectorFromBtVector3(vecLinearVelocity);

		//TODO: convert to degree based ?
		Vector avel = GetVectorFromBtVector3(vecAngularVelocity);

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

	if (GetVehicleBehaviour())
	{
		GetVehicleBehaviour()->StartFrame_Post(world);
	}
}

void CDynamicObject::EndFrame(btDiscreteDynamicsWorld* world)
{
	if (GetVehicleBehaviour())
	{
		GetVehicleBehaviour()->EndFrame(world);
	}
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
	/*if (freeze)
	{
		auto ent = GetGameObject()->GetEdict();
		ent->v.vuser1 = g_vecZero;
		ent->v.vuser2 = g_vecZero;

		GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		GetRigidBody()->setActivationState(WANTS_DEACTIVATION);
	}
	else if (!freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		GetRigidBody()->setActivationState(ACTIVE_TAG);
	}*/

	GetGameObject()->GetEdict()->v.vuser1 = g_vecZero;
	GetGameObject()->GetEdict()->v.vuser2 = g_vecZero;
	GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));

	if (freeze)
	{
		GetRigidBody()->setActivationState(DISABLE_SIMULATION);
	}
	else if (!freeze && GetRigidBody()->getActivationState() == DISABLE_SIMULATION)
	{
		GetRigidBody()->forceActivationState(ACTIVE_TAG);
	}
}

void CDynamicObject::SetPhysicNoCollision(bool no_collision)
{
	if (no_collision)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}


void CClippingHullObject::SetPhysicTransform(const Vector &origin, const Vector &angles)
{

}

void CClippingHullObject::SetPhysicNoCollision(bool no_collision)
{
	if (no_collision)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

void CClippingHullObject::SetPhysicFreeze(bool freeze)
{
/*	if (freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else if (!freeze)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		GetRigidBody()->forceActivationState(ACTIVE_TAG);
	}*/

	GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));

	if (freeze)
	{
		GetRigidBody()->setActivationState(DISABLE_SIMULATION);
	}
	else if (!freeze && GetRigidBody()->getActivationState() == DISABLE_SIMULATION)
	{
		GetRigidBody()->forceActivationState(ACTIVE_TAG);
	}
}

void CPlayerObject::SetPhysicNoCollision(bool no_collision)
{
	if (no_collision)
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		GetRigidBody()->setCollisionFlags(GetRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
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

			new_origin = GetVectorFromBtVector3(vecNewOrigin);

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

					new_velocity = GetVectorFromBtVector3(vecLinearVelocity);

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
				btVector3 vecLinearVelocity = GetRigidBody()->getLinearVelocity();

				auto new_velocity = GetVectorFromBtVector3(vecLinearVelocity);

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

	ent->v.absmin = GetVectorFromBtVector3(aabbMins);
	ent->v.absmax = GetVectorFromBtVector3(aabbMaxs);

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
		m_flImpactImpulse = impulse;
		m_ImpactPoint = GetVectorFromBtVector3(worldpos_on_source);
		m_ImpactDirection = GetVectorFromBtVector3(normal);
		m_ImpactEntity = hitent;
	}
}

void CPlayerObject::DispatchImpact(float impulse, const btVector3 &worldpos_on_source, const btVector3 &worldpos_on_hit, const btVector3 &normal, edict_t *hitent)
{
	if (impulse > m_flImpactImpulse)
	{
		m_flImpactImpulse = impulse;
		m_ImpactPoint = GetVectorFromBtVector3(worldpos_on_source);
		m_ImpactDirection = GetVectorFromBtVector3(normal);
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

bool CPhysicsManager::IsPointInsideBrushEntity(edict_t *ent, const vec3_t &p)
{
	auto pIndexArray = GetIndexArrayFromBrushEntity(ent);

	if (!pIndexArray)
		return false;

	bool bIsInside = true;

	for (const auto& face : pIndexArray->vFaceBuffer)
	{
		float dist_to_plane = GetSignedDistanceToSurface_GoldSrc(p, face.plane_normal, face.plane_dist);

		if (dist_to_plane > 0)
		{
			bIsInside = false;
			break;
		}
	}

	return bIsInside;
}

void CGhostPhysicObject::StartFrame_Post(btDiscreteDynamicsWorld* world)
{
	auto ent = GetGameObject()->GetEdict();

	btAlignedObjectArray<btRigidBody *> ObjectArray;
	btManifoldArray ManifoldArray;
	btBroadphasePairArray& PairArray = GetGhostObject()->getOverlappingPairCache()->getOverlappingPairArray();

	for (int i = 0; i < PairArray.size(); i++)
	{
		ManifoldArray.clear();

		auto CollisionPair = world->getPairCache()->findPair(PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

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
					auto rigidbody = (btRigidBody *)btRigidBody::upcast(ManifoldArray[j]->getBody0());

					if (!rigidbody)
					{
						rigidbody = (btRigidBody *)btRigidBody::upcast(ManifoldArray[j]->getBody1());
					}

					if (rigidbody)
					{
						int index = ObjectArray.findLinearSearch(rigidbody);
						if (index == ObjectArray.size())
						{
							//not found
							ObjectArray.push_back(rigidbody);
						}
					}
				}
			}
		}
	}

	auto localInvTrans = GetGhostObject()->getWorldTransform().inverse();

	if (GetGhostObject()->getCollisionShape()->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
	{
		auto pIndexArray = gPhysicsManager.GetIndexArrayFromBrushEntity(ent);

		if (pIndexArray)
		{
			for (int i = 0; i < GetGhostObject()->getNumOverlappingObjects(); i++)
			{
				auto rigidbody = (btRigidBody *)btRigidBody::upcast(GetGhostObject()->getOverlappingObject(i));

				if (rigidbody)
				{
					int index = ObjectArray.findLinearSearch(rigidbody);
					if (index == ObjectArray.size())
					{
						auto vPoint = (rigidbody->getWorldTransform() * localInvTrans).getOrigin();

						vec3_t vecPoint = GetVectorFromBtVector3(vPoint);

						bool bIsInside = true;

						for (const auto &face : pIndexArray->vFaceBuffer)
						{
							float dist_to_plane = GetSignedDistanceToSurface_GoldSrc(vecPoint, face.plane_normal, face.plane_dist);

							if (dist_to_plane > 0)
							{
								bIsInside = false;
								break;
							}
						}

						if(bIsInside)
						{
							ObjectArray.push_back(rigidbody);
						}
					}
				}
			}
		}
	}
	else if (GetGhostObject()->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE)
	{
		auto boxShape = (btBoxShape *)GetGhostObject()->getCollisionShape();

		for (int i = 0; i < GetGhostObject()->getNumOverlappingObjects(); i++)
		{
			auto rigidbody = (btRigidBody *)btRigidBody::upcast(GetGhostObject()->getOverlappingObject(i));

			if (rigidbody)
			{
				int index = ObjectArray.findLinearSearch(rigidbody);

				if (index == ObjectArray.size())
				{
					auto vPoint = (rigidbody->getWorldTransform() * localInvTrans).getOrigin();

					if (boxShape->isInside(vPoint, 0))
					{
						ObjectArray.push_back(rigidbody);
					}
				}
			}
		}
	}

	for (int i = 0; i < ObjectArray.size(); i++)
	{
		OnTouchRigidBody(world, ObjectArray.at(i));
	}
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

void CSolidOptimizerGhostPhysicObject::OnTouchRigidBody(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
{
	auto physobj = (CPhysicObject *)rigidbody->getUserPointer();

	if (physobj->IsPlayerObject())
	{
		int playerIndex = physobj->GetGameObject()->GetEntIndex();

		GetGameObject()->RemovePlayerSemiClipMask((1 << (playerIndex - 1)));
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

/// This class calculates the intersection between a fluid surface and a polyhedron and returns the submerged volume and its center of buoyancy
/// Construct this class and then one by one add all faces of the polyhedron using the AddFace function. After all faces have been added the result
/// can be gotten through GetResult.
class PolyhedronSubmergedVolumeCalculator
{
private:
	// Calculate submerged volume * 6 and center of mass * 4 for a tetrahedron with 4 vertices submerged
	// inV1 .. inV4 are submerged
	inline static void	sTetrahedronVolume4(const btVector3 & inV1, const btVector3 & inV2, const btVector3 & inV3, const btVector3 & inV4, btScalar &outVolumeTimes6, btVector3 &outCenterTimes4)
	{
		// Calculate center of mass and mass of this tetrahedron,
		// see: https://en.wikipedia.org/wiki/Tetrahedron#Volume
		outVolumeTimes6 = (inV1 - inV4).dot((inV2 - inV4).cross(inV3 - inV4)); // All contributions should be positive because we use a reference point that is on the surface of the hull
		if (outVolumeTimes6 < 0)
			outVolumeTimes6 = 0;
		outCenterTimes4 = inV1 + inV2 + inV3 + inV4;
	}

	// Get the intersection point with a plane.
	// inV1 is inD1 distance away from the plane, inV2 is inD2 distance away from the plane
	inline static btVector3	sGetPlaneIntersection(const btVector3 &inV1, btScalar inD1, const btVector3 &inV2, float inD2)
	{
		btScalar delta = inD1 - inD2;
		if (abs(delta) < 1.0e-6f)
			return inV1; // Parallel to plane, just pick a point
		else
			return inV1 + inD1 * (inV2 - inV1) / delta;
	}

	// Calculate submerged volume * 6 and center of mass * 4 for a tetrahedron with 1 vertex submerged
	// inV1 is submerged, inV2 .. inV4 are not
	// inD1 .. inD4 are the distances from the points to the plane
	inline static void sTetrahedronVolume1(const btVector3 & inV1, btScalar inD1, const btVector3 & inV2, float inD2, const btVector3 & inV3, btScalar inD3, const btVector3 & inV4, btScalar inD4, btScalar &outVolumeTimes6, btVector3 &outCenterTimes4)
	{
		// A tetrahedron with 1 point submerged is cut along 3 edges forming a new tetrahedron
		btVector3 v2 = sGetPlaneIntersection(inV1, inD1, inV2, inD2);
		btVector3 v3 = sGetPlaneIntersection(inV1, inD1, inV3, inD3);
		btVector3 v4 = sGetPlaneIntersection(inV1, inD1, inV4, inD4);

		sTetrahedronVolume4(inV1, v2, v3, v4, outVolumeTimes6, outCenterTimes4);
	}

	// Calculate submerged volume * 6 and center of mass * 4 for a tetrahedron with 2 vertices submerged
	// inV1, inV2 are submerged, inV3, inV4 are not
	// inD1 .. inD4 are the distances from the points to the plane
	inline static void sTetrahedronVolume2(const btVector3 &  inV1, btScalar inD1, const btVector3 &  inV2, float inD2, const btVector3 &  inV3, btScalar inD3, const btVector3 &  inV4, btScalar inD4, btScalar &outVolumeTimes6, btVector3 &outCenterTimes4)
	{
		// A tetrahedron with 2 points submerged is cut along 4 edges forming a quad
		btVector3 c = sGetPlaneIntersection(inV1, inD1, inV3, inD3);
		btVector3 d = sGetPlaneIntersection(inV1, inD1, inV4, inD4);
		btVector3 e = sGetPlaneIntersection(inV2, inD2, inV4, inD4);
		btVector3 f = sGetPlaneIntersection(inV2, inD2, inV3, inD3);

		// We pick point c as reference (which is on the cut off surface)
		// This leaves us with three tetrahedrons to sum up (any faces that are in the same plane as c will have zero volume)
		btVector3 center1, center2, center3;
		btScalar volume1, volume2, volume3;
		sTetrahedronVolume4(e, f, inV2, c, volume1, center1);
		sTetrahedronVolume4(e, inV1, d, c, volume2, center2);
		sTetrahedronVolume4(e, inV2, inV1, c, volume3, center3);

		// Tally up the totals
		outVolumeTimes6 = volume1 + volume2 + volume3;
		outCenterTimes4 = outVolumeTimes6 > 0.0f ? (volume1 * center1 + volume2 * center2 + volume3 * center3) / outVolumeTimes6 : btVector3(0, 0, 0);
	}

	// Calculate submerged volume * 6 and center of mass * 4 for a tetrahedron with 3 vertices submerged
	// inV1, inV2, inV3 are submerged, inV4 is not
	// inD1 .. inD4 are the distances from the points to the plane
	inline static void sTetrahedronVolume3(const btVector3 &inV1, float inD1, const btVector3 & inV2, float inD2, const btVector3 & inV3, float inD3, const btVector3 & inV4, float inD4, float &outVolumeTimes6, btVector3 &outCenterTimes4)
	{
		// A tetrahedron with 1 point above the surface is cut along 3 edges forming a new tetrahedron
		btVector3 v1 = sGetPlaneIntersection(inV1, inD1, inV4, inD4);
		btVector3 v2 = sGetPlaneIntersection(inV2, inD2, inV4, inD4);
		btVector3 v3 = sGetPlaneIntersection(inV3, inD3, inV4, inD4);

		btVector3 dry_center, total_center;
		btScalar dry_volume, total_volume;

		// We first calculate the part that is above the surface
		sTetrahedronVolume4(v1, v2, v3, inV4, dry_volume, dry_center);

		// Calculate the total volume
		sTetrahedronVolume4(inV1, inV2, inV3, inV4, total_volume, total_center);

		// From this we can calculate the center and volume of the submerged part
		outVolumeTimes6 = total_volume - dry_volume;
		if (outVolumeTimes6 < 0)
			outVolumeTimes6 = 0;
		outCenterTimes4 = outVolumeTimes6 > 0.0f ? (total_center * total_volume - dry_center * dry_volume) / outVolumeTimes6 : btVector3(0, 0, 0);
	}

public:
	/// A helper class that contains cached information about a polyhedron vertex
	class Point
	{
	public:
		btVector3			mPosition;						///< World space position of vertex
		btScalar			mDistanceToSurface;				///< Signed distance to the surface (> 0 is above, < 0 is below)
		bool				mAboveSurface;					///< If the point is above the surface (mDistanceToSurface > 0)
	};

	/// Constructor
	/// @param inTransform Transform to transform all incoming points with
	/// @param inPoints Array of points that are part of the polyhedron
	/// @param inPointStride Amount of bytes between each point (should usually be sizeof(Vec3))
	/// @param inNumPoints The amount of points
	/// @param inSurface The plane that forms the fluid surface (normal should point up)
	/// @param ioBuffer A temporary buffer of Point's that should have inNumPoints entries and should stay alive while this class is alive

	PolyhedronSubmergedVolumeCalculator(const btTransform &inTransform, const btVector3 *inPoints, int inPointStride, int inNumPoints, const btVector3 &inSurfaceNormal, const btScalar inSurfacePlane, Point *ioBuffer) : mPoints(ioBuffer)
	{
		mAllAbove = true;
		mAllBelow = true;
		mReferencePointIdx = 0;
		mSubmergedVolume = 0;
		mCenterOfBuoyancy = btVector3(0, 0, 0);

		// Convert the points to world space and determine the distance to the surface
		float reference_dist = FLT_MAX;
		for (int p = 0; p < inNumPoints; ++p)
		{
			// Calculate values
			btVector3 local_point = *reinterpret_cast<const btVector3 *>(reinterpret_cast<const unsigned char *>(inPoints) + p * inPointStride);
			btVector3 transformed_point = inTransform * local_point;

			auto dist = GetSignedDistanceToSurface(transformed_point, inSurfaceNormal, inSurfacePlane);

			bool above = dist >= 0.0f;

			// Keep track if all are above or below
			mAllAbove &= above;
			mAllBelow &= !above;

			// Calculate lowest point, we use this to create tetrahedrons out of all faces
			if (reference_dist > dist)
			{
				mReferencePointIdx = p;
				reference_dist = dist;
			}

			// Store values
			ioBuffer->mPosition = transformed_point;
			ioBuffer->mDistanceToSurface = dist;
			ioBuffer->mAboveSurface = above;
			++ioBuffer;
		}
	}

	/// Check if all points are above the surface. Should be used as early out.
	inline bool AreAllAbove() const
	{
		return mAllAbove;
	}

	/// Check if all points are below the surface. Should be used as early out.
	inline bool AreAllBelow() const
	{
		return mAllBelow;
	}

	/// Get the lowest point of the polyhedron. Used to form the 4th vertex to make a tetrahedron out of a polyhedron face.
	inline int GetReferencePointIdx() const
	{
		return mReferencePointIdx;
	}

	/// Add a polyhedron face. Supply the indices of the points that form the face (in counter clockwise order).
	void AddFace(int inIdx1, int inIdx2, int inIdx3)
	{
		// Find the points
		const Point &ref = mPoints[mReferencePointIdx];
		const Point &p1 = mPoints[inIdx1];
		const Point &p2 = mPoints[inIdx2];
		const Point &p3 = mPoints[inIdx3];

		// Determine which vertices are submerged
		unsigned int code = (p1.mAboveSurface ? 0 : 0b001) | (p2.mAboveSurface ? 0 : 0b010) | (p3.mAboveSurface ? 0 : 0b100);

		float volume;
		btVector3 center;
		switch (code)
		{
		case 0b000:
			// One point submerged
			sTetrahedronVolume1(ref.mPosition, ref.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, volume, center);
			break;

		case 0b001:
			// Two points submerged
			sTetrahedronVolume2(ref.mPosition, ref.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, volume, center);
			break;

		case 0b010:
			// Two points submerged
			sTetrahedronVolume2(ref.mPosition, ref.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, volume, center);
			break;

		case 0b100:
			// Two points submerged
			sTetrahedronVolume2(ref.mPosition, ref.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, volume, center);
			break;

		case 0b011:
			// Three points submerged
			sTetrahedronVolume3(ref.mPosition, ref.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, volume, center);
			break;

		case 0b101:
			// Three points submerged
			sTetrahedronVolume3(ref.mPosition, ref.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, volume, center);
			break;

		case 0b110:
			// Three points submerged
			sTetrahedronVolume3(ref.mPosition, ref.mDistanceToSurface, p3.mPosition, p3.mDistanceToSurface, p2.mPosition, p2.mDistanceToSurface, p1.mPosition, p1.mDistanceToSurface, volume, center);
			break;

		case 0b111:
			// Four points submerged
			sTetrahedronVolume4(ref.mPosition, p3.mPosition, p2.mPosition, p1.mPosition, volume, center);
			break;

		default:
			// Should not be possible
			volume = 0.0f;
			center = btVector3(0, 0, 0);
			break;
		}

		mSubmergedVolume += volume;
		mCenterOfBuoyancy += volume * center;
	}

	/// Call after all faces have been added. Returns the submerged volume and the center of buoyancy for the submerged volume.
	void GetResult(btScalar &outSubmergedVolume, btVector3 &outCenterOfBuoyancy) const
	{
		outCenterOfBuoyancy = mSubmergedVolume > 0.0f ? mCenterOfBuoyancy / (4.0f * mSubmergedVolume) : btVector3(0, 0, 0); // Do this before dividing submerged volume by 6 to get correct weight factor
		outSubmergedVolume = mSubmergedVolume / 6.0f;
	}

private:
	// The precalculated points for this polyhedron
	const Point *		mPoints;

	// If all points are above/below the surface
	bool				mAllBelow;
	bool				mAllAbove;

	// The lowest point
	int						mReferencePointIdx;

	// Aggregator for submerged volume and center of buoyancy
	btScalar				mSubmergedVolume;
	btVector3				mCenterOfBuoyancy;

};


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

bool CPhysicsManager::ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	auto gameObjTouched = GetGameObject(pentTouched);

	if (gameObjTouched && gameObjTouched->IsSemiClipToEntity(pentOther))
	{
		return false;
	}

	return true;
}

bool CPhysicsManager::PM_ShouldCollide(int entindex)
{
	if (entindex > 0)
	{
		auto gameObj = GetGameObject(entindex);

		if (gameObj)
		{
			int playerIndex = pmove->player_index + 1;

			if (gameObj->IsSemiClipToEntity(playerIndex) || gameObj->IsPMSemiClipToEntity(playerIndex))
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

bool CPhysicsManager::CreatePhysicObjectForBrushModel(edict_t* ent)
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

	auto pCollisionShape = CreateTriMeshShapeFromBrushEntity(ent);

	if (!pCollisionShape)
	{
		return false;
	}

	bool bKinematic = ((ent != r_worldentity) && (ent->v.movetype == MOVETYPE_PUSH && ent->v.solid == SOLID_BSP)) ? true : false;

	auto staticObject = CreateStaticObject(obj, pCollisionShape, bKinematic);
	
	obj->AddPhysicObject(staticObject, m_dynamicsWorld);

	if (ent->v.flags & FL_CONVEYOR)
	{
		staticObject->GetRigidBody()->setCollisionFlags(staticObject->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	return true;
}

bool CPhysicsManager::CreateSolidPlayer(edict_t* ent)
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

		obj->AddPhysicObject(playerobj, m_dynamicsWorld);
	}

	if (1)
	{
		auto shape = new btBoxShape(btVector3(hull_duck.x + 4, hull_duck.y + 4, hull_duck.z - 1.0f));

		auto playerobj = CreatePlayerObject(obj, shape, 18, 400 * m_simrate, true);

		obj->AddPhysicObject(playerobj, m_dynamicsWorld);
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

btHinge2Constraint * CPhysicVehicleBehaviour::GetWheelConstraint(int index)
{
	if (index < 0 || index >= (int)m_wheelConstraints.size())
		return NULL;

	return m_wheelConstraints[index];
}

void CPhysicVehicleBehaviour::SetWheelConstraint(int index, btHinge2Constraint *pConstraint)
{
	if (index >= (int)m_wheelConstraints.size())
	{
		m_wheelConstraints.resize(index + 1);
	}
	m_wheelConstraints[index] = pConstraint;
}

bool CPhysicVehicleBehaviour::GetVehicleWheelRuntimeInfo(int wheelIndex, PhysicWheelRuntimeInfo *RuntimeInfo)
{
	auto constraint = GetWheelConstraint(wheelIndex);

	if (constraint && constraint->getUserConstraintType() == ConstraintType_Wheel)
	{
		auto wheelInfo = (CPhysicVehicleWheelInfo *)constraint->getUserConstraintPtr();

		auto wheelPhysObject = (CCollisionPhysicObject *)constraint->getRigidBodyB().getUserPointer();

		RuntimeInfo->hitGround = wheelInfo->m_hitGround;
		RuntimeInfo->hitNormalInWorld = GetVectorFromBtVector3(wheelInfo->m_hitNormalInWorld);
		RuntimeInfo->hitPointInWorld = GetVectorFromBtVector3(wheelInfo->m_hitPointInWorld);
		RuntimeInfo->rpm = constraint->getRigidBodyB().getAngularVelocity().length() * SIMD_DEGS_PER_RAD * 60.0f / 360.0f;

		RuntimeInfo->waterVolume = wheelPhysObject->GetWaterVolume();
		RuntimeInfo->totalVolume = wheelPhysObject->GetTotalVolume();

		return true;
	}

	return false;
}

void CPhysicVehicleBehaviour::DoRaycasts(btDiscreteDynamicsWorld* world)
{
	for (size_t i = 0; i < m_wheelConstraints.size(); i++)
	{
		auto constraint = GetWheelConstraint(i);

		if (constraint)
		{
			if (constraint && constraint->getUserConstraintType() == ConstraintType_Wheel)
			{
				auto wheelInfo = (CPhysicVehicleWheelInfo *)constraint->getUserConstraintPtr();

				wheelInfo->m_hitGround = false;

				auto &wheel = constraint->getRigidBodyB();

				btVector3 from = wheel.getWorldTransform().getOrigin();
				btVector3 to = from + btVector3(0, 0, -wheelInfo->m_rayCastHeight);

				btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);

				world->rayTest(from, to, rayCallback);

				if (rayCallback.hasHit())
				{
					const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);

					if (body && body->hasContactResponse())
					{
						wheelInfo->m_hitPointInWorld = rayCallback.m_hitPointWorld;
						wheelInfo->m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
						wheelInfo->m_hitNormalInWorld.normalize();
						wheelInfo->m_hitGround = true;
					}
				}
			}
		}
	}
}

int CPhysicVehicleBehaviour::CountSurfaceContactPoints()
{
	int nCount = 0;

	for (size_t i = 0; i < m_wheelConstraints.size(); i++)
	{
		auto constraint = GetWheelConstraint(i);

		if (constraint)
		{
			if (constraint && constraint->getUserConstraintType() == ConstraintType_Wheel)
			{
				auto wheelInfo = (CPhysicVehicleWheelInfo *)constraint->getUserConstraintPtr();

				if (wheelInfo->m_hitGround)
				{
					nCount++;
				}
			}
		}
	}

	return nCount;
}

ATTRIBUTE_ALIGNED16(class)
CPhysicFourWheelsVehicleBehaviour : public CPhysicVehicleBehaviour
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	CPhysicFourWheelsVehicleBehaviour(CDynamicObject *vehiclePhysObject) : CPhysicVehicleBehaviour(vehiclePhysObject)
	{

	}

	bool SetVehicleSteering(int wheelIndex, float angularTarget, float targetVelcoity, float maxMotorForce) override
	{
		auto constraint = GetWheelConstraint(wheelIndex);

		if (constraint && constraint->getUserConstraintType() == ConstraintType_Wheel)
		{
			auto wheelInfo = (CPhysicVehicleWheelInfo *)constraint->getUserConstraintPtr();

			constraint->setServoTarget(wheelInfo->m_steerIndex, angularTarget);
			constraint->setTargetVelocity(wheelInfo->m_steerIndex, targetVelcoity);
			constraint->setMaxMotorForce(wheelInfo->m_steerIndex, maxMotorForce);

			return true;
		}

		return false;
	}

	bool SetVehicleEngine(int wheelIndex, bool enableMotor, float targetVelcoity, float maxMotorForce) override
	{
		auto constraint = GetWheelConstraint(wheelIndex);

		if (constraint && constraint->getUserConstraintType() == ConstraintType_Wheel)
		{
			auto wheelInfo = (CPhysicVehicleWheelInfo *)constraint->getUserConstraintPtr();

			constraint->enableMotor(wheelInfo->m_engineIndex, enableMotor);
			constraint->setTargetVelocity(wheelInfo->m_engineIndex, targetVelcoity);
			constraint->setMaxMotorForce(wheelInfo->m_engineIndex, maxMotorForce);

			return true;
		}

		return false;
	}

	void StartFrame(btDiscreteDynamicsWorld* world) override
	{
		//auto rigidbody = GetDynamicObject()->GetRigidBody();

		DoRaycasts(world);
	}

};

inline float fsel(float fComparand, float fValGE, float fLT)
{
	return fComparand >= 0 ? fValGE : fLT;
}

inline float RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return fsel(val - B, D, C);

	float cVal = (val - A) / (B - A);
	
	if (cVal < 0)
		cVal = 0;

	if (cVal > 1)
		cVal = 1;

	return C + (D - C) * cVal;
}

//from Source SDK

#define AIRBOAT_STEERING_RATE_MIN			0.2f
#define AIRBOAT_STEERING_RATE_MAX			1.0f
#define AIRBOAT_STEERING_INTERVAL			0.5f

#define AIRBOAT_ROT_DRAG					0.00004f
#define AIRBOAT_ROT_DAMPING					0.001f

ATTRIBUTE_ALIGNED16(class)
CPhysicAirboatBehaviour : public CPhysicVehicleBehaviour
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();

	CPhysicAirboatBehaviour(CDynamicObject *vehiclePhysObject) : CPhysicVehicleBehaviour(vehiclePhysObject)
	{
		m_bEngineEnabled = false;
		m_flEngineMaxVelocity = 0;
		m_flEngineMaxForce = 0;

		//Steering
		m_flPrevSteeringAngle = 0;
		m_flSteeringMaxVelocity = 0;
		m_flSteeringMaxForce = 0;
		m_flSteeringAngle = 0;
		m_flSteeringTime = 0;

		m_bAirborne = false;
		m_bWeakJump = false;
		m_flPitchErrorPrev = 0;
		m_flRollErrorPrev = 0;
		m_flAirTime = 0;
	}

	bool SetVehicleSteering(int wheelIndex, float angularTarget, float targetVelcoity, float maxMotorForce) override
	{
		m_flSteeringAngle = angularTarget;
		m_flSteeringMaxVelocity = targetVelcoity;
		m_flSteeringMaxForce = maxMotorForce;

		return true;
	}

	bool SetVehicleEngine(int wheelIndex, bool enableMotor, float targetVelcoity, float maxMotorForce) override
	{
		m_bEngineEnabled = enableMotor;
		m_flEngineMaxVelocity = targetVelcoity;
		m_flEngineMaxForce = maxMotorForce;

		return true;
	}

	void UpdateAirborneState(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		int nCount = CountSurfaceContactPoints();
		if (!nCount)
		{
			if (!m_bAirborne)
			{
				m_bAirborne = true;
				m_flAirTime = 0;

				float flSpeed = rigidbody->getLinearVelocity().length();
				if (flSpeed < 330)
				{
					m_bWeakJump = true;
				}
			}
			else
			{
				m_flAirTime += (float)(*host_frametime);
			}
		}
		else
		{
			m_bAirborne = false;
			m_bWeakJump = false;
		}
	}

	void DoSimulationTurbine(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		float flThrustForce = m_flEngineMaxForce;

		if (m_bWeakJump)
		{
			flThrustForce *= 0.5;
		}

		auto vecLinearVelocity = rigidbody->getLinearVelocity();

		if (m_bEngineEnabled && flThrustForce != 0 && vecLinearVelocity.length() < m_flEngineMaxVelocity)
		{
			btVector3 vecForceInLocaSpace(1, 0, 0);

			auto vecDirectionInWorldSpace = (rigidbody->getWorldTransform().getBasis() * vecForceInLocaSpace).normalized();

			if ((vecDirectionInWorldSpace.getZ() < -0.5) && (flThrustForce > 0))
			{
				// Driving up a slope. Reduce upward thrust to prevent ludicrous climbing of steep surfaces.
				float flFactor = 1 + vecDirectionInWorldSpace.getZ();
				flThrustForce *= flFactor;
			}
			else if ((vecDirectionInWorldSpace.getZ() > 0.5) && (flThrustForce < 0))
			{
				// Reversing up a slope. Reduce upward thrust to prevent ludicrous climbing of steep surfaces.
				float flFactor = 1 - vecDirectionInWorldSpace.getZ();
				flThrustForce *= flFactor;
			}

			auto vecForceInWorldSpace = vecDirectionInWorldSpace * flThrustForce;

			rigidbody->applyCentralForce(vecForceInWorldSpace);
		}
	}

	void DoSimulationSteering(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		auto vecAngularVelocity = rigidbody->getAngularVelocity();

		float flForceSteering = 0.0f;
		float flSteerScale = 0.0f;

		if (fabs(m_flSteeringAngle) > 0.01f)
		{
			// Get the sign of the steering force.
			float flSteeringSign = m_flSteeringAngle < 0.0f ? -1.0f : 1.0f;

			// If we changed steering sign or went from not steering to steering, reset the steer time
			// to blend the new steering force in over time.
			float flPrevSteeringSign = m_flPrevSteeringAngle < 0.0f ? -1.0f : 1.0f;

			if ((fabs(m_flPrevSteeringAngle) < 0.01f) || (flSteeringSign != flPrevSteeringSign))
			{
				m_flSteeringTime = 0;
			}

			if (1)
			{
				// Ramp the steering force up over two seconds.
				flSteerScale = RemapValClamped(m_flSteeringTime, 0, AIRBOAT_STEERING_INTERVAL, AIRBOAT_STEERING_RATE_MIN, AIRBOAT_STEERING_RATE_MAX);
			}
			else	// consoles
			{
				// Analog steering
				flSteerScale = RemapValClamped(fabs(m_flSteeringTime), 0, AIRBOAT_STEERING_INTERVAL, AIRBOAT_STEERING_RATE_MIN, AIRBOAT_STEERING_RATE_MAX);
			}
			flForceSteering = flSteerScale * m_flSteeringMaxForce;
			flForceSteering *= -flSteeringSign;

			m_flSteeringTime += 0.01f;
		}

		m_flPrevSteeringAngle = m_flSteeringAngle;

		// Get the sign of the drag forces.
		float flRotSpeedSign = vecAngularVelocity.getY() < 0.0f ? -1.0f : 1.0f;

		// Apply drag proportional to the square of the angular velocity.
		float flRotationalDrag = AIRBOAT_ROT_DRAG * vecAngularVelocity.getZ() * vecAngularVelocity.getZ() * m_flSteeringMaxForce;
		flRotationalDrag *= flRotSpeedSign;

		// Apply dampening proportional to angular velocity.
		float flRotationalDamping = AIRBOAT_ROT_DAMPING * fabs(vecAngularVelocity.getZ()) * m_flSteeringMaxForce;
		flRotationalDamping *= flRotSpeedSign;

		// Calculate the net rotational force.
		float flForceRotational = flForceSteering + flRotationalDrag + flRotationalDamping;

		// Apply it.
		btVector3 vecRotForce(0, 0, 1);

		vecRotForce = vecRotForce * flForceRotational;

		rigidbody->applyTorque(vecRotForce);

	}

	void DoSimulationKeepUprightPitch(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		// Disable pitch control during weak jumps. This reduces the unreal 'floaty' sensation.
		if (m_bWeakJump)
			return;

		// Calculate the goal vector in world space.
		btVector3 vecUpAxisWS(0, 0, 1);

		btVector3 vecForwardAxisCS(1, 0, 0);

		btVector3 vecCurAxisWS = rigidbody->getWorldTransform().getBasis() * vecForwardAxisCS;

		btVector3 vecGoalAxisWS = vecCurAxisWS;

		vecGoalAxisWS.setZ(0);

		vecGoalAxisWS = vecGoalAxisWS.normalized();

		// Get an axis to rotate around.
		btVector3 vecRotAxisWS = vecUpAxisWS.cross(vecCurAxisWS);

		// Get the amount that we need to rotate.
		// atan2() is well defined, so do a Dot & Cross instead of asin(Cross)
		float cosine = vecCurAxisWS.dot(vecUpAxisWS);
		float sine = vecCurAxisWS.dot(vecGoalAxisWS);
		float error = atan2(cosine, sine);

		float drivative = (error - m_flPitchErrorPrev) / (float)(*host_frametime);

		//PID control
		float kp = 0.1f;
		float kd = 0.04f;

		// Generate an angular impulse describing the rotation.
		btVector3 vecAngularImpulse = vecRotAxisWS * (rigidbody->getMass() * 6000 * (kp * error + kd * drivative));
		
		// Save the last error value for calculating the derivative.
		m_flPitchErrorPrev = error;

		// Clamp the impulse at a maximum length.
		float flMaxRotateForce = 1.5f * SIMD_RADS_PER_DEG * rigidbody->getMass() * 6000;
		if (vecAngularImpulse.length() > flMaxRotateForce)
		{
			vecAngularImpulse = vecAngularImpulse.normalized() * flMaxRotateForce;
		}

		rigidbody->applyTorqueImpulse(vecAngularImpulse);
	}

	void DoSimulationKeepUprightRoll(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		// Calculate the goal vector in world space.
		btVector3 vecUpAxisWS(0, 0, 1);

		btVector3 vecRightAxisCS(0, 1, 0);

		btVector3 vecCurAxisWS = rigidbody->getWorldTransform().getBasis() * vecRightAxisCS;

		btVector3 vecGoalAxisWS = vecCurAxisWS;

		vecGoalAxisWS.setZ(0);

		vecGoalAxisWS = vecGoalAxisWS.normalized();

		// Get an axis to rotate around.
		btVector3 vecRotAxisWS = vecUpAxisWS.cross(vecCurAxisWS);

		// Get the amount that we need to rotate.
		// atan2() is well defined, so do a Dot & Cross instead of asin(Cross)
		float cosine = vecCurAxisWS.dot(vecUpAxisWS);
		float sine = vecCurAxisWS.dot(vecGoalAxisWS);
		float error = atan2(cosine, sine);

		// Don't do any correction if we're within 10 degrees of the goal orientation.
		if (fabs(error) < SIMD_RADS_PER_DEG * 10)
		{
			m_flRollErrorPrev = error;
			return;
		}

		float drivative = (error - m_flRollErrorPrev) / (float)(*host_frametime);

		//PID control
		float kp = 0.2f;
		float kd = 0.3f;

		// Generate an angular impulse describing the rotation.
		btVector3 vecAngularImpulse = vecRotAxisWS * (rigidbody->getMass() * 6000 * (kp * error + kd * drivative));

		// Save the last error value for calculating the derivative.
		m_flRollErrorPrev = error;

		// Clamp the impulse at a maximum length.
		float flMaxRotateForce = 2.0f * SIMD_RADS_PER_DEG * rigidbody->getMass() * 6000;
		if (vecAngularImpulse.length() > flMaxRotateForce)
		{
			vecAngularImpulse = vecAngularImpulse.normalized() * flMaxRotateForce;
		}

		rigidbody->applyTorqueImpulse(vecAngularImpulse);
	}

	void StartFrame(btDiscreteDynamicsWorld* world) override
	{
		auto rigidbody = GetDynamicObject()->GetRigidBody();

		DoRaycasts(world);

		DoSimulationTurbine(world, rigidbody);

		DoSimulationSteering(world, rigidbody);

		DoSimulationKeepUprightPitch(world, rigidbody);

		DoSimulationKeepUprightRoll(world, rigidbody);
	}

private:
	//Engine
	bool m_bEngineEnabled;
	float m_flEngineMaxVelocity;
	float m_flEngineMaxForce;

	//Steering
	float m_flPrevSteeringAngle;
	float m_flSteeringMaxVelocity;
	float m_flSteeringMaxForce;
	float m_flSteeringAngle;
	float m_flSteeringTime;

	//WTF?
	bool m_bAirborne;
	bool m_bWeakJump;
	float m_flPitchErrorPrev;
	float m_flRollErrorPrev;
	float m_flAirTime;
};

bool CPhysicsManager::SetVehicleEngine(edict_t* ent, int wheelIndex, bool enableMotor, float targetVelcoity, float maxMotorForce)
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

	if (!dynObject->GetVehicleBehaviour())
		return false;

	return dynObject->GetVehicleBehaviour()->SetVehicleEngine(wheelIndex, enableMotor, targetVelcoity, maxMotorForce);
}

bool CPhysicsManager::SetVehicleSteering(edict_t* ent, int wheelIndex, float angularTarget, float targetVelcoity, float maxMotorForce)
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

	if (!dynObject->GetVehicleBehaviour())
		return false;

	return dynObject->GetVehicleBehaviour()->SetVehicleSteering(wheelIndex, angularTarget, targetVelcoity, maxMotorForce);
}

bool CPhysicsManager::GetVehicleWheelRuntimeInfo(edict_t* ent, int wheelIndex, PhysicWheelRuntimeInfo *RuntimeInfo)
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

	if (!dynObject->GetVehicleBehaviour())
		return false;

	return dynObject->GetVehicleBehaviour()->GetVehicleWheelRuntimeInfo(wheelIndex, RuntimeInfo);
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

bool CPhysicsManager::SetEntitySemiClip(edict_t* ent, int player_mask)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetPlayerSemiClipMask(player_mask);
	
	return true;
}

bool CPhysicsManager::SetEntityPMSemiClip(edict_t* ent, int player_mask)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetPlayerPMSemiClipMask(player_mask);

	return true;
}

bool CPhysicsManager::SetEntitySemiClipToEntityIndex(edict_t* ent, int entindex)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetSemiClipToEntity(entindex);

	return true;
}

bool CPhysicsManager::SetEntityPMSemiClipToEntityIndex(edict_t* ent, int entindex)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetPMSemiClipToEntity(entindex);

	return true;
}

bool CPhysicsManager::UnsetEntitySemiClipToEntityIndex(edict_t* ent, int entindex)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetSemiClipToEntity(entindex);

	return true;
}

bool CPhysicsManager::UnsetEntityPMSemiClipToEntityIndex(edict_t* ent, int entindex)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetPMSemiClipToEntity(entindex);

	return true;
}

bool CPhysicsManager::SetEntitySemiClipToEntity(edict_t* ent, edict_t* targetEntity)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetSemiClipToEntity(targetEntity);

	return true;
}

bool CPhysicsManager::SetEntityPMSemiClipToEntity(edict_t* ent, edict_t* targetEntity)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->SetPMSemiClipToEntity(targetEntity);

	return true;
}

bool CPhysicsManager::UnsetEntitySemiClipToEntity(edict_t* ent, edict_t* targetEntity)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetSemiClipToEntity(targetEntity);

	return true;
}

bool CPhysicsManager::UnsetEntityPMSemiClipToEntity(edict_t* ent, edict_t* targetEntity)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetPMSemiClipToEntity(targetEntity);

	return true;
}

bool CPhysicsManager::UnsetEntitySemiClipToAll(edict_t* ent)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetSemiClipToAll();

	return true;
}

bool CPhysicsManager::UnsetEntityPMSemiClipToAll(edict_t* ent)
{
	if (ent->free)
		return false;

	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	obj->UnsetPMSemiClipToAll();

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

bool CPhysicsManager::SetPhysicObjectNoCollision(edict_t* ent, bool no_collision)
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

		physobj->SetPhysicNoCollision(no_collision);
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

	auto mod = EngineGetPrecachedModelByIndex(ent->v.modelindex);

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

bool CPhysicsManager::CreatePhysicVehicle(edict_t* ent, PhysicVehicleParams *vehicleParams, PhysicWheelParams **wheelParamArray, size_t numWheelParam)
{
	auto gameObject = GetGameObject(ent);

	if (!gameObject)
	{
		gameObject = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(gameObject);
	}

	if (!gameObject->GetNumPhysicObject())
	{
		return false;
	}
	
	auto physObject = gameObject->GetPhysicObjectByIndex(0);

	if(!physObject->IsDynamicObject())
	{
		return false;
	}

	auto dynObject = (CDynamicObject *)physObject;

	if (dynObject->GetVehicleBehaviour())
	{
		//Already created?
		return false;
	}

	CPhysicVehicleBehaviour *VehicleBehaviour = NULL;

	if (vehicleParams->type == PhysicVehicleType_FourWheels)
	{
		VehicleBehaviour = new CPhysicFourWheelsVehicleBehaviour(dynObject);
	}
	else if (vehicleParams->type == PhysicVehicleType_Airboat)
	{
		VehicleBehaviour = new CPhysicAirboatBehaviour(dynObject);
	}

	if (!VehicleBehaviour)
		return false;

	for (size_t i = 0; i < numWheelParam; ++i)
	{
		auto wheelParam = wheelParamArray[i];

		auto wheelEnt = wheelParam->ent;

		if (!wheelEnt || wheelEnt->free)
			continue;

		auto wheelObject = GetGameObject(wheelEnt);

		if (wheelObject->GetNumPhysicObject() > 0)
		{
			auto wheelPhysObject = wheelObject->GetPhysicObjectByIndex(0);

			if (wheelPhysObject->IsDynamicObject())
			{
				auto wheelDynObject = (CDynamicObject *)wheelPhysObject;

				btVector3 anchor(wheelParam->connectionPoint.x, wheelParam->connectionPoint.y, wheelParam->connectionPoint.z);
				btVector3 parentAxis(wheelParam->wheelDirection.x, wheelParam->wheelDirection.y, wheelParam->wheelDirection.z);
				btVector3 childAxis(wheelParam->wheelAxle.x, wheelParam->wheelAxle.y, wheelParam->wheelAxle.z);
				btScalar suspensionStiffness(wheelParam->suspensionStiffness);
				btScalar suspensionDamping(wheelParam->suspensionDamping);
				btScalar suspensionLowerLimit(wheelParam->suspensionLowerLimit);
				btScalar suspensionUpperLimit(wheelParam->suspensionUpperLimit);

				auto pConstraint = new btHinge2Constraint(*dynObject->GetRigidBody(), *wheelDynObject->GetRigidBody(), anchor, parentAxis, childAxis);

				auto wheelInfo = new CPhysicVehicleWheelInfo(wheelParam);

				pConstraint->setUserConstraintType(ConstraintType_Wheel);
				pConstraint->setUserConstraintPtr(wheelInfo);

				pConstraint->setParam(BT_CONSTRAINT_CFM, 0.15f, wheelInfo->m_springIndex);
				pConstraint->setParam(BT_CONSTRAINT_ERP, 0.35f, wheelInfo->m_springIndex);

				pConstraint->enableSpring(wheelInfo->m_springIndex, true);
				pConstraint->setStiffness(wheelInfo->m_springIndex, suspensionStiffness);
				pConstraint->setDamping(wheelInfo->m_springIndex, suspensionDamping);
				pConstraint->setLimit(wheelInfo->m_springIndex, -suspensionLowerLimit, suspensionUpperLimit);

				if (wheelParam->flags & PhysicWheel_Engine)
				{
					pConstraint->enableMotor(wheelInfo->m_engineIndex, true);
					pConstraint->setMaxMotorForce(wheelInfo->m_engineIndex, vehicleParams->idleEngineForce);
					pConstraint->setTargetVelocity(wheelInfo->m_engineIndex, 0);
				}

				if (wheelParam->flags & PhysicWheel_Steering)
				{
					pConstraint->enableMotor(wheelInfo->m_steerIndex, true);
					pConstraint->setMaxMotorForce(wheelInfo->m_steerIndex, vehicleParams->idleSteeringForce);
					pConstraint->setTargetVelocity(wheelInfo->m_steerIndex, vehicleParams->idleSteeringSpeed);
					pConstraint->setServo(wheelInfo->m_steerIndex, true);
					pConstraint->setServoTarget(wheelInfo->m_steerIndex, 0);
				}
				else if (wheelParam->flags & PhysicWheel_NoSteering)
				{
					pConstraint->setLimit(wheelInfo->m_steerIndex, 0, 0);
				}

				if (wheelParam->flags & PhysicWheel_Pontoon)
				{
					pConstraint->setLimit(3, 0, 0);
					pConstraint->setLimit(4, 0, 0);
					pConstraint->setLimit(5, 0, 0);
				}

				pConstraint->setEquilibriumPoint();

				gameObject->AddConstraint(pConstraint, m_dynamicsWorld, true);

				VehicleBehaviour->SetWheelConstraint(wheelParam->index, pConstraint);
			}
		}
	}

	dynObject->SetVehicleBehaviour(VehicleBehaviour);

	return true;
}

void CPhysicsManager::SetPhysicPlayerConfig(PhysicPlayerConfigs *configs)
{
	m_playerMass = configs->mass;
	m_playerMaxPendingVelocity = configs->maxPendingVelocity;
}

void CPhysicsManager::EnablePhysicWorld(bool bEnabled)
{
	m_bEnabled = bEnabled;
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

	auto mod = EngineGetPrecachedModelByIndex(ent->v.modelindex);

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

		auto shape = new btBoxShape(boxSize);

		ghost->SetGhostObject(new btPairCachingGhostObject());
		ghost->GetGhostObject()->setCollisionShape(shape);
		ghost->GetGhostObject()->setCollisionFlags(ghost->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		obj->AddPhysicObject(ghost, m_dynamicsWorld);
	}

	if (1)
	{
		auto ghost = new CSolidOptimizerGhostPhysicObject(obj, boneindex, 1);

		auto shape = new btBoxShape(boxSize2);

		ghost->SetGhostObject(new btPairCachingGhostObject());
		ghost->GetGhostObject()->setCollisionShape(shape);
		ghost->GetGhostObject()->setCollisionFlags(ghost->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		obj->AddPhysicObject(ghost, m_dynamicsWorld);
	}

	obj->AddSolidOptimizer(boneindex, 0);

	return true;
}

model_t *CPhysicsManager::GetBrushModelFromEntity(edict_t *ent)
{
	int modelindex = ent->v.modelindex;

	if (modelindex == -1)
	{
		return NULL;
	}

	auto mod = EngineGetPrecachedModelByIndex(modelindex);

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

	return mod;
}

std::shared_ptr<CPhysicIndexArray> CPhysicsManager::GetIndexArrayFromBrushEntity(edict_t *ent)
{
	auto mod = GetBrushModelFromEntity(ent);

	if(!mod)
	{
		return nullptr;
	}

	auto resourceName = UTIL_GetAbsoluteModelName(mod);
	
	return LoadIndexArrayFromResource(resourceName);
}

btBvhTriangleMeshShape *CPhysicsManager::CreateTriMeshShapeFromBrushEntity(edict_t *ent)
{
	auto pIndexArray = GetIndexArrayFromBrushEntity(ent);

	if (!pIndexArray)
		return nullptr;

	if (!pIndexArray->vIndexBuffer.size())
		return nullptr;

	auto pTriangleIndexVertexArray = new btTriangleIndexVertexArray(
		pIndexArray->vIndexBuffer.size() / 3, pIndexArray->vIndexBuffer.data(), 3 * sizeof(int),
		pIndexArray->pVertexArray->vVertexBuffer.size(), (float*)pIndexArray->pVertexArray->vVertexBuffer.data(), sizeof(CPhysicBrushVertex));

	auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

	shapeSharedUserData->m_pIndexArray = pIndexArray;
	shapeSharedUserData->m_pTriangleIndexVertexArray  = pTriangleIndexVertexArray;

	auto pCollisionShape = new btBvhTriangleMeshShape(pTriangleIndexVertexArray, true, true);

	pCollisionShape->setUserPointer(shapeSharedUserData);

	return pCollisionShape;
}

/*

Ghost object that fire pfnTouch event when colliding with physic objects

*/

ATTRIBUTE_ALIGNED16(class)
CPhysicTriggerGhostPhysicObject : public CGhostPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicTriggerGhostPhysicObject(CGameObject *obj) : CGhostPhysicObject(obj)
	{

	}
	~CPhysicTriggerGhostPhysicObject()
	{

	}

	bool IsPhysicTriggerGhost() const override
	{
		return true;
	}

	bool IsPhysicWaterGhost() const override
	{
		return false;
	}

	void AddToPhysicWorld(btDiscreteDynamicsWorld* world) override
	{
		CPhysicObject::AddToPhysicWorld(world);

		if (GetGhostObject())
		{
			world->addCollisionObject(GetGhostObject(), btBroadphaseProxy::SensorTrigger, FallGuysCollisionFilterGroups::DynamicObjectFilter | FallGuysCollisionFilterGroups::ClippingHullFilter);
		}
	}

	void StartFrame(btDiscreteDynamicsWorld* world) override
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

	void OnTouchRigidBody(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody) override
	{
		auto physobj = (CCollisionPhysicObject *)rigidbody->getUserPointer();

		auto ent = GetGameObject()->GetEdict();

		auto touchent = physobj->GetGameObject()->GetEdict();

		gpGamedllFuncs->dllapi_table->pfnTouch(ent, touchent);
	}
};

bool CPhysicsManager::CreatePhysicTrigger(edict_t* ent)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	btCollisionShape *shape = CreateTriMeshShapeFromBrushEntity(ent);

	if (!shape)
	{
		shape = new btBoxShape(btVector3((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f));
	}

	auto ghostobj = new CPhysicTriggerGhostPhysicObject(obj);
	
	ghostobj->SetGhostObject(new btPairCachingGhostObject());
	ghostobj->GetGhostObject()->setCollisionShape(shape);
	ghostobj->GetGhostObject()->setCollisionFlags(ghostobj->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	obj->AddPhysicObject(ghostobj, m_dynamicsWorld);

	return true;
}

/*

Water that only interactive with physic objects

*/

ATTRIBUTE_ALIGNED16(class)
CPhysicWaterGhostPhysicObject : public CPhysicTriggerGhostPhysicObject
{
public:
	BT_DECLARE_ALIGNED_ALLOCATOR();
	CPhysicWaterGhostPhysicObject(CGameObject *obj, const btVector3 &inSurfaceNormal, const btScalar inSurfacePlane, btScalar inWaterDensity, btScalar inWaterLinearDrag, btScalar inWaterAngularDrag) : CPhysicTriggerGhostPhysicObject(obj)
	{
		m_WaterSurfaceNormal = inSurfaceNormal;
		m_WaterSurfacePlane = inSurfacePlane;
		m_WaterDensity = inWaterDensity;
		m_WaterLinearDrag = inWaterLinearDrag;
		m_WaterAngularDrag = inWaterAngularDrag;
		m_WaterFluidVelocity = btVector3(0, 0, 0);
	}
	~CPhysicWaterGhostPhysicObject()
	{

	}

	bool IsPhysicWaterGhost() const override
	{
		return true;
	}

	btVector3 GetSurfaceNormal() const
	{
		return m_WaterSurfaceNormal;
	}

	float GetSurfacePlane() const
	{
		return m_WaterSurfacePlane;
	}

	float GetWaterDensity() const
	{
		return m_WaterDensity;
	}

	float GetWaterLinearDrag() const
	{
		return m_WaterLinearDrag;
	}

	float GetWaterAngularDrag() const
	{
		return m_WaterAngularDrag;
	}

	btVector3 GetWaterFluidVelocity() const
	{
		return m_WaterFluidVelocity;
	}

	btVector3 GetWaterLinearVelocity() const
	{
		auto ent = GetGameObject()->GetEdict();

		btVector3 vecLinearVelocity(ent->v.velocity.x, ent->v.velocity.y, ent->v.velocity.z);

		return vecLinearVelocity;
	}

	btVector3 GetWaterAngularVelocity() const
	{
		auto ent = GetGameObject()->GetEdict();

		btVector3 vecAngularVelocity(ent->v.avelocity.x * SIMD_RADS_PER_DEG, ent->v.avelocity.y* SIMD_RADS_PER_DEG, ent->v.avelocity.z * SIMD_RADS_PER_DEG);

		return vecAngularVelocity;
	}

	void OnTouchRigidBody(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		auto physobj = (CCollisionPhysicObject *)rigidbody->getUserPointer();

		if (physobj->IsDynamicObject() && !physobj->IsKinematic())
		{
			ApplyBuoyancyImpulse(world, rigidbody);
		}
	}

	void GetSubmergedVolumeForBoxShape(btDiscreteDynamicsWorld* world, btBoxShape *shape, const btTransform &worldTransform, float &outSubmergedVolume, btVector3 &outCenterOfBuoyancy)
	{
		auto shapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

		// Points of the bounding box
		btVector3 points[] =
		{
			btVector3(-1, -1, -1),
			btVector3(1, -1, -1),
			btVector3(-1,  1, -1),
			btVector3(1,  1, -1),
			btVector3(-1, -1,  1),
			btVector3(1, -1,  1),
			btVector3(-1,  1,  1),
			btVector3(1,  1,  1),
		};

		// Faces of the bounding box
		using Face = int[5];
#define MAKE_FACE(a, b, c, d) { a, b, c, d, ((1 << a) | (1 << b) | (1 << c) | (1 << d)) } // Last int is a bit mask that indicates which indices are used
		Face faces[] =
		{
			MAKE_FACE(0, 2, 3, 1),
			MAKE_FACE(4, 6, 2, 0),
			MAKE_FACE(4, 5, 7, 6),
			MAKE_FACE(1, 3, 7, 5),
			MAKE_FACE(2, 6, 7, 3),
			MAKE_FACE(0, 1, 5, 4),
		};

		btVector3 halfExtents = shape->getHalfExtentsWithoutMargin();

		btTransform transScaled(btMatrix3x3::getIdentity().scaled(halfExtents));

		PolyhedronSubmergedVolumeCalculator::Point *buffer = (PolyhedronSubmergedVolumeCalculator::Point *)alloca(8 * sizeof(PolyhedronSubmergedVolumeCalculator::Point));
		PolyhedronSubmergedVolumeCalculator submerged_vol_calc(worldTransform * transScaled, points, sizeof(btVector3), 8, GetSurfaceNormal(), GetSurfacePlane(), buffer);

		if (submerged_vol_calc.AreAllAbove())
		{
			// We're above the water
			outSubmergedVolume = 0.0f;
			outCenterOfBuoyancy = btVector3(0, 0, 0);
		}
		else if (submerged_vol_calc.AreAllBelow())
		{
			// We're fully submerged
			outSubmergedVolume = shapeSharedUserData->m_volume;
			outCenterOfBuoyancy = worldTransform.getOrigin();
		}
		else
		{
			// Calculate submerged volume
			int reference_point_bit = 1 << submerged_vol_calc.GetReferencePointIdx();
			for (const Face &f : faces)
			{
				// Test if this face includes the reference point
				if ((f[4] & reference_point_bit) == 0)
				{
					// Triangulate the face (a quad)
					submerged_vol_calc.AddFace(f[0], f[1], f[2]);
					submerged_vol_calc.AddFace(f[0], f[2], f[3]);
				}
			}

			submerged_vol_calc.GetResult(outSubmergedVolume, outCenterOfBuoyancy);

			outSubmergedVolume /= 8;

			if (outSubmergedVolume > shapeSharedUserData->m_volume)
				outSubmergedVolume = shapeSharedUserData->m_volume;
		}
	}

	void GetSubmergedVolumeForSphereShape(btDiscreteDynamicsWorld* world, btSphereShape *shape, const btTransform &worldTransform, float &outSubmergedVolume, btVector3 &outCenterOfBuoyancy)
	{
		auto shapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

		auto scaled_radius = shape->getRadius();

		auto distance_to_surface = GetSignedDistanceToSurface(worldTransform.getOrigin(), GetSurfaceNormal(), GetSurfacePlane());

		if (distance_to_surface >= scaled_radius)
		{
			// Above surface
			outCenterOfBuoyancy = btVector3(0, 0, 0);
			outSubmergedVolume = 0;
		}
		else if (distance_to_surface <= -scaled_radius)
		{
			// Under surface
			outCenterOfBuoyancy = worldTransform.getOrigin();
			outSubmergedVolume = shapeSharedUserData->m_volume;
		}
		else
		{
			// Intersecting surface

			// Calculate submerged volume, see: https://en.wikipedia.org/wiki/Spherical_cap
			float h = scaled_radius - distance_to_surface;
			outSubmergedVolume = ((float)(M_PI) / 3.0f) * (h * h) * (3.0f * scaled_radius - h);

			// Calculate center of buoyancy, see: http://mathworld.wolfram.com/SphericalCap.html (eq 10)
			float z = (3.0f / 4.0f) * ((2.0f * scaled_radius - h) * (2.0f * scaled_radius - h)) / (3.0f * scaled_radius - h);
			outCenterOfBuoyancy = worldTransform.getOrigin() - z * GetSurfaceNormal(); // Negative normal since we want the portion under the water
		}
	}

	void GetSubmergedVolumeForCylinderShape(btDiscreteDynamicsWorld* world, btCylinderShape *shape, const btTransform &worldTransform, float &outSubmergedVolume, btVector3 &outCenterOfBuoyancy)
	{
		auto shapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

		// Points of the bounding box
		btVector3 points[] =
		{
			btVector3(-1, -1, -1),
			btVector3(1, -1, -1),
			btVector3(-1,  1, -1),
			btVector3(1,  1, -1),
			btVector3(-1, -1,  1),
			btVector3(1, -1,  1),
			btVector3(-1,  1,  1),
			btVector3(1,  1,  1),
		};

		// Faces of the bounding box
		using Face = int[5];
#define MAKE_FACE(a, b, c, d) { a, b, c, d, ((1 << a) | (1 << b) | (1 << c) | (1 << d)) } // Last int is a bit mask that indicates which indices are used
		Face faces[] =
		{
			MAKE_FACE(0, 2, 3, 1),
			MAKE_FACE(4, 6, 2, 0),
			MAKE_FACE(4, 5, 7, 6),
			MAKE_FACE(1, 3, 7, 5),
			MAKE_FACE(2, 6, 7, 3),
			MAKE_FACE(0, 1, 5, 4),
		};

		btVector3 halfExtents = shape->getHalfExtentsWithoutMargin();

		btTransform transScaled(btMatrix3x3::getIdentity().scaled(halfExtents));

		PolyhedronSubmergedVolumeCalculator::Point *buffer = (PolyhedronSubmergedVolumeCalculator::Point *)alloca(8 * sizeof(PolyhedronSubmergedVolumeCalculator::Point));
		PolyhedronSubmergedVolumeCalculator submerged_vol_calc(worldTransform * transScaled, points, sizeof(btVector3), 8, GetSurfaceNormal(), GetSurfacePlane(), buffer);

		if (submerged_vol_calc.AreAllAbove())
		{
			// We're above the water
			outSubmergedVolume = 0.0f;
			outCenterOfBuoyancy = btVector3(0, 0, 0);
		}
		else if (submerged_vol_calc.AreAllBelow())
		{
			// We're fully submerged
			outSubmergedVolume = shapeSharedUserData->m_volume;
			outCenterOfBuoyancy = worldTransform.getOrigin();
		}
		else
		{
			// Calculate submerged volume
			int reference_point_bit = 1 << submerged_vol_calc.GetReferencePointIdx();
			for (const Face &f : faces)
			{
				// Test if this face includes the reference point
				if ((f[4] & reference_point_bit) == 0)
				{
					// Triangulate the face (a quad)
					submerged_vol_calc.AddFace(f[0], f[1], f[2]);
					submerged_vol_calc.AddFace(f[0], f[2], f[3]);
				}
			}

			submerged_vol_calc.GetResult(outSubmergedVolume, outCenterOfBuoyancy);

			// Rough estimate
			outSubmergedVolume *= ((float)(M_PI) / 4.0f);

			outSubmergedVolume /= 8;

			if (outSubmergedVolume > shapeSharedUserData->m_volume)
				outSubmergedVolume = shapeSharedUserData->m_volume;
		}
	}

	void GetSubmergedVolumeForCapsuleShape(btDiscreteDynamicsWorld* world, btCapsuleShape *shape, const btTransform &worldTransform, float &outSubmergedVolume, btVector3 &outCenterOfBuoyancy)
	{
		auto shapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

		// Points of the bounding box
		btVector3 points[] =
		{
			btVector3(-1, -1, -1),
			btVector3(1, -1, -1),
			btVector3(-1,  1, -1),
			btVector3(1,  1, -1),
			btVector3(-1, -1,  1),
			btVector3(1, -1,  1),
			btVector3(-1,  1,  1),
			btVector3(1,  1,  1),
		};

		// Faces of the bounding box
		using Face = int[5];
#define MAKE_FACE(a, b, c, d) { a, b, c, d, ((1 << a) | (1 << b) | (1 << c) | (1 << d)) } // Last int is a bit mask that indicates which indices are used
		Face faces[] =
		{
			MAKE_FACE(0, 2, 3, 1),
			MAKE_FACE(4, 6, 2, 0),
			MAKE_FACE(4, 5, 7, 6),
			MAKE_FACE(1, 3, 7, 5),
			MAKE_FACE(2, 6, 7, 3),
			MAKE_FACE(0, 1, 5, 4),
		};

		btVector3 halfExtents = btVector3(shape->getRadius(), shape->getRadius(), shape->getHalfHeight());

		btTransform transScaled(btMatrix3x3::getIdentity().scaled(halfExtents));

		PolyhedronSubmergedVolumeCalculator::Point *buffer = (PolyhedronSubmergedVolumeCalculator::Point *)alloca(8 * sizeof(PolyhedronSubmergedVolumeCalculator::Point));
		PolyhedronSubmergedVolumeCalculator submerged_vol_calc(worldTransform * transScaled, points, sizeof(btVector3), 8, GetSurfaceNormal(), GetSurfacePlane(), buffer);

		if (submerged_vol_calc.AreAllAbove())
		{
			// We're above the water
			outSubmergedVolume = 0.0f;
			outCenterOfBuoyancy = btVector3(0, 0, 0);
		}
		else if (submerged_vol_calc.AreAllBelow())
		{
			// We're fully submerged
			outSubmergedVolume = shapeSharedUserData->m_volume;
			outCenterOfBuoyancy = worldTransform.getOrigin();
		}
		else
		{
			// Calculate submerged volume
			int reference_point_bit = 1 << submerged_vol_calc.GetReferencePointIdx();
			for (const Face &f : faces)
			{
				// Test if this face includes the reference point
				if ((f[4] & reference_point_bit) == 0)
				{
					// Triangulate the face (a quad)
					submerged_vol_calc.AddFace(f[0], f[1], f[2]);
					submerged_vol_calc.AddFace(f[0], f[2], f[3]);
				}
			}

			submerged_vol_calc.GetResult(outSubmergedVolume, outCenterOfBuoyancy);

			// Rough estimate
			outSubmergedVolume *= ((float)(M_PI) / 4.0f);

			outSubmergedVolume /= 8;

			if (outSubmergedVolume > shapeSharedUserData->m_volume)
				outSubmergedVolume = shapeSharedUserData->m_volume;
		}
	}
	/*
	The outTotalVolume is the total volume
	The outSubmergedVolume is the volume of submerged part.
	The outCenterOfBuoyancy is in world space
	*/
	void GetSubmergedVolume(btDiscreteDynamicsWorld* world, btCollisionShape *shape, const btTransform &worldTransform, float &outSubmergedVolume, btVector3 &outCenterOfBuoyancy)
	{
		if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
		{
			auto sphereShape = (btSphereShape *)shape;

			GetSubmergedVolumeForSphereShape(world, sphereShape, worldTransform, outSubmergedVolume, outCenterOfBuoyancy);
		}
		else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE)
		{
			auto boxShape = (btBoxShape *)shape;

			GetSubmergedVolumeForBoxShape(world, boxShape, worldTransform, outSubmergedVolume, outCenterOfBuoyancy);
		}
		else if (shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
		{
			auto cylinderShape = (btCylinderShape *)shape;

			GetSubmergedVolumeForCylinderShape(world, cylinderShape, worldTransform, outSubmergedVolume, outCenterOfBuoyancy);
		}
		else if (shape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
		{
			auto capsuleShape = (btCapsuleShape *)shape;

			GetSubmergedVolumeForCapsuleShape(world, capsuleShape, worldTransform, outSubmergedVolume, outCenterOfBuoyancy);
		}
		else if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
		{
			outSubmergedVolume = 0.0f;
			outCenterOfBuoyancy = btVector3(0, 0, 0);

			auto compoundShape = (btCompoundShape *)shape;

			for (int i = 0; i < compoundShape->getNumChildShapes(); i++) {

				auto childShape = compoundShape->getChildShape(i);
				auto childTransform = compoundShape->getChildTransform(i);

				// Get center of mass transform of child
				auto childWorldTransform = worldTransform * childTransform;

				// Recurse to child
				float submerged_volume = 0;
				btVector3 center_of_buoyancy(0, 0, 0);

				GetSubmergedVolume(world, childShape, childWorldTransform, submerged_volume, center_of_buoyancy);

				// Accumulate volumes
				outSubmergedVolume += submerged_volume;

				// The center of buoyancy is the weighted average of the center of buoyancy of our child shapes
				outCenterOfBuoyancy += submerged_volume * center_of_buoyancy;
			}

			if (outSubmergedVolume > 0.0f)
				outCenterOfBuoyancy /= outSubmergedVolume;
		}
	}

	void ApplyBuoyancyImpulse(btDiscreteDynamicsWorld* world, btRigidBody *rigidbody)
	{
		auto physobj = (CCollisionPhysicObject *)rigidbody->getUserPointer();

		physobj->SetWaterVolume(0);

		// For GetSubmergedVolume we transform the surface relative to the body position for increased precision

		float total_volume = 0, submerged_volume = 0;

		btVector3 center_of_buoyancy;

		total_volume = physobj->GetTotalVolume();

		GetSubmergedVolume(world, rigidbody->getCollisionShape(), rigidbody->getWorldTransform(), submerged_volume, center_of_buoyancy);

		//TODO:Use add center of mass offset?
		btVector3 relative_center_of_buoyancy = center_of_buoyancy - rigidbody->getCenterOfMassPosition();

		// If we're not submerged, there's no point in doing the rest of the calculations
		if (submerged_volume > 0.0f && total_volume > 0.0f)
		{
			physobj->SetWaterVolume(submerged_volume);

			auto density_ratio = GetWaterDensity() / physobj->GetDensity();

			auto volume_ratio = submerged_volume / total_volume;

			// Buoyancy force = Density of Fluid * Submerged volume * Magnitude of gravity * Up direction (eq 2.5.1)

			// We should apply this at the center of buoyancy (= center of mass of submerged volume)
			btVector3 buoyancy_force = volume_ratio * (-1.0f) * density_ratio * rigidbody->getGravity() * rigidbody->getMass();

			rigidbody->applyForce(buoyancy_force, relative_center_of_buoyancy);

			// Calculate the velocity of the center of buoyancy relative to the fluid
			btVector3 linear_velocity = rigidbody->getLinearVelocity();
			btVector3 angular_velocity = rigidbody->getAngularVelocity();
			btVector3 center_of_buoyancy_velocity = linear_velocity + angular_velocity.cross(relative_center_of_buoyancy);
			btVector3 relative_center_of_buoyancy_velocity = GetWaterFluidVelocity() - center_of_buoyancy_velocity;

			btVector3 shape_center;
			float shape_radius = 0;
			rigidbody->getCollisionShape()->getBoundingSphere(shape_center, shape_radius);

			// Determine area of the local space bounding sphere in the direction of the relative velocity between the fluid and the center of buoyancy
			float area = 0.0f;
			float relative_center_of_buoyancy_velocity_len_sq = relative_center_of_buoyancy_velocity.length();
			if (relative_center_of_buoyancy_velocity_len_sq > 1.0e-12f)
			{
				area = (float)(M_PI)* shape_radius * shape_radius;
			}

			//F=0.5Cpv^2s
			// Calculate the drag force
			btVector3 drag_force = (0.5f * density_ratio * GetWaterLinearDrag() * area * volume_ratio) * relative_center_of_buoyancy_velocity * relative_center_of_buoyancy_velocity.length();
			btVector3 angular_drag = (-1.0f) * GetWaterAngularDrag() * rigidbody->getMass() * volume_ratio * area * angular_velocity;//* rigidbody->getGravity()

			//rigidbody->applyCentralForce(drag_force);
			//rigidbody->applyTorque(angular_drag);
		}
	}

private:
	btVector3 m_WaterSurfaceNormal;
	btScalar m_WaterSurfacePlane;
	btScalar m_WaterDensity;
	btScalar m_WaterLinearDrag;
	btScalar m_WaterAngularDrag;
	btVector3 m_WaterFluidVelocity;
};

bool CPhysicsManager::CreatePhysicWater(edict_t* ent, float density, float linear_drag, float angular_drag)
{
	auto obj = GetGameObject(ent);

	if (!obj)
	{
		obj = new CGameObject(ent, g_engfuncs.pfnIndexOfEdict(ent));

		AddGameObject(obj);
	}

	auto shape = CreateTriMeshShapeFromBrushEntity(ent);

	if (!shape)
	{
		//No you can't!
		//shape = new btBoxShape(btVector3((ent->v.maxs.x - ent->v.mins.x) * 0.5f, (ent->v.maxs.y - ent->v.mins.y) * 0.5f, (ent->v.maxs.z - ent->v.mins.z) * 0.5f));
		return false;
	}

	btVector3 inSurfaceNormal = btVector3(0, 0, 0);
	btScalar inSurfacePlane = -99999;

	auto shapeSharedUserData = (CBulletCollisionShapeSharedUserData*)shape->getUserPointer();

	auto pIndexArray = shapeSharedUserData->m_pIndexArray;

	for (const auto &face : pIndexArray->vFaceBuffer)
	{
		if (face.plane_normal.z * face.plane_dist > inSurfaceNormal.getZ() * inSurfacePlane)
		{
			inSurfaceNormal = btVector3(face.plane_normal.x, face.plane_normal.y, face.plane_normal.z);
			inSurfacePlane = face.plane_dist;
		}
	}

	auto ghostobj = new CPhysicWaterGhostPhysicObject(obj, inSurfaceNormal, inSurfacePlane, density, linear_drag, angular_drag);

	ghostobj->SetGhostObject(new btPairCachingGhostObject());
	ghostobj->GetGhostObject()->setCollisionShape(shape);
	ghostobj->GetGhostObject()->setCollisionFlags(ghostobj->GetGhostObject()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	obj->AddPhysicObject(ghostobj, m_dynamicsWorld);

	return true;
}

btCollisionShape *CPhysicsManager::CreateCollisionShapeFromParams(CGameObject *obj, PhysicShapeParams *shapeParams)
{
	btCollisionShape *shape = NULL;

	switch (shapeParams->type)
	{
	case PhysicShape_Box:
	{
		btVector3 size(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z);

		shape = new btBoxShape(size);

		auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

		shapeSharedUserData->m_volume = CalcVolumeForBoxShape(size);

		shape->setUserPointer(shapeSharedUserData);

		break;
	}
	case PhysicShape_Sphere:
	{
		auto size = btScalar(shapeParams->size.x);

		shape = new btSphereShape(size);

		auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

		shapeSharedUserData->m_volume = CalcVolumeForSphereShape(size);

		shape->setUserPointer(shapeSharedUserData);

		break;
	}
	case PhysicShape_Capsule:
	{
		if (shapeParams->direction == PhysicShapeDirection_X)
		{
			shape = new btCapsuleShapeX(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Y)
		{
			shape = new btCapsuleShape(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Z)
		{
			shape = new btCapsuleShapeZ(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));
		}

		auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

		shapeSharedUserData->m_volume = CalcVolumeForCapsuleShape(btScalar(shapeParams->size.x), btScalar(shapeParams->size.y));

		shape->setUserPointer(shapeSharedUserData);

		break;
	}
	case PhysicShape_Cylinder:
	{
		if (shapeParams->direction == PhysicShapeDirection_X)
		{
			shape = new btCylinderShapeX(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Y)
		{
			shape = new btCylinderShape(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Z)
		{
			shape = new btCylinderShapeZ(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}

		auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

		if (shapeParams->direction == PhysicShapeDirection_X)
		{
			shapeSharedUserData->m_volume = CalcVolumeForCylinderShapeX(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Y)
		{
			shapeSharedUserData->m_volume = CalcVolumeForCylinderShapeY(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}
		else if (shapeParams->direction == PhysicShapeDirection_Z)
		{
			shapeSharedUserData->m_volume = CalcVolumeForCylinderShapeZ(btVector3(shapeParams->size.x, shapeParams->size.y, shapeParams->size.z));
		}

		shape->setUserPointer(shapeSharedUserData);

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
	btVector3 shapeInertia;

	//btVector3 localInertia;
	shape->calculateLocalInertia(objectParams->mass, shapeInertia);

	btVector3 offset(objectParams->centerofmass.x, objectParams->centerofmass.y, objectParams->centerofmass.z);

	if (!offset.isZero())
	{
		// Calculate the shifted inertia tensor using the Parallel Axis Theorem
		btScalar offsetLengthSquared = offset.length2(); // this gives d.dot(d)

		// this gives d * d^T
		btMatrix3x3 offsetTensor(
			offset.x() * offset.x(), offset.x() * offset.y(), offset.x() * offset.z(),
			offset.y() * offset.x(), offset.y() * offset.y(), offset.y() * offset.z(),
			offset.z() * offset.x(), offset.z() * offset.y(), offset.z() * offset.z()
		);

		// convert shapeInertia to matrix form
		btMatrix3x3 shapeInertiaTensor = btMatrix3x3::getIdentity().scaled(shapeInertia);

		btMatrix3x3 identity = btMatrix3x3::getIdentity();
		btMatrix3x3 massOffset = identity * offsetLengthSquared - offsetTensor;
		btMatrix3x3 shiftedInertiaTensor = shapeInertiaTensor + massOffset * objectParams->mass;

		// Convert the shifted inertia tensor back to vector form
		btVector3 shiftedInertia(shiftedInertiaTensor[0][0], shiftedInertiaTensor[1][1], shiftedInertiaTensor[2][2]);

		shapeInertia = shiftedInertia;
	}

	auto dynamicobj = CreateDynamicObject(
		obj, 
		shape, 
		shapeInertia,
		objectParams->mass,
		objectParams->density,
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

	obj->AddPhysicObject(dynamicobj, m_dynamicsWorld);

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

				constraint->setUserConstraintType(ConstraintType_ClippingHull);

				obj->AddPhysicObject(hullobj, m_dynamicsWorld);

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

bool CPhysicsManager::CreateCompoundPhysicObject(edict_t* ent, PhysicShapeParams **shapeParamArray, size_t numShapeParams, PhysicObjectParams *objectParams)
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
	
	auto mod = EngineGetPrecachedModelByIndex(ent->v.modelindex);

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

	auto compound = new btCompoundShape();

	auto shapeSharedUserData = new CBulletCollisionShapeSharedUserData();

	compound->setUserPointer(shapeSharedUserData);

	for (size_t i = 0; i < numShapeParams; ++i)
	{
		auto shapeParams = shapeParamArray[i];

		btCollisionShape *shape = CreateCollisionShapeFromParams(obj, shapeParams);

		if (shape)
		{
			btTransform trans;
			trans.setIdentity();

			EulerMatrix(btVector3(shapeParams->angles.x, shapeParams->angles.y, shapeParams->angles.z), trans.getBasis());

			trans.setOrigin(btVector3(shapeParams->origin.x, shapeParams->origin.y, shapeParams->origin.z));

			compound->addChildShape(trans, shape);

			auto childShapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

			if (childShapeSharedUserData)
			{
				childShapeSharedUserData->m_volume += childShapeSharedUserData->m_volume;
			}
		}
	}

	//Invalid shape
	if (!compound->getNumChildShapes())
	{
		OnBeforeDeleteCollisionShape(compound);
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

	auto mod = EngineGetPrecachedModelByIndex(ent->v.modelindex);

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

	m_worldVertexResources.clear();

	FreeAllIndexArrays(PhysicIndexArrayFlag_FromBSP, 0);

	for (int i = 0; i < EngineGetMaxPrecacheModel(); ++i)
	{
		auto mod = EngineGetPrecachedModelByIndex(i);

		if (mod && mod->type == mod_brush && mod->name[0])
		{
			if (mod->needload == NL_PRESENT || mod->needload == NL_CLIENT)
			{
				std::shared_ptr<CPhysicVertexArray> worldVertexArray = GenerateWorldVertexArray(mod);

				if (worldVertexArray)
				{
					GenerateBrushIndexArray(mod, worldVertexArray);
				}
			}
		}
	}

	CreatePhysicObjectForBrushModel(r_worldentity);
}

struct GameFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const override
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
				auto ent1 = physobj1->GetGameObject()->GetEdict();

				auto playerobj0 = (CPlayerObject *)physobj0;

				if(playerobj0->IsDuck() && !ent0->v.bInDuck)
					return false;
				else if (!playerobj0->IsDuck() && ent0->v.bInDuck)
					return false;

				//No duck/stand self collision
				if (ent0 == ent1)
					return false;
			}

			if (physobj1->IsPlayerObject())
			{
				auto ent0 = physobj0->GetGameObject()->GetEdict();
				auto ent1 = physobj1->GetGameObject()->GetEdict();

				auto playerobj1 = (CPlayerObject *)physobj1;

				if (playerobj1->IsDuck() && !ent1->v.bInDuck)
					return false;
				else if (!playerobj1->IsDuck() && ent1->v.bInDuck)
					return false;

				//No duck/stand self collision
				if (ent0 == ent1)
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
	//m_dynamicsWorld->getSolverInfo().m_numIterations = 100;

	gContactAddedCallback = CustomMaterialCombinerCallback;
	m_dynamicsWorld->setInternalTickCallback(InternalTickCallback);
}

void CPhysicsManager::Shutdown(void)
{
	RemoveAllGameBodies();

	FreeAllIndexArrays(PhysicIndexArrayFlag_FromBSP, 0);

	m_worldVertexResources.clear();

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
	if (m_dynamicsWorld)
	{
		delete m_dynamicsWorld;
		m_dynamicsWorld = NULL;
	}
}

void CPhysicsManager::StepSimulation(double frametime)
{
	if (!m_bEnabled)
		return;

	m_dynamicsWorld->stepSimulation((btScalar)frametime, 2, m_simrate);
}

void CPhysicsManager::SetSimRate(float rate)
{
	m_simrate = rate;
}

float CPhysicsManager::GetGravityAcceleration() const
{
	return m_gravityAcceleration;
}

void CPhysicsManager::SetGravityAcceleration(float value)
{
	m_gravityAcceleration = value;

	m_dynamicsWorld->setGravity(btVector3(0, 0, -m_gravityAcceleration));
}

int CPhysicsManager::GetSolidPlayerMask()
{
	return m_solidPlayerMask;
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
		obj->RemoveAllConstraints(m_dynamicsWorld);
		obj->RemoveAllPhysicObjects(m_dynamicsWorld);

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
	//SemiClip to all solid players at start, and remove masks later
	if (IsSolidOptimizerEnabled())
	{
		SetPlayerSemiClipMask(gPhysicsManager.GetSolidPlayerMask());
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

	for (size_t i = 0; i < m_physics.size(); ++i)
	{
		m_physics[i]->EndFrame(world);
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

		if (m_follow_flags & FollowEnt_CopyOrigin)
		{
			vec3_t sourceOrigin = m_follow_ent->v.origin;

			/*if (m_follow_flags & FollowEnt_UseBonePosition)
			{
				g_engfuncs.pfnGetBonePosition(m_follow_ent, 21, sourceOrigin, NULL);
			}*/

			if (m_follow_flags & FollowEnt_CopyOriginX)
			{
				ent->v.origin.x = sourceOrigin.x + m_follow_origin_offet.x;
			}
			if (m_follow_flags & FollowEnt_CopyOriginY)
			{
				ent->v.origin.y = sourceOrigin.y + m_follow_origin_offet.y;
			}
			if (m_follow_flags & FollowEnt_CopyOriginZ)
			{
				ent->v.origin.z = sourceOrigin.z + m_follow_origin_offet.z;
			}

			SET_ORIGIN(ent, ent->v.origin);
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

	if (IsSemiClipToEntity(host) || IsPMSemiClipToEntity(host))
	{
		state->solid = SOLID_NOT;
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

	if ((m_follow_flags & FollowEnt_UseMoveTypeFollow) && m_follow_ent)
	{
		state->aiment = ENTINDEX(m_follow_ent);
		state->movetype = MOVETYPE_FOLLOW;
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

void OnBeforeDeleteRigidBody(btRigidBody *rigidbody)
{
	if (rigidbody)
	{
		auto shape = rigidbody->getCollisionShape();

		if (shape)
		{
			OnBeforeDeleteCollisionShape(shape);

			delete shape;
		}

		auto motionState = rigidbody->getMotionState();

		if (motionState)
		{
			delete motionState;
		}
	}
}

void OnBeforeDeletePairCachingGhostObject(btPairCachingGhostObject *ghostobj)
{
	//Should be removed from world before free

	if (ghostobj)
	{
		auto shape = ghostobj->getCollisionShape();

		if (shape)
		{
			OnBeforeDeleteCollisionShape(shape);
			delete shape;
		}
	}
}

void OnBeforeDeleteConstraint(btTypedConstraint *constraint)
{
	if (constraint->getUserConstraintType() == ConstraintType_Wheel)
	{
		auto pSharedUserData = (CBulletBaseSharedUserData*)constraint->getUserConstraintPtr();

		if (pSharedUserData)
		{
			delete pSharedUserData;
		}

		constraint->setUserConstraintPtr(nullptr);
	}
}

void OnBeforeDeleteActionInterface(btActionInterface *action)
{

}

void OnBeforeDeleteCollisionShape(btCollisionShape *shape)
{
	auto shapeSharedUserData = GetSharedUserDataFromCollisionShape(shape);

	if (shapeSharedUserData)
	{
		delete shapeSharedUserData;

		shape->setUserPointer(nullptr);
	}
}
