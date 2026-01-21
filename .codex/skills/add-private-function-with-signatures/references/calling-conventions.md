# Calling Conventions Guide

This document provides detailed information about calling conventions used in metamod-fallguys2 project.

## Overview

Calling conventions define how functions receive parameters and return values. This is critical for correctly hooking private functions from game engine DLLs.

## Available Calling Convention Macros

Defined in `asext/include/asext_api.h`:

### SC_SERVER_DECL

```cpp
#ifdef _WIN32
#define SC_SERVER_DECL __fastcall
#else
#define SC_SERVER_DECL
#endif
```

**Usage**: Class member functions
- Windows: `__fastcall` (first two integer/pointer arguments in ECX, EDX)
- Linux: Standard cdecl (all arguments on stack)

**Example**:
```cpp
typedef void(SC_SERVER_DECL *fnMemberFunction)(void* pthis, int arg1);
```

### SC_SERVER_CDECL

```cpp
#ifdef _WIN32
#define SC_SERVER_CDECL __cdecl
#else
#define SC_SERVER_CDECL
#endif
```

**Usage**: Standard C functions
- Windows: `__cdecl` (all arguments on stack, caller cleans)
- Linux: Standard cdecl

**Example**:
```cpp
typedef void(SC_SERVER_CDECL *fnCFunction)(int arg1, int arg2);
```

## Calling Convention Selection

### How to Identify Calling Convention in IDA Pro

1. **Check function signature** - IDA shows calling convention in decompiled code:
   ```cpp
   void __cdecl FunctionName(...)      // cdecl
   void __fastcall FunctionName(...)   // fastcall
   void __thiscall FunctionName(...)   // thiscall (class member)
   void __stdcall FunctionName(...)    // stdcall
   ```

2. **Check assembly prologue**:
   - **fastcall**: First two args in ECX/EDX
   ```asm
   mov ecx, [esp+4]    ; First arg to ECX
   mov edx, [esp+8]    ; Second arg to EDX
   ```

   - **cdecl/stdcall**: All args on stack
   ```asm
   push ebp
   mov ebp, esp
   mov eax, [ebp+8]    ; First arg from stack
   ```

3. **Check stack cleanup**:
   - **cdecl**: Caller cleans stack (`add esp, X` after call)
   - **stdcall/fastcall**: Callee cleans stack (`ret X`)

### Decision Tree

```
Is it a class member function?
├─ Yes → Use SC_SERVER_DECL
└─ No  → Is it a __cdecl function?
         ├─ Yes → Use SC_SERVER_CDECL
         └─ No  → Check IDA signature, may need new macro
```

## Adding New Calling Convention Macros

If you encounter a calling convention not covered by existing macros:

1. **Define macro in `asext/include/asext_api.h`**:
   ```cpp
   #ifdef _WIN32
   #define SC_SERVER_STDCALL __stdcall
   #else
   #define SC_SERVER_STDCALL
   #endif
   ```

2. **Common conventions**:
   - `__cdecl` - C calling convention
   - `__stdcall` - Callee cleans stack (Windows only)
   - `__thiscall` - Class member on Windows, ecx=this (use SC_SERVER_DECL for cross-platform)
   - `__fastcall` - `__thiscall` replacement for Class member on Windows, ecx=this, edx=dummy (use SC_SERVER_DECL for cross-platform)

## Platform-Specific Differences

### Windows

- **Multiple conventions**: cdecl, fastcall, stdcall, thiscall
- **Name mangling**: Decorated names (e.g., `?Function@@YAXH@Z`)
- **Register usage**: Varies by convention

### Linux

- **Single convention**: All effectively cdecl on x86
- **Name mangling**: C++ mangling (e.g., `_Z8FunctionNameParams`)
- **Register usage**: Consistent across functions

## Common Pitfalls

### ❌ Wrong: Using Windows-only convention on Linux

```cpp
typedef void(__cdecl *fnFunction)(int arg);  // Compile error on Linux
```

### ✅ Correct: Using cross-platform macro

```cpp
typedef void(SC_SERVER_CDECL *fnFunction)(int arg);  // Works on both
```

### ❌ Wrong: Mixing conventions

```cpp
// IDA shows __fastcall but using cdecl
typedef void(SC_SERVER_CDECL *fnMemberFunc)(void* pthis, int arg);
```

### ✅ Correct: Matching IDA convention

```cpp
typedef void(SC_SERVER_DECL *fnMemberFunc)(void* pthis, int arg);
```

## Real-World Examples

### Example 1: RegisterSCScriptColor24 (C function)

**IDA signature**:
```cpp
void __cdecl RegisterSCScriptColor24(CASDocumentation *a1)
```

**Definition**:
```cpp
typedef void(SC_SERVER_CDECL *fnRegisterSCScriptColor24)(CASDocumentation* pthis);
```

### Example 2: CPlayerMove::PlayStepSound (Member function)

**IDA signature**:
```cpp
void __fastcall CPlayerMove::PlayStepSound(void *pthis, int edx_unused, ...)
```

**Definition**:
```cpp
typedef void(SC_SERVER_DECL *fnCPlayerMove_PlayStepSound)(void* pthis, int arg1);
```

**Note**: fastcall's EDX parameter is often unused but must be declared.

## Testing Calling Conventions

After adding a function:

1. **Compile test**: Verify no calling convention errors
2. **Runtime test**: Check function executes without crashes
3. **Parameter test**: Verify parameters received correctly:
   ```cpp
   if (g_pfn_TestFunc)
   {
       LOG_CONSOLE(PLID, "Calling with param: %d", testValue);
       g_pfn_TestFunc(testValue);
       // Verify expected behavior
   }
   ```

## References

- MSVC Calling Conventions: https://learn.microsoft.com/en-us/cpp/cpp/calling-conventions
- System V ABI (Linux): https://wiki.osdev.org/System_V_ABI
- IDA Pro Documentation: Function types and calling conventions
