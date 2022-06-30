#include <extdll.h>

#include <dllapi.h>
#include <meta_api.h>

#include <cl_entity.h>
#include <entity_state.h>

#include <vector>

#include "asext.h"

#include "serverdef.h"

PRIVATE_FUNCTION_DEFINE(CASHook_Call);
PRIVATE_FUNCTION_DEFINE(CASHook_CASHook);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectType);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectProperty);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectMethod);

bool g_ASDocInit = false;
std::vector<fnASDocInitCallback> g_ASDocInitCallbacks;
std::vector<CASHook *> g_ASHooks;

C_DLLEXPORT fnASEXT_CallHook ASEXT_CallHook = NULL;

void ASEXT_RegisterEntityState(CASDocumentation *pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	g_call_original_CASDocumentation_RegisterObjectType(pthis, SC_SERVER_PASS_DUMMYARG "Entity states transmit to client", "entity_state_t", 0, 0x40001u);
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Fields which are filled in by routines outside of delta compression", "entity_state_t", "int entityType", offsetof(entity_state_t, entityType));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Index into cl_entities array for this entity.", "entity_state_t", "int number", offsetof(entity_state_t, number));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float msg_time", offsetof(entity_state_t, msg_time));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "Message number last time the player/entity state was updated.", "entity_state_t", "int messagenum", offsetof(entity_state_t, messagenum));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector origin", offsetof(entity_state_t, origin));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector angles", offsetof(entity_state_t, angles));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int modelindex", offsetof(entity_state_t, modelindex));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int sequence", offsetof(entity_state_t, sequence));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float frame", offsetof(entity_state_t, frame));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int colormap", offsetof(entity_state_t, colormap));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int16 skin", offsetof(entity_state_t, skin));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int16 solid", offsetof(entity_state_t, solid));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int effects", offsetof(entity_state_t, effects));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float scale", offsetof(entity_state_t, scale));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int8 eflags", offsetof(entity_state_t, eflags));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int rendermode", offsetof(entity_state_t, rendermode));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int renderamt", offsetof(entity_state_t, renderamt));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int renderfx", offsetof(entity_state_t, renderfx));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int movetype", offsetof(entity_state_t, movetype));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float animtime", offsetof(entity_state_t, animtime));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float framerate", offsetof(entity_state_t, framerate));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int body", offsetof(entity_state_t, body));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector velocity", offsetof(entity_state_t, velocity));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector mins", offsetof(entity_state_t, mins));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector maxs", offsetof(entity_state_t, maxs));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int aiment", offsetof(entity_state_t, aiment));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int owner", offsetof(entity_state_t, owner));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float friction", offsetof(entity_state_t, friction));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float gravity", offsetof(entity_state_t, gravity));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int team", offsetof(entity_state_t, team));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int playerclass", offsetof(entity_state_t, playerclass));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int health", offsetof(entity_state_t, health));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int spectator", offsetof(entity_state_t, spectator));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int weaponmodel", offsetof(entity_state_t, weaponmodel));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int gaitsequence", offsetof(entity_state_t, gaitsequence));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector basevelocity", offsetof(entity_state_t, basevelocity));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int usehull", offsetof(entity_state_t, usehull));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int oldbuttons", offsetof(entity_state_t, oldbuttons));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int onground", offsetof(entity_state_t, onground));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iStepLeft", offsetof(entity_state_t, iStepLeft));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float flFallVelocity", offsetof(entity_state_t, flFallVelocity));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fov", offsetof(entity_state_t, fov));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int weaponanim", offsetof(entity_state_t, weaponanim));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser1", offsetof(entity_state_t, iuser1));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser2", offsetof(entity_state_t, iuser2));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser3", offsetof(entity_state_t, iuser3));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "int iuser4", offsetof(entity_state_t, iuser4));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser1", offsetof(entity_state_t, fuser1));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser2", offsetof(entity_state_t, fuser2));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser3", offsetof(entity_state_t, fuser3));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "float fuser4", offsetof(entity_state_t, fuser4));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser1", offsetof(entity_state_t, vuser1));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser2", offsetof(entity_state_t, vuser2));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser3", offsetof(entity_state_t, vuser3));
	g_call_original_CASDocumentation_RegisterObjectProperty(pthis, SC_SERVER_PASS_DUMMYARG "", "entity_state_t", "Vector vuser4", offsetof(entity_state_t, vuser4));
}

C_DLLEXPORT void ASEXT_RegisterObjectMethod(void *pthis, const char *docs, const char *name, const char *func, void *pfn, int type)
{
	SC_SERVER_DUMMYVAR;
	CASMethodRegistration reg;
	reg.pfnMethod = pfn;
	g_call_original_CASDocumentation_RegisterObjectMethod((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, func, &reg, type);
}

C_DLLEXPORT void ASEXT_RegisterObjectType(void *pthis, const char *docs, const char *name, int unk, unsigned int flags)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectType((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, unk, flags);
}

C_DLLEXPORT void ASEXT_RegisterObjectProperty(void *pthis, const char *docs, const char *name, const char *prop, int offset)
{
	SC_SERVER_DUMMYVAR;

	g_call_original_CASDocumentation_RegisterObjectProperty((CASDocumentation *)pthis, SC_SERVER_PASS_DUMMYARG docs, name, prop, offset);
}

int SC_SERVER_DECL NewCASDocumentation_RegisterObjectType(CASDocumentation *pthis, SC_SERVER_DUMMYARG const char *docs, const char *name, int a4, unsigned int flags)
{
	if (name && docs && !strcmp(name, "CSurvivalMode") && !strcmp(docs, "Survival Mode handler") && flags == 0x40001u)
	{
		ASEXT_RegisterEntityState(pthis, SC_SERVER_PASS_DUMMYARG_NOCOMMA);

		for (size_t i = 0; i < g_ASDocInitCallbacks.size(); ++i)
		{
			g_ASDocInitCallbacks[i](pthis);
		}

		g_ASDocInitCallbacks.clear();
		g_ASDocInit = true;
	} 

	return g_call_original_CASDocumentation_RegisterObjectType(pthis, SC_SERVER_PASS_DUMMYARG docs, name, a4, flags);
}

C_DLLEXPORT bool ASEXT_RegisterDocInitCallback(fnASDocInitCallback callback)
{
	if (g_ASDocInit)
		return false;

	g_ASDocInitCallbacks.emplace_back(callback);

	return true;
}

C_DLLEXPORT void *ASEXT_RegisterHook(const char *docs, int stopMode, int type, int flags, const char *domain, const char *func, const char *args)
{
	SC_SERVER_DUMMYVAR;

	CASHookRegistration reg;
	reg.unk = 0;
	reg.stopMode = stopMode;
	reg.docs = docs;

	CASHook *hook = new CASHook;
	g_ASHooks.emplace_back(hook);

	g_pfn_CASHook_CASHook(hook, SC_SERVER_PASS_DUMMYARG type, ASHookFlag_MapScript | ASHookFlag_Plugin, domain, func, args, &reg);

	return hook;
}