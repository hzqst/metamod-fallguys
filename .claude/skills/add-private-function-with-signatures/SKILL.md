---
name: add-private-function-with-signatures
description: Add private functions from game DLLs (server.dll/engine.dll) to metamod plugins using signature scanning and symbol lookup. Use when adding new private function hooks that require IDA Pro analysis, signature creation, cross-platform support (Windows/Linux), and runtime function pointer filling.
---

# Add Private Function With Signatures

## Overview

This skill guides the process of adding private functions from game engine DLLs to metamod plugins. It covers IDA Pro analysis, signature creation, function type definition, and runtime hooking for both Windows and Linux platforms.

## Workflow

### Step 1: Analyze Function with IDA Pro

Use IDA Pro MCP tools to analyze the target function:

```bash
# Find the function
mcp__ida-pro-mcp__lookup_funcs("FunctionName")

# Get decompiled code
mcp__ida-pro-mcp__decompile("0xAddress")

# Get disassembly
mcp__ida-pro-mcp__disasm("0xAddress", max_instructions=30)

# Extract signature bytes
mcp__ida-pro-mcp__get_bytes({"addr": "0xAddress", "size": 50})

# Verify signature uniqueness (CRITICAL STEP!)
# Convert bytes to IDA search format (use signature_converter.py script)
python scripts/signature_converter.py spaced "\\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A"
# Output: 83 EC 2C A1 ?? ?? ?? ??

# Search in IDA Pro with converted pattern
mcp__ida-pro-mcp__find_bytes("83 EC 2C A1 ?? ?? ?? ?? 33 C4")
# Result should show ONLY ONE match - your target function
# If multiple matches found, extend signature or use more specific bytes
```

**Critical**: Always verify signature uniqueness using `find_bytes`. If the pattern matches multiple functions, the signature is too generic and must be made more specific.

**Wildcard rules** (`\x2A` = any byte, shown as `??` in IDA format):
- **Always wildcard**: absolute addresses, GOT offsets, relative call/jump offsets (`E8`/`E9` operands), variable stack offsets (`[ebp-XX]`)
- **Keep literal**: opcodes, vtable offsets (e.g. `FF 90 B4 00 00 00`), characteristic constants, struct member offsets, string instruction prefixes

**Tip**: Use `scripts/signature_converter.py` to convert between formats:
```bash
# Convert C string to IDA format
python scripts/signature_converter.py spaced "\\x83\\xEC\\x2C"

# Convert IDA format to C string
python scripts/signature_converter.py c_string "83 EC 2C"

# Show all formats
python scripts/signature_converter.py all "83EC2C"
```

### Step 2: Define Function Type

Edit `fallguys/serverdef.h`:

```cpp
// Function signature from IDA: ReturnType __callingconv FunctionName(params)
typedef ReturnType(CALLING_CONVENTION *fnFunctionName)(ParamTypes);
PRIVATE_FUNCTION_EXTERN(FunctionName);
```

**Calling convention mapping**:
- `SC_SERVER_DECL` - fastcall (Windows) / cdecl (Linux), for class methods
- `SC_SERVER_CDECL` - cdecl (both platforms), for C functions
- Default - cdecl

For new calling conventions, see [calling-conventions.md](references/calling-conventions.md).

### Step 3: Add Function Signatures

Edit `fallguys/signatures.h`:

**Windows signature**:
```cpp
#ifdef _WIN32
#define FunctionName_Signature "\\x83\\xEC\\x2C\\xA1\\x2A\\x2A\\x2A\\x2A..."
```

**Linux signature and symbol**:
```cpp
#else
#define FunctionName_Signature "\\x55\\x57\\x56\\x53\\x83\\xEC\\x5C..."
#define FunctionName_Symbol "_Z23FunctionNameMangledName"
```

See [signature-patterns.md](references/signature-patterns.md) for signature creation guidelines.

### Step 4: Define Global Function Pointer

Edit `fallguys/server_hook.cpp` (or relevant .cpp file):

```cpp
PRIVATE_FUNCTION_DEFINE(ExistingFunction1);
PRIVATE_FUNCTION_DEFINE(ExistingFunction2);
PRIVATE_FUNCTION_DEFINE(FunctionName);  // Add new line
```

