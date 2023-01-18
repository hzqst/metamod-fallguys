# Third-Party AngelScript Extension

You can register your own hooks or methods in Sven Co-op AngelScript engine.

```
// Include this header file : "metamod-fallguys\asext\include\asext_api.h"
#include "asext_api.h"

// Define this macro in meta_api.cpp
IMPORT_ASEXT_API_DEFINE();

// Add the following code in meta_api.cpp->Meta_Attach

`C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME /* now */,
	META_FUNCTIONS* pFunctionTable, meta_globals_t* pMGlobals,
	gamedll_funcs_t* pGamedllFuncs){`

```
	void *asextHandle = NULL;

#ifdef _WIN32
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.dll", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#else
	LOAD_PLUGIN(PLID, "addons/metamod/dlls/asext.so", PLUG_LOADTIME::PT_ANYTIME, &asextHandle);
#endif
	if (!asextHandle)
	{
		LOG_ERROR(PLID, "asext dll handle not found!");
		return FALSE;
	}

	IMPORT_ASEXT_API(asext);

```

## Register your own methods in AngelScript engine

```
int SC_SERVER_DECL CASEngineFuncs__TestFunc(void* pthis SC_SERVER_DUMMYARG_NOCOMMA)
{
	return 114514;
}

//Must be registered before AS initialization, Meta_Attach is okay

	ASEXT_RegisterDocInitCallback([](void *pASDoc) {

		ASEXT_RegisterObjectMethod(pASDoc,
			"A Test Function", "CEngineFuncs", "int TestFunc()",
			(void *)CASEngineFuncs__TestFunc, 3);

	});
	
```

Now you can call this from game, you will get test = 114514 if registeration works fine :

```
int test = g_EngineFuncs.TestFunc();
```

## Register your own hooks in AngelScript engine

```

// Define a global var

void *g_PlayerPostThinkPostHook = NULL;

```

```

//Must be registered before AS initialization, Meta_Attach is okay

g_PlayerPostThinkPostHook = ASEXT_RegisterHook("Post call of gEntityInterface.pfnPlayerPostThink", StopMode_CALL_ALL, 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer");

```

```

//Add to where you would like to call AngelScript hooks
void NewPlayerPostThink_Post(edict_t *pEntity)
{
	if(ASEXT_CallHook)//The second arg must be zero, the third, 4th, 5th, 6th... args are the real args pass to AngelScript VM.
		(*ASEXT_CallHook)(g_PlayerPostThinkPostHook, 0, pEntity->pvPrivateData);

	SET_META_RESULT(MRES_IGNORED);
}
```


//Now you can register hook from AngelScript map script or plugin :
```

void MapInit()
{
    g_Hooks.RegisterHook(Hooks::Player::PlayerPostThinkPost, @PlayerPostThinkPost);
}

```