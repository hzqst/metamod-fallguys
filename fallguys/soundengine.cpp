#include <extdll.h>			// always

#include <meta_api.h>		// of course

#include <fmod.hpp>
#include <fmod_errors.h>

#include "enginedef.h"
#include "serverdef.h"
#include "soundengine.h"

static DLHANDLE g_FMOD_DllHandle = NULL;
static FMOD_SYSTEM *g_FMOD_System = NULL;

#define FMOD_FUNCTION_DEFINE(name) static decltype(name) * g_pfn_##name

FMOD_FUNCTION_DEFINE(FMOD_System_Create);
FMOD_FUNCTION_DEFINE(FMOD_System_SetCallback);
FMOD_FUNCTION_DEFINE(FMOD_System_SetOutput);
FMOD_FUNCTION_DEFINE(FMOD_System_Init);
FMOD_FUNCTION_DEFINE(FMOD_System_Close);
FMOD_FUNCTION_DEFINE(FMOD_System_Release);
FMOD_FUNCTION_DEFINE(FMOD_System_CreateSound);
FMOD_FUNCTION_DEFINE(FMOD_Sound_GetLength);
FMOD_FUNCTION_DEFINE(FMOD_Sound_GetFormat);
FMOD_FUNCTION_DEFINE(FMOD_Sound_Release);

#if 0
bool LoadFMOD_Client()
{
	bool success = false;

	auto clientHandle = gpMetaUtilFuncs->pfnGetModuleHandle(CLIENT_DLL_NAME);

	if (clientHandle)
	{
		auto FMOD_DllHandle = gpMetaUtilFuncs->pfnGetModuleHandle(FMOD_DLL_NAME);

		if (FMOD_DllHandle)
		{

#define FMOD_DLSYM_CLIEN(name) g_pfn_##name = (decltype(g_pfn_##name))gpMetaUtilFuncs->pfnGetProcAddress(FMOD_DllHandle, #name);

			FMOD_DLSYM_CLIEN(FMOD_System_Create);
			FMOD_DLSYM_CLIEN(FMOD_System_Init);
			FMOD_DLSYM_CLIEN(FMOD_System_Close);
			FMOD_DLSYM_CLIEN(FMOD_System_Release);
			FMOD_DLSYM_CLIEN(FMOD_System_CreateSound);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetLength);
			FMOD_DLSYM_CLIEN(FMOD_Sound_GetFormat);
			FMOD_DLSYM_CLIEN(FMOD_Sound_Release);

			success = true;

			gpMetaUtilFuncs->pfnCloseModuleHandle(FMOD_DllHandle);
		}

		gpMetaUtilFuncs->pfnCloseModuleHandle(clientHandle);
	}

	return success;
}
#endif

static FMOD_RESULT F_CALLBACK EmptyFMODCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
	return FMOD_OK;
}

static bool LoadFMOD_Server()
{
	char szGameDir[32] = { 0 };
	g_engfuncs.pfnGetGameDir(szGameDir);

	char szDllPath[64] = { 0 };
	snprintf(szDllPath, sizeof(szDllPath) - 1, FMOD_DLL_PATH, szGameDir);
	szDllPath[sizeof(szDllPath) - 1] = 0;

	g_FMOD_DllHandle = gpMetaUtilFuncs->pfnLoadLibrary(szDllPath);

	if (!g_FMOD_DllHandle)
	{
		LOG_ERROR(PLID, "Failed to load fmodex dll from \"%s\"! The SoundEngine API may not work.", szDllPath);
		return false;
	}

#define FMOD_DLSYM(name) g_pfn_##name = (decltype(g_pfn_##name))gpMetaUtilFuncs->pfnGetProcAddress(g_FMOD_DllHandle, #name);\
	if (!g_pfn_##name)\
	{\
		if (g_FMOD_DllHandle)\
		{\
			gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);\
			g_FMOD_DllHandle = NULL;\
		}\
		LOG_ERROR(PLID, "Failed to locate " #name " from fmodex dll!");\
		return false;\
	}

	FMOD_DLSYM(FMOD_System_Create);
	FMOD_DLSYM(FMOD_System_SetCallback);
	FMOD_DLSYM(FMOD_System_SetOutput);
	FMOD_DLSYM(FMOD_System_Init);
	FMOD_DLSYM(FMOD_System_Close);
	FMOD_DLSYM(FMOD_System_Release);
	FMOD_DLSYM(FMOD_System_CreateSound);
	FMOD_DLSYM(FMOD_Sound_GetLength);
	FMOD_DLSYM(FMOD_Sound_GetFormat);
	FMOD_DLSYM(FMOD_Sound_Release);

	auto result = g_pfn_FMOD_System_Create(&g_FMOD_System);

	if (result != FMOD_OK)
	{
		if (g_FMOD_DllHandle)
		{
			gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);
			g_FMOD_DllHandle = NULL;
		}

		LOG_ERROR(PLID, "Failed to FMOD_System_Create, result: %d", result);
		return false;
	}

	result = g_pfn_FMOD_System_SetCallback(g_FMOD_System, EmptyFMODCallback);

	if (result != FMOD_OK)
	{
		if (g_FMOD_System)
		{
			g_pfn_FMOD_System_Release(g_FMOD_System);
			g_FMOD_System = NULL;
		}

		if (g_FMOD_DllHandle)
		{
			gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);
			g_FMOD_DllHandle = NULL;
		}

		LOG_ERROR(PLID, "Failed to FMOD_System_SetCallback, result: %d", result);
		return false;
	}

	g_pfn_FMOD_System_SetOutput(g_FMOD_System, FMOD_OUTPUTTYPE_NOSOUND);

	if (result != FMOD_OK)
	{
		if (g_FMOD_System)
		{
			g_pfn_FMOD_System_Release(g_FMOD_System);
			g_FMOD_System = NULL;
		}

		if (g_FMOD_DllHandle)
		{
			gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);
			g_FMOD_DllHandle = NULL;
		}

		LOG_ERROR(PLID, "Failed to FMOD_System_SetOutput, result: %d", result);
		return false;
	}

	result = g_pfn_FMOD_System_Init(g_FMOD_System, 0, FMOD_INIT_NORMAL, NULL);

	if (result != FMOD_OK)
	{
		if (g_FMOD_System)
		{
			g_pfn_FMOD_System_Release(g_FMOD_System);
			g_FMOD_System = NULL;
		}

		if (g_FMOD_DllHandle)
		{
			gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);
			g_FMOD_DllHandle = NULL;
		}

		LOG_ERROR(PLID, "Failed to FMOD_System_Init, result: %d", result);
		return false;
	}

	return true;
}

