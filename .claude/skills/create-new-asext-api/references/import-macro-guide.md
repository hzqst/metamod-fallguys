# Import Macro Guide

How the `IMPORT_ASEXT_API` and `IMPORT_ASEXT_API_DEFINE` macros work, and how external plugins use them to dynamically load asext functions.

## Overview

External plugins (e.g., fallguys, ascurl, asqcvar) don't link directly against asext. Instead, they use a **dynamic import** pattern:

1. `IMPORT_ASEXT_API_DEFINE()` — declares function pointer variables
2. `IMPORT_ASEXT_API(asext_handle)` — fills them at runtime via `dlsym` / `GetProcAddress`

## Macro Definitions

### IMPORT_FUNCTION_DEFINE

```cpp
// Expands to a global function pointer variable initialized to NULL
#define IMPORT_FUNCTION_DEFINE(name) \
    decltype(&name) name = nullptr
```

Example:
```cpp
IMPORT_FUNCTION_DEFINE(ASEXT_SetDefaultNamespace);
// Expands to:
fnASEXT_SetDefaultNamespace ASEXT_SetDefaultNamespace = nullptr;
```

### IMPORT_FUNCTION_DLSYM

```cpp
// Looks up the function by name in the shared library and assigns the pointer
#define IMPORT_FUNCTION_DLSYM(handle, name) \
    name = (decltype(&name))gpMetaUtilFuncs->pfnGetProcAddress(handle, #name)
```

Example:
```cpp
IMPORT_FUNCTION_DLSYM(asext, ASEXT_SetDefaultNamespace);
// Expands to:
ASEXT_SetDefaultNamespace = (fnASEXT_SetDefaultNamespace)
    gpMetaUtilFuncs->pfnGetProcAddress(asext, "ASEXT_SetDefaultNamespace");
```

### IMPORT_FUNCTION_POINTER_DEFINE / IMPORT_FUNCTION_POINTER_DLSYM

For function **pointers to function pointers** (like `ASEXT_CallHook` which is `fnASEXT_CallHook *`):

```cpp
IMPORT_FUNCTION_POINTER_DEFINE(ASEXT_CallHook);
IMPORT_FUNCTION_POINTER_DLSYM(asext, ASEXT_CallHook);
```

Most ASEXT functions use the regular (non-pointer) variants.

## Usage in External Plugins

### Step 1: Include the header

```cpp
#include "asext_api.h"
```

### Step 2: Define function pointers (once, globally)

```cpp
// In a .cpp file (typically meta_api.cpp of the external plugin)
IMPORT_ASEXT_API_DEFINE();
```

### Step 3: Load at runtime (in Meta_Attach)

```cpp
auto asext = LOAD_PLUGIN("asext");
if (asext) {
    IMPORT_ASEXT_API(asext);
}
```

### Step 4: Call the function

```cpp
void MyDocInitCallback(CASDocumentation *pASDoc) {
    // Always check for NULL — the function may not exist in older asext versions
    if (ASEXT_SetDefaultNamespace) {
        ASEXT_SetDefaultNamespace(pASDoc, "MyPlugin");
    }
}
```

## Adding a New Function to the Macros

When adding `ASEXT_NewFunction`:

### In `IMPORT_ASEXT_API` macro:

Find the line continuation chain and insert:
```cpp
#define IMPORT_ASEXT_API(asext) IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallHook);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CallCASBaseCallable);\
// ... existing entries ...
IMPORT_FUNCTION_DLSYM(asext, ASEXT_RegisterEnumValue);\
IMPORT_FUNCTION_DLSYM(asext, ASEXT_NewFunction);\           // ← INSERT
IMPORT_FUNCTION_DLSYM(asext, ASEXT_CreateDirectory);\
// ... rest ...
```

### In `IMPORT_ASEXT_API_DEFINE` macro:

Same pattern:
```cpp
#define IMPORT_ASEXT_API_DEFINE() IMPORT_FUNCTION_DEFINE(ASEXT_RegisterDocInitCallback);\
// ... existing entries ...
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterEnumValue);\
IMPORT_FUNCTION_DEFINE(ASEXT_NewFunction);\                  // ← INSERT
IMPORT_FUNCTION_DEFINE(ASEXT_RegisterHook);\
// ... rest ...
```

## Important Notes

1. **Line continuations**: Every line except the last must end with `;\` (semicolon + backslash). No trailing whitespace after `\`.

2. **Ordering**: Keep consistent ordering between `IMPORT_ASEXT_API` and `IMPORT_ASEXT_API_DEFINE`. Group related functions together.

3. **Backward compatibility**: External plugins compiled against an older asext_api.h that doesn't include the new function will still work — they simply won't import it. The function pointer remains NULL.

4. **NULL safety**: External plugins must always check `if (ASEXT_NewFunction)` before calling, because:
   - The asext version may predate the function
   - The signature scan may have failed at runtime
   - The asext plugin may not be loaded

5. **typedef consistency**: The typedef in asext_api.h uses **plain function pointer** syntax (no `SC_SERVER_DECL`), because external plugins call through the wrapper which handles calling convention internally:
   ```cpp
   // asext_api.h — external-facing, simple signature
   typedef void(*fnASEXT_SetDefaultNamespace)(CASDocumentation* pASDoc, const char* ns);

   // serverdef.h — internal, platform-specific calling convention
   typedef void(SC_SERVER_DECL *fnCASDocumentation_SetDefaultNamespace)(
       CASDocumentation* pthis, SC_SERVER_DUMMYARG const char* ns);
   ```
