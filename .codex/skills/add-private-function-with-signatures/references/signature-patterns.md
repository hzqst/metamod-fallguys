# Signature Creation Patterns

This document covers best practices and patterns for creating function signatures for signature scanning.

## Table of Contents

- [Signature Basics](#signature-basics)
- [Windows Signatures](#windows-signatures)
- [Linux Signatures](#linux-signatures)
- [Wildcards and Masking](#wildcards-and-masking)
- [Signature Selection Strategies](#signature-selection-strategies)
- [Verification Process](#verification-process)
- [Real-World Examples](#real-world-examples)

## Signature Basics

### What is a Signature?

A signature is a sequence of bytes that uniquely identifies a function in memory. It's used to locate functions that don't have exported symbols.

### Signature Format

```cpp
#define FunctionName_Signature "\xBYTE1\xBYTE2\x2A..."
```

- `\xHH` - Hex byte (e.g., `\x55` = 0x55)
- `\x2A` - Wildcard (matches any byte)

### Signature Components

Typical function signature includes:
1. **Function prologue** - Setup code (push ebp, mov esp, sub esp)
2. **Initial instructions** - First operations after prologue
3. **Characteristic constants** - Unique values (magic numbers, sizes)

## Windows Signatures

### Typical x86 Windows Prologue Patterns

**Standard frame setup**:
```
55              push ebp
8B EC           mov ebp, esp
83 EC XX        sub esp, XXh        ; Stack frame allocation
```

**Security cookie check**:
```
83 EC 2C        sub esp, 2Ch
A1 XX XX XX XX  mov eax, [___security_cookie]
33 C4           xor eax, esp
89 44 24 XX     mov [esp+XXh], eax
```

**Register preservation**:
```
56              push esi
57              push edi
53              push ebx
```

### Windows Signature Selection

**Prefer (in order)**:
1. Prologue + first unique instructions (20-30 bytes)
2. Prologue + characteristic constants
3. Unique instruction sequences

**Avoid**:
- Absolute addresses (use wildcards)
- Stack offsets (may vary)
- Compiler-generated code that changes between builds

### Windows Example

```cpp
// RegisterSCScriptColor24 Windows signature
#define RegisterSCScriptColor24_Signature "\x83\xEC\x2C\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x28\x56\x8B\x74\x24\x30\x8B\xCE\x6A\x02\x6A\x03\x68
```

**Rationale**:
- Unique prologue with security cookie
- Characteristic constants (2, 3)
- 25 bytes - good length for uniqueness

## Linux Signatures

### Typical x86 Linux Prologue Patterns

**Standard frame setup**:
```
55              push ebp
89 E5           mov ebp, esp
53              push ebx
83 EC XX        sub esp, XXh
```

**PIC (Position Independent Code)**:
```
E8 00 00 00 00     call __x86.get_pc_thunk.bx
81 C3 XX XX XX XX  add ebx, offset _GLOBAL_OFFSET_TABLE_
```

**Modern prologue**:
```
55              push ebp
57              push edi
56              push esi
53              push ebx
83 EC XX        sub esp, XXh
```

### Linux Signature Selection

**Must consider**:
1. **PIC code** - Use wildcards for GOT offsets
2. **Symbol availability** - Always provide symbol fallback
3. **Sven Co-op versions**:
   - 5.16+ uses SCServerDLL003 (no symbols) → Need signature
   - 5.15 has symbols → Use symbol name

**Pattern**:
```cpp
#else  // Linux

#define FunctionName_Signature "\x55\x57\x56\x53..."
#define FunctionName_Symbol "_Z23MangledFunctionName"

#endif
```

### Linux Example

```cpp
// RegisterSCScriptColor24 Linux signature
#define RegisterSCScriptColor24_Signature "\
\x55\x57\x56\x53\      // push ebp; push edi; push esi; push ebx
\x83\xEC\x5C\          // sub esp, 5Ch
\xE8\x2A\x2A\x2A\x2A\  // call __x86.get_pc_thunk (address wildcarded)
\x81\xC3\x2A\x2A\x2A\x2A\  // add ebx, _GLOBAL_OFFSET_TABLE_ (offset wildcarded)
\x8B\x74\x24\x70\      // mov esi, [esp+70h]
\xC7\x44\x24\x10\x02\x00\x00\x00\  // mov [esp+10h], 2
\xC7\x44\x24\x0C\x03" // mov [esp+0Ch], 3

// Symbol for Sven Co-op 5.15
#define RegisterSCScriptColor24_Symbol "_Z23RegisterSCScriptColor24P16CASDocumentation"
```

**Rationale**:
- Standard prologue with register saves
- PIC initialization (wildcarded)
- Characteristic constants (2, 3)
- Includes symbol for 5.15 fallback

## Wildcards and Masking

### When to Use Wildcards (`\x2A`)

**Always wildcard**:
1. **Absolute addresses**:
   ```
   A1 XX XX XX XX  → \xA1\x2A\x2A\x2A\x2A
   ```

2. **Offsets/displacements**:
   ```
   81 C3 XX XX XX XX  → \x81\xC3\x2A\x2A\x2A\x2A
   ```

3. **Variable stack offsets**:
   ```
   89 44 24 XX  → \x89\x44\x24\x2A  (if offset varies)
   ```

**Keep specific**:
1. **Opcodes** - Always keep
2. **Constants** - Unique values (magic numbers)
3. **Small immediate values** - Loop counters, sizes

### Wildcard Strategy

**Too many wildcards** → May match multiple functions
**Too few wildcards** → May fail across game versions

**Balance**:
- Start with 20-30 byte signature
- Wildcard only addresses and variable offsets
- Keep characteristic constants
- Test across game versions if possible

## Signature Selection Strategies

### Strategy 1: Prologue + Constants

Best for functions with unique parameter values:

```cpp
// Function that uses specific constants
"\x55\x8B\xEC\x83\xEC\x20"  // Prologue
"\x6A\x0A"                       // push 10 (unique value)
"\x6A\x05"                       // push 5 (unique value)
```

### Strategy 2: Prologue + Unique Instructions

Best for functions with distinctive operations:

```cpp
// Function with uncommon instruction sequence
"\x55\x8B\xEC"                 // Prologue
"\x0F\xB6\x45\x08"            // movzx eax, byte ptr [ebp+8]
"\x33\xD2"                       // xor edx, edx
"\xF7\xF1"                       // div ecx (distinctive)
```

### Strategy 3: Middle-of-Function Patterns

When prologue is too generic:

```cpp
// Find unique middle section
"\x8D\x44\x24\x10"            // lea eax, [esp+10h]
"\x50"                            // push eax
"\xE8\x2A\x2A\x2A\x2A"       // call (address wildcarded)
"\x85\xC0"                       // test eax, eax
"\x74\x15"                       // jz short +15h (specific jump)
```

### Strategy 4: Caller Signature

When function itself is too generic:

Use `FILL_FROM_SIGNATURED_CALLER_FROM_END`:
1. Find unique call site
2. Extract call instruction + context
3. Use offset to locate actual function

## Verification Process

**Critical**: Always verify signature uniqueness before committing.

### Step 1: Search in IDA Pro

1. **Open Search → Sequence of bytes**
2. **Paste signature** (without `\x` prefix):
   ```
   83 EC 2C A1 2A 2A 2A 2A 33 C4
   ```
3. **Use wildcards** - Replace `2A` with `?` in IDA search:
   ```
   83 EC 2C A1 ? ? ? ? 33 C4
   ```

### Step 2: Check Results

**✅ Success**: Only one match (your target function)
**❌ Failure**: Multiple matches

### Step 3: Adjust if Needed

**If multiple matches**:
1. Lengthen signature (include more bytes)
2. Add characteristic constants
3. Use more specific instruction sequence

**If no matches**:
1. Check byte accuracy
2. Verify wildcard placement
3. Consider using caller signature

## Real-World Examples

### Example 1: Simple Function with Constants

```cpp
// PM_PlaySoundFX_SERVER
// Characteristic: Uses specific sound channel values

Windows:
#define PM_PlaySoundFX_SERVER_Signature "\
\x55\x8B\xEC\        // Standard prologue
\x83\xEC\x18\        // sub esp, 18h
\x8B\x45\x10\        // mov eax, [ebp+10h]
\x8D\x4D\xE8\        // lea ecx, [ebp-18h]
\x89\x4D\xF0\        // mov [ebp-10h], ecx
\x6A\x00\             // push 0 (characteristic)
\x6A\x00"             // push 0 (characteristic)

Linux:
#define PM_PlaySoundFX_SERVER_Signature "\
\x55\x89\xE5\        // push ebp; mov ebp, esp
\x83\xEC\x38\        // sub esp, 38h
\xC7\x44\x24\x0C\x00\x00\x00\x00\  // mov [esp+0Ch], 0
\xC7\x44\x24\x08\x00\x00\x00\x00"  // mov [esp+8], 0
```

### Example 2: Security Cookie Pattern

```cpp
// Modern MSVC with security cookie

#define SecureFunctionSignature "\
\x83\xEC\x40\                    // sub esp, 40h
\xA1\x2A\x2A\x2A\x2A\          // mov eax, [__security_cookie]
\x33\xC4\                          // xor eax, esp
\x89\x44\x24\x3C\                // mov [esp+3Ch], eax
\x56\                               // push esi
\x8B\x74\x24\x48"                // mov esi, [esp+48h]
```

### Example 3: PIC Code on Linux

```cpp
// Position Independent Code with GOT access

#define PICFunctionSignature "\
\x55\x57\x56\x53\                // push ebp, edi, esi, ebx
\x83\xEC\x2C\                     // sub esp, 2Ch
\xE8\x2A\x2A\x2A\x2A\           // call __x86.get_pc_thunk (wildcard)
\x81\xC3\x2A\x2A\x2A\x2A\      // add ebx, GOT_offset (wildcard)
\x8B\x44\x24\x40"                // mov eax, [esp+40h]
```

### Example 4: Caller-Based Signature

```cpp
// When function prologue is generic, use caller

// Step 1: Find unique caller
// Step 2: Get call site signature
#define CallerSignature "\
\x8B\x45\x08\        // mov eax, [ebp+8]
\x50\                   // push eax
\xE8\x2A\x2A\x2A\x2A\  // call TargetFunction (get offset)
\x83\xC4\x04\        // add esp, 4
\x85\xC0"             // test eax, eax

// In meta_api.cpp:
FILL_FROM_SIGNATURED_CALLER_FROM_END(server, TargetFunction, -1);
```

## Best Practices Summary

1. **Length**: 20-30 bytes optimal
2. **Start**: Include function prologue
3. **Unique elements**: Constants, distinctive instructions
4. **Wildcards**: Only for addresses and variable offsets
5. **Verification**: Always search in IDA before committing
6. **Platform differences**: Account for Windows vs Linux patterns
7. **Symbols**: Always provide Linux symbol for 5.15 fallback
8. **Documentation**: Comment signatures with their structure

## Tools and Techniques

### IDA Pro Tips

1. **Copy bytes**: Right-click instruction → Copy → Bytes (space-separated)
2. **Search pattern**: Edit → Search → Sequence of bytes
3. **Wildcard search**: Use `?` for wildcards in IDA
4. **Verify context**: Check surrounding functions to ensure uniqueness

### Signature Formatting

```cpp
// Good: Readable with comments
#define Func_Signature "\
\x55\x8B\xEC\        /* push ebp; mov ebp, esp */\
\x83\xEC\x20\        /* sub esp, 20h */\
\x6A\x0A"             /* push 10 */

// Acceptable: Compact
#define Func_Signature "\x55\x8B\xEC\x83\xEC\x20\x6A\x0A"

// Avoid: Hard to maintain
#define Func_Signature "U\x8B\xEC\x83\xEC \x6A\n"  // Mixed formats
```

## Common Mistakes

### ❌ Too Short

```cpp
#define Func_Signature "\x55\x8B\xEC"  // Just prologue - matches many functions
```

### ❌ Too Long

```cpp
#define Func_Signature "\x55\x8B\xEC...\x5D\xC3"  // 200 bytes - may fail across versions
```

### ❌ No Wildcards

```cpp
// Hardcoded address will fail in different builds
#define Func_Signature "\xA1\x10\x20\x30\x40"
```

### ✅ Correct Balance

```cpp
#define Func_Signature "\
\x55\x8B\xEC\                  // Prologue
\xA1\x2A\x2A\x2A\x2A\        // Address wildcarded
\x6A\x0A\                        // Unique constant
\x50"                             // 7 instructions, ~20 bytes
```

## Testing Signatures

After creating signatures:

1. **Compile test**: Ensure syntax correct
2. **Load test**: Check logs for "found" vs "not found"
3. **Version test**: Test across game versions if possible
4. **Call test**: Verify function executes correctly

```cpp
// Add logging in Meta_Attach
LOG_MESSAGE(PLID, "Searching for FunctionName...");
FILL_FROM_SIGNATURE(server, FunctionName);
if (g_pfn_FunctionName)
    LOG_MESSAGE(PLID, "FunctionName found at 0x%p", g_pfn_FunctionName);
else
    LOG_ERROR(PLID, "FunctionName not found!");
```
