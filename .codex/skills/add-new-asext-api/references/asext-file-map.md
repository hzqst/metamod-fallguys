# ASEXT File Map

Detailed guide to every file involved when adding a new ASEXT API function, including exact insertion points and surrounding context.

## 1. `asext/serverdef.h` — Private Function Types

**Role**: Declares the function pointer type (`typedef`) and the extern globals (`PRIVATE_FUNCTION_EXTERN`).

**Insertion point**: Group with related functions. CASDocumentation functions are at ~line 138–167.

**Template**:
```cpp
//ClassName::MethodName __fastcall in Windows
typedef ReturnType (SC_SERVER_DECL *fnClassName_MethodName)(
    ClassName *pthis, SC_SERVER_DUMMYARG ParamType1 param1, ParamType2 param2);
PRIVATE_FUNCTION_EXTERN(ClassName_MethodName);
```

**What the macro expands to**:
```cpp
// PRIVATE_FUNCTION_EXTERN(Foo) expands to:
extern fnFoo g_pfn_Foo;
extern fnFoo g_call_original_Foo;
extern hook_t *g_phook_Foo;
```

**Grouping order** (CASDocumentation):
1. RegisterObjectType (has NewCASDocumentation_RegisterObjectType hook)
2. RegisterObjectProperty
3. RegisterGlobalProperty
4. RegisterObjectMethod
5. RegisterObjectBehaviour
6. RegisterFuncDef
7. RegisterEnum
8. RegisterEnumValue
9. SetDefaultNamespace ← new functions go at the end of their group

---

## 2. `asext/signatures.h` — Platform Signatures

**Role**: Defines byte-pattern signatures (Windows + Linux 5.16+) and mangled symbol names (Linux 5.15).

**Structure**:
```
#pragma once
#include <signatures_template.h>

#ifdef _WIN32
    // Windows signatures (~line 5–44)
    #define FunctionName_Signature "..."
#else
    // Linux signatures + symbols (~line 46–143)
    // Comment: XXXX_Symbol is for 5.15, XXXX_Signature is for 5.16+
    #define FunctionName_Signature "..."
    #define FunctionName_Symbol "..."
#endif
```

**Insertion point**: After the last function of the same class group in BOTH the `#ifdef _WIN32` and `#else` blocks.

**Important**: If a signature is unknown, use `""` (empty string). The corresponding `FILL_FROM_*` call in meta_api.cpp must then be omitted or guarded.

---

## 3. `asext/server_hook.cpp` — Function Pointers + Wrappers

**Role**: Defines the actual global function pointer storage and the `ASEXT_*` wrapper functions.

**Two insertion points**:

### 3a. PRIVATE_FUNCTION_DEFINE block (~line 12–22)

```cpp
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectType);
PRIVATE_FUNCTION_DEFINE(CASDocumentation_RegisterObjectProperty);
// ...
PRIVATE_FUNCTION_DEFINE(CASDocumentation_SetDefaultNamespace);  // ← add here
```

This expands to:
```cpp
fnCASDocumentation_SetDefaultNamespace g_pfn_CASDocumentation_SetDefaultNamespace = NULL;
fnCASDocumentation_SetDefaultNamespace g_call_original_CASDocumentation_SetDefaultNamespace = NULL;
hook_t *g_phook_CASDocumentation_SetDefaultNamespace = NULL;
```

### 3b. ASEXT_* wrapper function (~line 115–145)

```cpp
C_DLLEXPORT void ASEXT_SetDefaultNamespace(CASDocumentation* pthis, const char* ns)
{
    SC_SERVER_DUMMYVAR;
    g_call_original_CASDocumentation_SetDefaultNamespace(
        pthis, SC_SERVER_PASS_DUMMYARG ns);
}
```

**SC_SERVER_DUMMYVAR / SC_SERVER_PASS_DUMMYARG**: These macros handle the Windows `__fastcall` dummy EDX parameter. On Linux they expand to nothing.

---

## 4. `asext/meta_api.cpp` — Runtime Filling

**Role**: At plugin load time (`Meta_Attach`), scans the server binary to locate function addresses.

**Four insertion points** within `Meta_Attach`:

### 4a. Windows branch (`#ifdef _WIN32`, ~line 162)
```cpp
FILL_FROM_SIGNATURE(server, ClassName_MethodName);
```

### 4b. Linux 5.16+ branch (`if (CreateInterface("SCServerDLL003"...))`, ~line 206)
```cpp
FILL_FROM_SIGNATURE(server, ClassName_MethodName);
```

### 4c. Linux 5.15 branch (`else`, ~line 262)
```cpp
FILL_FROM_SYMBOL(server, ClassName_MethodName);
```

### 4d. LOG_MESSAGE (after `#endif`, ~line 306)
```cpp
LOG_MESSAGE(PLID, "ClassName_MethodName found at %p", g_pfn_ClassName_MethodName);
```

**Placement**: Always insert after the last function of the same class group in each block to maintain consistent ordering across all three paths.

---

## 5. `asext/asext.h` — Internal Export Header

**Role**: Declares `C_DLLEXPORT` functions for the asext shared library. Used within asext itself and by the build system.

**Insertion point**: End of file, or grouped with related functions.

```cpp
/*
    Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_SetDefaultNamespace(CASDocumentation* pASDoc, const char* ns);
```

**Note**: Some newer functions (RegisterEnum, RegisterEnumValue) were added directly in server_hook.cpp with `C_DLLEXPORT` without a corresponding declaration here. Adding the declaration is recommended for consistency but not strictly required.

---

## 6. `asext/include/asext_api.h` — External Plugin Import API

**Role**: Header included by **external plugins** to dynamically import asext functions at runtime.

**Three insertion points**:

### 6a. typedef + extern (~line 256–332)

Group with related functions:
```cpp
/*
    Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_SetDefaultNamespace)(CASDocumentation* pASDoc, const char* ns);

extern fnASEXT_SetDefaultNamespace ASEXT_SetDefaultNamespace;
```

### 6b. IMPORT_ASEXT_API macro (~line 559–596)

```cpp
#define IMPORT_ASEXT_API(asext) IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallHook);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallCASBaseCallable);\
// ...
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterEnumValue);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_SetDefaultNamespace);\   // ← add here
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateDirectory);\
// ...
```

### 6c. IMPORT_ASEXT_API_DEFINE macro (~line 598–634)

```cpp
#define IMPORT_ASEXT_API_DEFINE() IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDocInitCallback);\
// ...
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterEnumValue);\
IMPORT_FUNCTION_DEFINE(ASEXT_SetDefaultNamespace);\   // ← add here
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterHook);\
// ...
```

See [import-macro-guide.md](import-macro-guide.md) for how these macros work.

---

## File Dependency Graph

```
External Plugin
    │
    ▼
asext/include/asext_api.h  ← typedef + extern + IMPORT macros
    │
    ▼ (dynamic import at runtime via IMPORT_ASEXT_API)
    │
asext/asext.h              ← C_DLLEXPORT declarations
    │
    ▼
asext/server_hook.cpp      ← ASEXT_* wrapper + PRIVATE_FUNCTION_DEFINE
    │
    ▼ (calls g_call_original_*)
    │
asext/meta_api.cpp         ← FILL_FROM_SIGNATURE/SYMBOL (runtime)
    │
    ▼ (reads signatures from)
    │
asext/signatures.h         ← byte patterns + mangled symbols
    │
    ▼ (types defined in)
    │
asext/serverdef.h          ← typedef + PRIVATE_FUNCTION_EXTERN
```