bool LoadFMOD()
{
	if (LoadFMOD_Server())
		return true;

	return true;
}

void UnloadFMOD()
{
	if (g_FMOD_System)
	{
		g_pfn_FMOD_System_Close(g_FMOD_System);
		g_pfn_FMOD_System_Release(g_FMOD_System);
		g_FMOD_System = NULL;
	}

	if (g_FMOD_DllHandle)
	{
		gpMetaUtilFuncs->pfnFreeLibrary(g_FMOD_DllHandle);
		g_FMOD_DllHandle = NULL;
	}
}

//SoundEngine_SoundInfo

void SoundEngine_SoundInfo_ctor(SoundEngine_SoundInfo *pthis)
{
	pthis->type = 0;
	pthis->format = 0;
	pthis->channels = 0;
	pthis->bits = 0;
	pthis->length = 0;
}

void SoundEngine_SoundInfo_copyctor(SoundEngine_SoundInfo *a1, SoundEngine_SoundInfo *a2)
{
	a1->type = a2->type;
	a1->format = a2->format;
	a1->channels = a2->channels;
	a1->bits = a2->bits;
	a1->length = a2->length;
}

SoundEngine_SoundInfo * SC_SERVER_DECL SoundEngine_SoundInfo_opassign(SoundEngine_SoundInfo *a1, SC_SERVER_DUMMYARG SoundEngine_SoundInfo *a2)
{
	SoundEngine_SoundInfo_copyctor(a1, a2);

	return a1;
}

void SoundEngine_SoundInfo_dtor(SoundEngine_SoundInfo *pthis)
{

}

SoundEngine_SoundInfo::SoundEngine_SoundInfo()
{
	SoundEngine_SoundInfo_ctor(this);
}

bool SoundEngine_GetSoundInfo(const char *szSoundName, SoundEngine_SoundInfo *SoundInfo)
{
	if (!g_FMOD_System)
		return false;

	char szFuillPath[256];
	snprintf(szFuillPath, sizeof(szFuillPath) - 1, "sound/%s", szSoundName);
	szFuillPath[sizeof(szFuillPath) - 1] = 0;

	int length = 0;
	auto buf = g_engfuncs.pfnLoadFileForMe(szFuillPath, &length);

	if (!buf)
		return false;

	// Set up the FMOD_CREATESOUNDEXINFO structure
	FMOD_CREATESOUNDEXINFO exinfo = { 0 };
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length = length;

	bool success = false;

	FMOD_SOUND *sound = NULL;

	auto result = g_pfn_FMOD_System_CreateSound(g_FMOD_System, (const char *)buf, FMOD_OPENMEMORY, &exinfo, &sound);

	if (result == FMOD_OK)
	{
		FMOD_SOUND_TYPE type = FMOD_SOUND_TYPE_UNKNOWN;
		FMOD_SOUND_FORMAT format = FMOD_SOUND_FORMAT_NONE;
		int channels = 0;
		int bits = 0;

		result = g_pfn_FMOD_Sound_GetFormat(sound, (FMOD_SOUND_TYPE *)&type, (FMOD_SOUND_FORMAT *)&format, &channels, &bits);

		if (result == FMOD_OK)
		{
			SoundInfo->type = type;
			SoundInfo->format = format;
			SoundInfo->channels = channels;
			SoundInfo->bits = bits;

			success = true;
		}

		unsigned int sound_len = 0;

		result = g_pfn_FMOD_Sound_GetLength(sound, &sound_len, FMOD_TIMEUNIT_MS);

		if (result == FMOD_OK)
		{
			SoundInfo->length = sound_len;
		}

		g_pfn_FMOD_Sound_Release(sound);
	}

	g_engfuncs.pfnFreeFile(buf);

	return success;
}