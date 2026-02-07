---
name: add-new-asext-api
description: Add a new exported API function to the asext module, wrapping a private server.dll/server.so function for use by external plugins. Use when you need to expose CASDocumentation, CASDirectoryList, or other AngelScript-related private functions through the ASEXT_* API layer.
---

# Add New ASEXT API

## Overview

This skill covers the end-to-end process of adding a new exported API function to the **asext** module. It wraps a private function from `server.dll` (Windows) / `server.so` (Linux) and exposes it through the `ASEXT_*` API layer so that external plugins (e.g., fallguys, ascurl) can import and call it at runtime.

**When to use**: You have identified a private function in the game server binary (via IDA Pro) that you want to make available to other metamod plugins through asext's dynamic import system.

**Prerequisite knowledge**: Familiarity with the [add-private-function-with-signatures](../add-private-function-with-signatures/SKILL.md) skill for IDA analysis and signature creation basics.

## Files to Modify (6 files)

| # | File | Purpose |
|---|------|---------|
| 1 | `asext/serverdef.h` | Function typedef + `PRIVATE_FUNCTION_EXTERN` |
| 2 | `asext/signatures.h` | Windows signature, Linux signature + symbol |
| 3 | `asext/server_hook.cpp` | `PRIVATE_FUNCTION_DEFINE` + `ASEXT_*` wrapper |
| 4 | `asext/meta_api.cpp` | `FILL_FROM_*` calls (3 code paths) + `LOG_MESSAGE` |
| 5 | `asext/asext.h` | `C_DLLEXPORT` declaration (internal header) |
| 6 | `asext/include/asext_api.h` | typedef + extern + `IMPORT_ASEXT_API` + `IMPORT_ASEXT_API_DEFINE` macros |

See [asext-file-map.md](references/asext-file-map.md) for detailed file roles and layout.

## Workflow

### Step 1: Analyze Target Function and Create Signatures

Use the [add-private-function-with-signatures](../add-private-function-with-signatures/SKILL.md) skill for the full IDA Pro workflow: decompile, extract bytes, craft signature with wildcards, and verify uniqueness.

**Key observations to record** during IDA analysis:
- **Return type** (void, int, etc.)
- **Calling convention** (Windows: `__thiscall`/`__fastcall`/`__cdecl`; Linux: `__cdecl`)
- **Parameter list** (types and count)
- **Struct offsets** used inside the function (may differ between Windows and Linux)

**Repeat for both platforms** (server.dll AND server.so) if both IDA databases are available.

**For Linux 5.15**: Also record the C++ mangled symbol name (e.g. `_ZN16CASDocumentation19SetDefaultNamespaceEPKc`). If the binary is stripped (no symbols), only a signature is needed.

### Step 2: Define Function Type in `asext/serverdef.h`

Add the typedef and extern declaration alongside existing `CASDocumentation_*` functions:

```cpp
// Comment describing the function and its Windows calling convention
typedef ReturnType (SC_SERVER_DECL *fnClassName_MethodName)(
    ClassName *pthis, SC_SERVER_DUMMYARG ParamType1 param1, ParamType2 param2);
PRIVATE_FUNCTION_EXTERN(ClassName_MethodName);
```

**Naming convention**: `fnClassName_MethodName` for the typedef, `ClassName_MethodName` for the macro name.

**Calling convention selection**:
- Class member functions (most cases): `SC_SERVER_DECL` + `SC_SERVER_DUMMYARG`
- Static / free C functions: `SC_SERVER_CDECL` (no dummy arg)

**If the function is also a hook target** (needs inline hook), add the `New*` declaration:
```cpp
ReturnType SC_SERVER_DECL NewClassName_MethodName(
    ClassName *pthis, SC_SERVER_DUMMYARG ParamType1 param1, ParamType2 param2);
```

### Step 3: Add Signatures in `asext/signatures.h`

The file has two sections separated by `#ifdef _WIN32` / `#else`.

**Windows section** (inside `#ifdef _WIN32`):
```cpp
#define ClassName_MethodName_Signature "\x56\x8B\xF1\x57..."
```

**Linux section** (inside `#else`):
```cpp
#define ClassName_MethodName_Signature "\x56\x53\x83\xEC\x2A..."
#define ClassName_MethodName_Symbol "_ZN<len>ClassName<len>MethodNameE<param_types>"
```

**Placement**: Insert after the last function of the same class group (e.g., after `CASDocumentation_RegisterEnumValue` for CASDocumentation functions).

**If signature is unknown for a platform**, use empty string `""` as placeholder — the corresponding `FILL_FROM_*` call must be omitted from meta_api.cpp until the signature is provided.

### Step 4: Define Function Pointer in `asext/server_hook.cpp`

**4a. Add PRIVATE_FUNCTION_DEFINE** at the top of the file (with other defines):

```cpp
PRIVATE_FUNCTION_DEFINE(ClassName_MethodName);
```