This expands to:
```cpp
fnFunctionName g_pfn_FunctionName;
fnFunctionName g_call_original_FunctionName;
```

### Step 5: Fill Function Pointer in Meta_Attach

Edit `fallguys/meta_api.cpp`:

**Windows branch**:
```cpp
#ifdef _WIN32
FILL_FROM_SIGNATURE(server, FunctionName);
```

**Linux Sven Co-op 5.16+ branch**:
```cpp
if (CreateInterface("SCServerDLL003", nullptr) != nullptr)
{
    FILL_FROM_SIGNATURE(server, FunctionName);
}
```

**Linux Sven Co-op 5.15 branch**:
```cpp
else
{
    FILL_FROM_SYMBOL(server, FunctionName);
}
```

**Fill macros**:
- `FILL_FROM_SIGNATURE(module, name)` - Direct signature scan on target function
- `FILL_FROM_SYMBOL(module, name)` - Lookup by symbol name (Linux only)
- `FILL_FROM_SIGNATURED_CALLER_FROM_START(module, name, offset)` - Locate via caller signature + offset from **start** of match
- `FILL_FROM_SIGNATURED_CALLER_FROM_END(module, name, offset)` - Locate via caller signature + offset from **end** of match
- `FILL_FROM_SIGNATURED_TY_CALLER_FROM_START(module, name, ty, offset)` - Same as above, uses `name_Signature_ty`
- `FILL_FROM_SIGNATURED_TY_CALLER_FROM_END(module, name, ty, offset)` - Same as above, uses `name_Signature_ty`

**Module options**: `server` (server.dll/so), `engine` (hw.dll/so)

### Step 5b: Caller-Based Signature Approach (for short/inlined functions)

When the target function is too short or simple to produce a unique signature (e.g. small methods, trivial wrappers, iterator operators), use a **caller-based** approach: find a unique byte pattern inside a known caller of the target, and use an offset to reach the `E8` (call) instruction.

#### When to Use

- Target function body is very short (< 15 bytes)
- Target function signature is not unique across the binary
- Multiple short related functions can all be located from a single well-known caller

#### How It Works

1. `LOCATE_FROM_SIGNATURE` finds the byte pattern (stored in `name_Signature`) inside the caller function
2. An offset is added to reach the `E8` call instruction:
   - **FROM_START**: `effective_addr = signature_match + offset`
   - **FROM_END**: `effective_addr = signature_match + strlen(signature) + offset`
3. `pfnGetNextCallAddr(effective_addr, 1)` reads the `E8` relative call at that address and returns the call target

**Critical**: `pfnGetNextCallAddr(addr, 1)` checks **exactly one byte position**. If the byte at `addr` is not `E8` (relative call) or `FF 15` (indirect call), it returns `NULL`. The offset **must** point exactly at the `E8` byte.

#### IDA Pro Workflow

```bash
# 1. Find the caller function
mcp__ida-pro-mcp__lookup_funcs("CallerFunction")

# 2. Disassemble to see all call instructions
mcp__ida-pro-mcp__disasm("0xCallerAddr")

# 3. Identify the call to target, note its address (the E8 byte)
#    e.g. 0x102dd875: E8 XX XX XX XX  ; call TargetFunction

# 4. Get raw bytes around the call instruction
mcp__ida-pro-mcp__get_bytes([{"addr": "0x102dd865", "size": 30}])

# 5. Craft a unique byte pattern near the E8, wildcard variable parts
#    Pattern: 8B 40 30 FF D0 89 45 ?? 8B ?? 8D 45 ?? 50
#    The E8 is at offset 14 from the pattern start

# 6. Verify uniqueness - MUST be exactly 1 match
mcp__ida-pro-mcp__find_bytes(["8B 40 30 FF D0 89 45 ?? 8B ?? 8D 45 ?? 50"])

# 7. Calculate offset = E8_address - signature_match_address
#    e.g. 0x102dd875 - 0x102dd867 = 14
```

#### Signature Design Guidelines

- **Offset must be < 15** for robustness. If the E8 is far from the unique pattern, choose a different pattern closer to the call.
- **Wildcard** (`\x2A`) stack offsets (e.g. `[ebp-XXh]`), register choices, and relative call addresses — these vary between builds.
- **Keep literal** opcodes, vtable offsets, constant values, and structural patterns (e.g. `FF D0` = indirect call, `0F 84` = jz near).
- When multiple target functions share one caller, create **individual** unique signatures near each call site. Don't reuse one signature with large offsets.
- Distinguish similar patterns by including **context after the call** (e.g. `0F 84` vs `0F 85` for jz vs jnz).

