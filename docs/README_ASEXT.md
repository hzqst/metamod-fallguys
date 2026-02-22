# Third-Party AngelScript Extension

You can register your own hooks or methods in Sven Co-op AngelScript engine.

```cpp
// Include this header file : "metamod-fallguys\asext\include\asext_api.h"
#include "asext_api.h"

// Define this macro in meta_api.cpp
IMPORT_ASEXT_API_DEFINE();
```

```cpp
// Add the following code in meta_api.cpp->Meta_Attach

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

```cpp
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

Now call g_EngineFuncs.TestFunc from angelscript, you will get test = 114514 if the registeration works fine

```angelscript
int test = g_EngineFuncs.TestFunc();
```

## Register your own hooks in AngelScript engine

```cpp

// Define a global var

void *g_PlayerPostThinkPostHook = NULL;

```

```cpp

//Must be registered before AS initialization, Meta_Attach is okay

g_PlayerPostThinkPostHook = ASEXT_RegisterHook("Post call of gEntityInterface.pfnPlayerPostThink", StopMode_CALL_ALL, 2, ASHookFlag_MapScript | ASHookFlag_Plugin, "Player", "PlayerPostThinkPost", "CBasePlayer@ pPlayer");

```

```cpp

//Add to where you would like to call AngelScript hooks

void NewPlayerPostThink_Post(edict_t *pEntity)
{
	if(ASEXT_CallHook)//The second arg must be zero, the third, 4th, 5th, 6th... args are the real args pass to AngelScript VM.
		(*ASEXT_CallHook)(g_PlayerPostThinkPostHook, 0, pEntity->pvPrivateData);

	SET_META_RESULT(MRES_IGNORED);
}
```

Now you can register hook from AngelScript map script or plugin :

```angelscript

void MapInit()
{
    g_Hooks.RegisterHook(Hooks::Player::PlayerPostThinkPost, @PlayerPostThinkPost);
}

```

## Expose yourself as a macro to AngelScript module

```cpp

void RegisterAngelScriptMethods(void)
{
	ASEXT_RegisterScriptBuilderDefineCallback([](CScriptBuilder *pScriptBuilder) {

		ASEXT_CScriptBuilder_DefineWord(pScriptBuilder, "METAMOD_PLUGIN_FALLGUYS");

	});
}
```

## Build Configuration (v20260208d)

Starting from v20260208d, asext requires the `angelscript.h`. Add the following include directory to your CMake configuration:

```cmake
include_directories(
    ${CMAKE_SOURCE_DIR}/hlsdk/common
    ${CMAKE_SOURCE_DIR}/hlsdk/dlls
    ${CMAKE_SOURCE_DIR}/hlsdk/pm_shared
    ${CMAKE_SOURCE_DIR}/hlsdk/engine
    ${CMAKE_SOURCE_DIR}/metamod
    ${CMAKE_SOURCE_DIR}/thirdparty/angelscript-sdk/angelscript/include # Mandatory for asext
)
```

Alternatively, you can copy `/thirdparty/angelscript-sdk/angelscript/include/angelscript.h` into your project (not recommended).

## Set default namespace in AngelScript

```cpp
void ASEXT_SetDefaultNamespace(CASDocumentation* pthis, const char* ns);
```

Use this to register global properties (or other symbols) under a custom namespace.

### Namespace Usage

```cpp
ASEXT_SetDefaultNamespace(pASDoc, "MyNameSpace");

ASEXT_RegisterGlobalProperty(pASDoc, "zzz", "xxx", &whatever);

ASEXT_SetDefaultNamespace(pASDoc, "");
```

## Iterate AngelScript `dictionary`

The following APIs allow iterating over entries in an AngelScript `dictionary` object from C++:

```cpp
void ASEXT_CScriptDictionary_CIterator_begin(CScriptDictionary *pScriptDictionary, CScriptDictionary_CIterator *it);

void ASEXT_CScriptDictionary_CIterator_end(CScriptDictionary *pScriptDictionary, CScriptDictionary_CIterator *it);

bool ASEXT_CScriptDictionary_CIterator_operator_NE(CScriptDictionary_CIterator *a1, CScriptDictionary_CIterator *a2);

bool ASEXT_CScriptDictionary_CIterator_GetValue(CScriptDictionary_CIterator *it, void *data, int typeId);

const char *ASEXT_CScriptDictionary_CIterator_GetKey(CScriptDictionary_CIterator *it);

void ASEXT_CScriptDictionary_CIterator_operator_PP(CScriptDictionary_CIterator *it);

asITypeInfo* ASEXT_CASBaseManager_GetTypeInfoByName(CASServerManager* pthis, const sc_stdstring *name);
```

### Dictionary Iteration Usage

```cpp
    if ( !ASEXT_CScriptDictionary_IsEmpty(pScriptDictionary) )
    {
      std_string typeName{};
      typeName.assign("string");

      asITypeInfo* pStringTypeInfo = ASEXT_CASBaseManager_GetTypeInfoByName(ASEXT_GetServerManager(), &typeName);

      CScriptDictionary_CIterator it{}, itend{};
      ASEXT_CScriptDictionary_begin(pScriptDictionary, &it)
      ASEXT_CScriptDictionary_end(pScriptDictionary, &itend);
      for(; ASEXT_CScriptDictionary_CIterator_operator_NE(it, itend); ASEXT_CScriptDictionary_CIterator_operator_PP(&it) ) // Counter part of "it = begin; it != end; it++"
      {
        CString value{};
        value.assign("", 0);
        if ( ASEXT_CScriptDictionary_CIterator_GetValue(it, &value, pStringTypeInfo->GetTypeId() ) )
        {
            const CString*key = ASEXT_CScriptDictionary_CIterator_GetKey(it);
            
            //You have both "key" and "value" as angelscript strings now.
        }
        value.dtor();
      }
    }
```