This expands to:
```cpp
fnClassName_MethodName g_pfn_ClassName_MethodName = NULL;
fnClassName_MethodName g_call_original_ClassName_MethodName = NULL;
```

**4b. Add the ASEXT_* wrapper function** (the exported API that external plugins call):

```cpp
C_DLLEXPORT void ASEXT_MethodName(ClassName* pthis, ParamType1 param1)
{
    SC_SERVER_DUMMYVAR;
    g_call_original_ClassName_MethodName(pthis, SC_SERVER_PASS_DUMMYARG param1);
}
```

**Naming convention**: `ASEXT_MethodName` (drop the class name prefix, keep the method name).

**Important**: The wrapper uses `g_call_original_*` (not `g_pfn_*`) to call through the original function pointer. For non-hooked functions, both point to the same address.

### Step 5: Fill Function Pointer in `asext/meta_api.cpp`

There are **three code paths** in `Meta_Attach` that must be updated:

**5a. Windows branch** (`#ifdef _WIN32`, ~line 162+):

```cpp
FILL_FROM_SIGNATURE(server, ClassName_MethodName);
```

Or if using a caller-based signature:
```cpp
FILL_FROM_SIGNATURED_CALLER_FROM_END(server, ClassName_MethodName, <offset>);
```

**5b. Linux 5.16+ branch** (`if (CreateInterface("SCServerDLL003", nullptr))`, ~line 206+):

```cpp
FILL_FROM_SIGNATURE(server, ClassName_MethodName);
```

Or with caller-based:
```cpp
FILL_FROM_SIGNATURED_CALLER_FROM_START(server, ClassName_MethodName, <offset>);
```

**5c. Linux 5.15 branch** (`else`, ~line 262+):

```cpp
FILL_FROM_SYMBOL(server, ClassName_MethodName);
```

**5d. Add LOG_MESSAGE** (after `#endif`, ~line 300+):

```cpp
LOG_MESSAGE(PLID, "ClassName_MethodName found at %p", g_pfn_ClassName_MethodName);
```

**Placement in each block**: Insert after the last function of the same class group to maintain ordering.

For the full list of `FILL_FROM_*` macros and the caller-based signature approach, see [add-private-function-with-signatures](../add-private-function-with-signatures/SKILL.md) Step 5 and Step 5b.

### Step 6: Add Declaration in `asext/asext.h`

Add the `C_DLLEXPORT` declaration for the wrapper function:

```cpp
/*
    Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_MethodName(ClassName* pthis, ParamType1 param1);
```

**Placement**: At the end of the file, or grouped with related functions.

### Step 7: Add to Import API in `asext/include/asext_api.h`

This file is used by **external plugins** to dynamically import asext functions. Three locations must be updated:

**7a. Add typedef + extern** (alongside existing ASEXT_* declarations):

```cpp
/*
    Must be called inside DocInitCallback
*/
typedef void(*fnASEXT_MethodName)(ClassName* pASDoc, ParamType1 param1);

extern fnASEXT_MethodName ASEXT_MethodName;
```

**7b. Add to `IMPORT_ASEXT_API` macro** (the DLSYM import block):

```cpp
IMPORT_FUNCTION_DLSYM(asext, ASEXT_MethodName);\
```

**7c. Add to `IMPORT_ASEXT_API_DEFINE` macro** (the definition block):

```cpp
IMPORT_FUNCTION_DEFINE(ASEXT_MethodName);\
```

See [import-macro-guide.md](references/import-macro-guide.md) for details on how these macros work.

## Complete Example: `ASEXT_SetDefaultNamespace`

Below is the exact diff for adding `CASDocumentation::SetDefaultNamespace` support:

### serverdef.h
```cpp
typedef void (SC_SERVER_DECL* fnCASDocumentation_SetDefaultNamespace)(
    CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* ns);
PRIVATE_FUNCTION_EXTERN(CASDocumentation_SetDefaultNamespace);
```

### signatures.h
```cpp
// Windows (direct function body, __thiscall)
#define CASDocumentation_SetDefaultNamespace_Signature \
    "\x56\x8B\xF1\x57\x8B\x7C\x24\x2A\x57\x8B\x4E\x24\x8B\x01\xFF\x90\xB4\x00\x00\x00\x85\xC0\x78\x03\x89\x7E\x28"

// Linux (direct function body, cdecl)
#define CASDocumentation_SetDefaultNamespace_Signature \
    "\x56\x53\x83\xEC\x2A\x8B\x5C\x24\x2A\x8B\x74\x24\x2A\x8B\x43\x20\x8B\x10\x89\x74\x24\x2A\x89\x04\x24\xFF\x92\xB4\x00\x00\x00\x85\xC0\x78\x03\x89\x73\x24"

#define CASDocumentation_SetDefaultNamespace_Symbol \
    "_ZN16CASDocumentation19SetDefaultNamespaceEPKc"
```