#### Example: Locating iterator functions from a caller

Six CScriptDictionary iterator functions (begin, end, operator!=, GetValue, GetKey, operator++) are all called within `CASEntityFuncs::InitializeEntity`. Each is too short for a direct signature, so each gets a unique byte pattern near its call site:

```cpp
// signatures.h - each pattern is unique within the binary
#define CScriptDictionary_begin_Signature      "\x8B\x40\x30\xFF\xD0\x89\x45\x2A\x8B\x2A\x8D\x45\x2A\x50"
#define CScriptDictionary_end_Signature        "\x8D\x45\x2A\x8B\x2A\x50\xE8\x2A\x2A\x2A\x2A\x50\x8D\x4D\x2A\xE8\x2A\x2A\x2A\x2A\x84\xC0\x0F\x84"
#define CScriptDictionary_CIterator_operator_NE_Signature "\x50\x8D\x4D\x2A\xE8\x2A\x2A\x2A\x2A\x84\xC0\x0F\x84"
```

```cpp
// meta_api.cpp - offset points exactly at the E8 byte
FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_begin, 14);
FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_end, 6);
FILL_FROM_SIGNATURED_CALLER_FROM_START(server, CScriptDictionary_CIterator_operator_NE, 4);
```

### Step 6: Call Function in Business Code

Always check pointer before calling:

```cpp
if (g_pfn_FunctionName)
{
    g_pfn_FunctionName(arg1, arg2);
}
```

**Example usage**:
```cpp
ASEXT_RegisterDocInitCallback([](CASDocumentation *pASDoc) {
    if (g_pfn_RegisterSCScriptColor24)
    {
        g_pfn_RegisterSCScriptColor24(pASDoc);
    }
});
```

## Checklist

- [ ] Analyze function with IDA Pro (decompile, disasm, bytes)
- [ ] Verify signature uniqueness in IDA Pro
- [ ] Define calling convention macro (if new) in `asext/include/asext_api.h`
- [ ] Define function typedef in `serverdef.h`
- [ ] Add Windows signature in `signatures.h` (direct or caller-based)
- [ ] Add Linux signature and symbol in `signatures.h`
- [ ] Add `PRIVATE_FUNCTION_DEFINE` in `server_hook.cpp`
- [ ] Add Windows fill macro in `meta_api.cpp` (`FILL_FROM_SIGNATURE` or `FILL_FROM_SIGNATURED_CALLER_FROM_START/END`)
- [ ] Add Linux 5.16 fill macro in `meta_api.cpp`
- [ ] Add Linux 5.15 `FILL_FROM_SYMBOL` in `meta_api.cpp`
- [ ] Call function with null check in business code
- [ ] Compile and test (Windows and Linux)
- [ ] Verify function found in logs

## Key Files

- `asext/include/asext_api.h` - Calling convention macros
- `serverdef.h` - Function type definitions
- `signatures.h` - Signatures and symbols
- `server_hook.cpp` - Function pointer definitions
- `meta_api.cpp` - Function pointer filling
- `metamod/signatures_template.h` - Macro definitions

## References

- [calling-conventions.md](references/calling-conventions.md) - Detailed calling convention guide
- [signature-patterns.md](references/signature-patterns.md) - Signature creation best practices
- [troubleshooting.md](references/troubleshooting.md) - Common issues and solutions

## Scripts

### signature_converter.py

Converts between three signature formats:
- **C String**: `\x83\xEC\x2C\xA1\x2A` (for code)
- **Spaced**: `83 EC 2C A1 ??` (for IDA Pro)
- **Compact**: `83EC2CA12A` (compressed)

**Usage**:
```bash
# Convert to IDA Pro format
python scripts/signature_converter.py spaced "\x83\xEC\x2C"

# Convert to C string format
python scripts/signature_converter.py c_string "83 EC 2C"

# Show all formats
python scripts/signature_converter.py all "83EC2C"

# Get help
python scripts/signature_converter.py --help
```
