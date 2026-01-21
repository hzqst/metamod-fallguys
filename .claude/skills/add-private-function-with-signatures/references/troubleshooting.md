# Troubleshooting Guide

This document provides solutions to common problems encountered when adding private functions with signatures.

## Table of Contents

- [Compilation Errors](#compilation-errors)
- [Linking Errors](#linking-errors)
- [Runtime Errors](#runtime-errors)
- [Signature Scanning Failures](#signature-scanning-failures)
- [Debugging Techniques](#debugging-techniques)

## Compilation Errors

### Error: Expected ')' before '*' token

**Full error**:
```
fallguys/serverdef.h:123:45: error: expected ')' before '*' token
typedef void(__cdecl* fnFunctionName)(...);
                     ^
```

**Cause**: Using Windows-specific calling convention on Linux.

**Solution**: Use cross-platform macro instead of platform-specific keyword.

❌ Wrong:
```cpp
typedef void(__cdecl *fnFunction)(int arg);
typedef void(__fastcall *fnFunction)(int arg);
```

✅ Correct:
```cpp
typedef void(SC_SERVER_CDECL *fnFunction)(int arg);
typedef void(SC_SERVER_DECL *fnFunction)(int arg);
```

**If no appropriate macro exists**, add one to `asext/include/asext_api.h`:
```cpp
#ifdef _WIN32
#define SC_SERVER_STDCALL __stdcall
#else
#define SC_SERVER_STDCALL
#endif
```

---

### Error: Calling convention not supported

**Error**:
```
error: '__fastcall' calling convention is not supported for this target
```

**Cause**: Trying to use x86 calling conventions on non-x86 platforms.

**Solution**: Ensure macros are conditionally defined:
```cpp
#if defined(_WIN32) && defined(_M_IX86)
#define SC_SERVER_DECL __fastcall
#else
#define SC_SERVER_DECL
#endif
```

---

### Error: Macro redefinition

**Error**:
```
warning: 'FunctionName_Signature' macro redefined
```

**Cause**: Signature defined multiple times or in wrong location.

**Solution**:
1. Check `fallguys/signatures.h` for duplicates
2. Ensure signature is inside platform-specific `#ifdef`:
   ```cpp
   #ifdef _WIN32
   #define Func_Signature "..."
   #else
   #define Func_Signature "..."
   #endif
   ```

## Linking Errors

### Error: Unresolved external symbol

**Full error**:
```
error LNK2019: unresolved external symbol "void (__cdecl* g_pfn_FunctionName)"
```

**Cause**: Function pointer declared but not defined.

**Solution**: Add `PRIVATE_FUNCTION_DEFINE` to a `.cpp` file:

❌ Missing definition:
```cpp
// serverdef.h only
PRIVATE_FUNCTION_EXTERN(FunctionName);
```

✅ Add definition:
```cpp
// server_hook.cpp (or other .cpp)
PRIVATE_FUNCTION_DEFINE(FunctionName);
```

**Location**: Add to `fallguys/server_hook.cpp` alongside other definitions:
```cpp
PRIVATE_FUNCTION_DEFINE(CPlayerMove_PlayStepSound);
PRIVATE_FUNCTION_DEFINE(PM_PlaySoundFX_SERVER);
PRIVATE_FUNCTION_DEFINE(FunctionName);  // Add here
```

---

### Error: Multiple definition

**Error**:
```
error: multiple definition of 'g_pfn_FunctionName'
```

**Cause**: `PRIVATE_FUNCTION_DEFINE` used in multiple `.cpp` files or header.

**Solution**:
1. Use `PRIVATE_FUNCTION_DEFINE` in only ONE `.cpp` file
2. Use `PRIVATE_FUNCTION_EXTERN` in header files
3. Remove duplicate definitions:
   ```bash
   # Search for duplicates
   grep -r "PRIVATE_FUNCTION_DEFINE(FunctionName)" fallguys/
   ```

## Runtime Errors

### Crash: Access violation when calling function

**Symptom**: Game crashes when calling `g_pfn_FunctionName(...)`.

**Cause 1**: Function pointer is NULL (signature not found).

**Solution**: Always check pointer before calling:
```cpp
❌ Wrong:
g_pfn_FunctionName(arg1, arg2);  // Crashes if NULL

✅ Correct:
if (g_pfn_FunctionName)
{
    g_pfn_FunctionName(arg1, arg2);
}
else
{
    LOG_ERROR(PLID, "FunctionName not available");
}
```

**Cause 2**: Wrong calling convention.

**Solution**: Verify calling convention in IDA matches typedef:
```cpp
// IDA shows: void __fastcall Function(...)
// Must use:
typedef void(SC_SERVER_DECL *fnFunction)(...);
// NOT:
typedef void(SC_SERVER_CDECL *fnFunction)(...);
```

**Cause 3**: Wrong parameter types or count.

**Solution**: Match IDA signature exactly:
```cpp
// IDA: void __cdecl Function(int a1, float a2, const char *a3)
typedef void(SC_SERVER_CDECL *fnFunction)(int, float, const char*);
```

---

### Crash: Stack corruption after calling function

**Symptom**: Crash occurs after function returns, or local variables corrupted.

**Cause**: Calling convention mismatch causing stack imbalance.

**Diagnosis**:
```cpp
// Add logging before and after call
LOG_CONSOLE(PLID, "Before call, ESP: %p", &stack_var);
if (g_pfn_FunctionName)
    g_pfn_FunctionName(arg);
LOG_CONSOLE(PLID, "After call, ESP: %p", &stack_var);
```

**Solution**:
1. Verify calling convention in IDA
2. Check for fastcall hidden parameters:
   ```cpp
   // fastcall may have unused EDX parameter
   typedef void(SC_SERVER_DECL *fnFunction)(void* pthis, int edx_unused, int real_arg);
   ```

---

### Crash: Invalid parameters received

**Symptom**: Function executes but operates on wrong data.

**Cause**: Parameter types mismatched.

**Solution**: Match types exactly to IDA:
```cpp
// IDA shows: bool Function(void *a1, int a2)
❌ Wrong:
typedef int(SC_SERVER_CDECL *fnFunction)(void*, bool);

✅ Correct:
typedef bool(SC_SERVER_CDECL *fnFunction)(void*, int);
```

**Note**: Pay attention to:
- Pointer vs value types
- Signed vs unsigned
- int vs bool (different sizes)
- float vs double

## Signature Scanning Failures

### Function not found in logs

**Log message**:
```
[META] FunctionName not found
```

**Diagnosis steps**:

1. **Check module loaded**:
   ```cpp
   LOG_MESSAGE(PLID, "server.dll loaded: %p", g_pServer);
   ```

2. **Check signature syntax**:
   ```cpp
   // Verify escape sequences correct
   "\\x55\\x8B\\xEC"  // Correct
   "\x55\x8B\xEC"     // Wrong (will compile but fail)
   ```

3. **Test signature in IDA**:
   - Search → Sequence of bytes
   - Convert `\\x2A` to `?` for wildcards
   - Should find exactly one match

**Solutions**:

#### Solution 1: Signature changed between versions

Adjust signature to be more generic:
```cpp
// Too specific - may fail in other versions
"\\x83\\xEC\\x2C\\xA1\\x12\\x34\\x56\\x78"

// More generic - wildcards for addresses
"\\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A"
```

#### Solution 2: Signature too short

Lengthen signature:
```cpp
// Too short - matches multiple functions
"\\x55\\x8B\\xEC"  // 3 bytes

// Better - unique
"\\x55\\x8B\\xEC\\x83\\xEC\\x20\\x6A\\x0A\\x68\\x2A\\x2A\\x2A\\x2A"  // 14 bytes
```

#### Solution 3: Use caller signature

If function signature is too generic:
```cpp
// Find unique call site instead
FILL_FROM_SIGNATURED_CALLER_FROM_END(server, FunctionName, -1);
```

#### Solution 4: Linux symbol not found

For Sven Co-op 5.15, verify symbol name:
```cpp
// Get correct mangled name from `nm` or `objdump`
#define FunctionName_Symbol "_Z23CorrectMangledName"
```

Check symbol with:
```bash
nm -D server.so | grep FunctionName
objdump -T server.so | grep FunctionName
```

---

### Function found but at wrong address

**Symptom**: Log shows function found but calling crashes.

**Cause**: Signature matches wrong function.

**Diagnosis**:
1. Check IDA for multiple matches:
   ```
   Search → Sequence of bytes → Count results
   ```

2. Log found address:
   ```cpp
   if (g_pfn_FunctionName)
       LOG_CONSOLE(PLID, "Found at: 0x%p", g_pfn_FunctionName);
   ```

3. Compare address in IDA

**Solution**: Make signature more specific:
```cpp
// Add more distinctive bytes
// Include characteristic constants
// Extend length to 25-30 bytes
```

---

### Platform-specific signature issues

#### Windows: Security cookie variations

Different MSVC versions use different security cookie code.

**Solution**: Wildcard security cookie checks:
```cpp
"\\xA1\\x2A\\x2A\\x2A\\x2A"  // mov eax, [security_cookie]
"\\x33\\xC4"                  // xor eax, esp
```

#### Linux: PIC code variations

GOT offset changes between builds.

**Solution**: Wildcard PIC initialization:
```cpp
"\\xE8\\x2A\\x2A\\x2A\\x2A"        // call __x86.get_pc_thunk
"\\x81\\xC3\\x2A\\x2A\\x2A\\x2A"   // add ebx, GOT_offset
```

## Debugging Techniques

### Enable verbose logging

Add logging throughout the process:

```cpp
// In meta_api.cpp Meta_Attach
LOG_MESSAGE(PLID, "=== Starting function pointer filling ===");

#ifdef _WIN32
LOG_MESSAGE(PLID, "Platform: Windows");
#else
LOG_MESSAGE(PLID, "Platform: Linux");
#endif

LOG_MESSAGE(PLID, "Searching for FunctionName...");
FILL_FROM_SIGNATURE(server, FunctionName);

if (g_pfn_FunctionName)
    LOG_MESSAGE(PLID, "✓ FunctionName found at 0x%p", g_pfn_FunctionName);
else
    LOG_ERROR(PLID, "✗ FunctionName NOT FOUND");

LOG_MESSAGE(PLID, "=== Function pointer filling complete ===");
```

---

### Verify function execution

Add entry/exit logging:

```cpp
// Wrapper for testing
void Test_FunctionName(int arg1)
{
    LOG_CONSOLE(PLID, ">>> Calling FunctionName with arg: %d", arg1);

    if (!g_pfn_FunctionName)
    {
        LOG_ERROR(PLID, "Function pointer is NULL!");
        return;
    }

    // Call original
    g_pfn_FunctionName(arg1);

    LOG_CONSOLE(PLID, "<<< FunctionName returned successfully");
}
```

---

### Dump signature bytes

Compare actual memory with expected signature:

```cpp
// Get function address
void* func_addr = (void*)g_pfn_FunctionName;

// Dump first 32 bytes
LOG_CONSOLE(PLID, "Function bytes:");
unsigned char* bytes = (unsigned char*)func_addr;
for (int i = 0; i < 32; i++)
{
    if (i % 16 == 0) LOG_CONSOLE(PLID, "");
    LOG_CONSOLE(PLID, "%02X ", bytes[i]);
}
```

Compare output with IDA bytes to verify correctness.

---

### Use IDA Pro MCP for verification

```python
# Decompile to verify signature
mcp__ida-pro-mcp__decompile("0xAddress")

# Get bytes for signature
mcp__ida-pro-mcp__get_bytes({"addr": "0xAddress", "size": 50})

# Search signature
mcp__ida-pro-mcp__find_bytes("55 8B EC 83 EC 20")
```

---

### Test with minimal call

Create isolated test:

```cpp
// In Meta_Attach or test command
void TestFunctionHook()
{
    LOG_CONSOLE(PLID, "=== Testing FunctionName ===");

    // Check pointer
    if (!g_pfn_FunctionName)
    {
        LOG_ERROR(PLID, "Function not available");
        return;
    }

    // Try simple call with known-safe parameters
    try
    {
        g_pfn_FunctionName(/* safe test params */);
        LOG_CONSOLE(PLID, "✓ Test call succeeded");
    }
    catch (...)
    {
        LOG_ERROR(PLID, "✗ Test call crashed");
    }
}
```

---

### Check macro expansion

Verify macros expand correctly:

```cpp
// In serverdef.h
PRIVATE_FUNCTION_EXTERN(TestFunction);

// Preprocessor should expand to:
extern fnTestFunction g_pfn_TestFunction;
extern fnTestFunction g_call_original_TestFunction;

// In server_hook.cpp
PRIVATE_FUNCTION_DEFINE(TestFunction);

// Should expand to:
fnTestFunction g_pfn_TestFunction = nullptr;
fnTestFunction g_call_original_TestFunction = nullptr;
```

Use compiler `-E` flag to check preprocessor output:
```bash
g++ -E fallguys/serverdef.h | grep TestFunction
```

## Prevention Checklist

Before committing new function:

- [ ] Tested on Windows build
- [ ] Tested on Linux build
- [ ] Signature verified in IDA (only one match)
- [ ] Calling convention matches IDA
- [ ] Parameter types match IDA exactly
- [ ] Function pointer null-checked before calls
- [ ] Logs confirm function found at runtime
- [ ] Test call executes without crash
- [ ] Added to all three fill locations (Win, Linux 5.16, Linux 5.15)

## Quick Reference: Common Issues

| Symptom | Likely Cause | Quick Fix |
|---------|--------------|-----------|
| Compile error: calling convention | Platform-specific keyword | Use SC_SERVER_* macro |
| Link error: unresolved symbol | Missing PRIVATE_FUNCTION_DEFINE | Add to .cpp file |
| Link error: multiple definition | DEFINE in multiple files | Keep only one DEFINE |
| Runtime: crash on call | NULL pointer | Add null check |
| Runtime: stack corruption | Wrong calling convention | Match IDA convention |
| Not found in logs | Bad signature | Verify in IDA |
| Found wrong function | Signature not unique | Make more specific |
| Linux 5.15 not found | Wrong symbol name | Check with nm/objdump |

## Getting Help

If issues persist:

1. **Check logs** - Look for "not found" messages
2. **Verify in IDA** - Confirm signature matches
3. **Test on both platforms** - Windows and Linux
4. **Simplify** - Test with minimal parameters first
5. **Compare** - Look at working examples (e.g., RegisterSCScriptColor24)

## Useful Commands

```bash
# Search for signature definitions
grep -r "FunctionName_Signature" fallguys/

# Search for function defines
grep -r "PRIVATE_FUNCTION_DEFINE(FunctionName)" fallguys/

# Check Linux symbols
nm -D server.so | grep FunctionName

# Check calling conventions used
grep -r "SC_SERVER_" asext/include/

# View preprocessor expansion
g++ -E fallguys/serverdef.h | grep -A5 FunctionName
```