### server_hook.cpp
```cpp
PRIVATE_FUNCTION_DEFINE(CASDocumentation_SetDefaultNamespace);

C_DLLEXPORT void ASEXT_SetDefaultNamespace(CASDocumentation* pthis, const char* ns)
{
    SC_SERVER_DUMMYVAR;
    g_call_original_CASDocumentation_SetDefaultNamespace(pthis, SC_SERVER_PASS_DUMMYARG ns);
}
```

### meta_api.cpp
```cpp
// Windows
FILL_FROM_SIGNATURE(server, CASDocumentation_SetDefaultNamespace);

// Linux 5.16+
FILL_FROM_SIGNATURE(server, CASDocumentation_SetDefaultNamespace);

// Linux 5.15
FILL_FROM_SYMBOL(server, CASDocumentation_SetDefaultNamespace);

// Logging
LOG_MESSAGE(PLID, "CASDocumentation_SetDefaultNamespace found at %p",
    g_pfn_CASDocumentation_SetDefaultNamespace);
```

### asext.h
```cpp
/*
    Must be called inside DocInitCallback
*/
C_DLLEXPORT void ASEXT_SetDefaultNamespace(CASDocumentation* pASDoc, const char* ns);
```

### asext_api.h
```cpp
// typedef + extern
typedef void(*fnASEXT_SetDefaultNamespace)(CASDocumentation* pASDoc, const char* ns);
extern fnASEXT_SetDefaultNamespace ASEXT_SetDefaultNamespace;

// In IMPORT_ASEXT_API macro
IMPORT_FUNCTION_DLSYM(asext, ASEXT_SetDefaultNamespace);\

// In IMPORT_ASEXT_API_DEFINE macro
IMPORT_FUNCTION_DEFINE(ASEXT_SetDefaultNamespace);\
```

## Platform Differences to Watch

| Aspect | Windows (server.dll) | Linux (server.so) |
|--------|---------------------|-------------------|
| Calling convention | `__thiscall` / `__fastcall` (ecx=this) | `cdecl` (this on stack) |
| Struct offsets | May differ (e.g., `this+0x24`) | May differ (e.g., `this+0x20`) |
| vtable offsets | Usually same across platforms | Usually same across platforms |
| Signature source | IDA Pro on server.dll | IDA Pro on server.so |
| Symbol availability | Never (use signatures only) | 5.15 has symbols; 5.16+ stripped |
| Stack cleanup | `ret N` for thiscall | Caller cleans |

## Checklist

- [ ] **IDA Analysis**: Decompile target function on both Windows and Linux
- [ ] **Signature Creation**: Create signatures from function bytes
- [ ] **Signature Verification**: Verify uniqueness with `find_bytes` in IDA (one match only)
- [ ] `asext/serverdef.h`: Add `typedef` + `PRIVATE_FUNCTION_EXTERN`
- [ ] `asext/signatures.h`: Add Windows signature (line 5–45 `#ifdef _WIN32` block)
- [ ] `asext/signatures.h`: Add Linux signature + symbol (line 46+ `#else` block)
- [ ] `asext/server_hook.cpp`: Add `PRIVATE_FUNCTION_DEFINE`
- [ ] `asext/server_hook.cpp`: Add `ASEXT_*` wrapper function with `C_DLLEXPORT`
- [ ] `asext/meta_api.cpp`: Add `FILL_FROM_SIGNATURE` in Windows branch
- [ ] `asext/meta_api.cpp`: Add `FILL_FROM_SIGNATURE` in Linux 5.16+ branch
- [ ] `asext/meta_api.cpp`: Add `FILL_FROM_SYMBOL` in Linux 5.15 branch
- [ ] `asext/meta_api.cpp`: Add `LOG_MESSAGE` after `#endif`
- [ ] `asext/asext.h`: Add `C_DLLEXPORT` declaration
- [ ] `asext/include/asext_api.h`: Add `typedef` + `extern` declaration
- [ ] `asext/include/asext_api.h`: Add to `IMPORT_ASEXT_API` macro
- [ ] `asext/include/asext_api.h`: Add to `IMPORT_ASEXT_API_DEFINE` macro
- [ ] **Build**: Compile on both Windows and Linux
- [ ] **Runtime**: Verify `LOG_MESSAGE` shows non-NULL address
- [ ] **Integration**: Test calling `ASEXT_*` from an external plugin

## References

- [asext-file-map.md](references/asext-file-map.md) - Detailed file roles and insertion points
- [import-macro-guide.md](references/import-macro-guide.md) - How the IMPORT macros work
- [../add-private-function-with-signatures/SKILL.md](../add-private-function-with-signatures/SKILL.md) - Base skill for IDA analysis and signatures
- [../add-private-function-with-signatures/references/signature-patterns.md](../add-private-function-with-signatures/references/signature-patterns.md) - Signature creation best practices
- [../add-private-function-with-signatures/references/calling-conventions.md](../add-private-function-with-signatures/references/calling-conventions.md) - Calling convention reference
