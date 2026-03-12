# Release Debug Symbols Design

**Date:** 2026-03-12

**Goal:** Enable release-style debug information for `ascurl`, `asext`, `asqcvar`, `asusermsg`, `fallguys`, and `metamod` across native `make`, CMake, and MSVC builds, and publish an additional Windows release archive that includes matching `.pdb` files.

## Scope

- Build entry points:
  - top-level native `make`
  - top-level CMake plus module `CMakeLists.txt`
  - `metamod.sln` and the six module `*.vcxproj` files
- Release packaging:
  - keep the existing Windows archive
  - add `metamod-fallguys-windows-x86-with-pdb.zip`
- Modules covered:
  - `metamod`
  - `asext`
  - `fallguys`
  - `ascurl`
  - `asqcvar`
  - `asusermsg`

## Requirements

1. `Release` and `opt` builds remain optimized release outputs.
2. Windows release builds generate matching `.pdb` files for all six modules.
3. Linux `opt` builds retain DWARF debug sections, using an optimized configuration such as `-O2 -g`.
4. Existing artifact names, module names, and output layout stay stable unless a new debug-symbol archive is being added.
5. Generated build directories such as `build-cmake/` are not treated as source-of-truth configuration files.

## Architecture

### 1. Native make

The shared native make implementation lives in `metamod/Makefile` and is reused by the bundled module `Makefile` wrappers. The `OPT=opt` branch will continue to use the optimized code path, but it will also include debug information flags so the final `.so` files keep DWARF data. The `OPT=dbg` branch stays as the full debug variant; this change only upgrades `opt` from "optimized without symbols" to "optimized with symbols".

This keeps the existing install directories such as `opt.linux_i386` unchanged, which means current scripts and packaging paths do not need structural changes.

### 2. CMake

The top-level `CMakeLists.txt` becomes the single place that defines release-debug-symbol policy for CMake builds. Linux `Release` will add debug information alongside optimization, and Windows `Release` will explicitly request compiler and linker debug info so all shared-library targets emit `.pdb` files in a predictable way.

The module `CMakeLists.txt` files remain mostly focused on sources, includes, and linkage. They should inherit the release-debug-symbol policy from the top level instead of duplicating flags target by target.

### 3. MSVC solution projects

The source-controlled `*.vcxproj` files remain the source of truth for native MSVC builds. Most plugin projects already enable link-time debug information in `Release`; `metamod/metamod.vcxproj` is the main gap. All six release projects should explicitly describe their release debug behavior so builds are reproducible across developer machines and GitHub Actions runners.

Windows release builds will still produce the same `.dll` files in the same output locations. The only behavioral addition is that matching `.pdb` files are kept and later packaged.

## Packaging And Release Flow

### Windows archives

The current GitHub Actions Windows workflow continues to produce `metamod-fallguys-windows-x86.zip` from the existing `build/` directory.

An additional packaging step creates a temporary release directory, for example `build-with-pdb/`, by copying the current `build/` tree and then placing the six matching `.pdb` files into `build-with-pdb/addons/metamod/dlls/` next to the corresponding `.dll` files.

The workflow then publishes:

- `metamod-fallguys-windows-x86.zip`
- `metamod-fallguys-windows-x86-with-pdb.zip`

Both archives must come from the same build outputs so every `.dll` matches its `.pdb`.

### Linux artifacts

Linux release packaging keeps using the existing `build/` layout. The `.so` files inside that layout will now retain DWARF data because the `opt` build no longer strips or omits debug information.

No new Linux archive name is required for this task.

## Data Flow

1. Build scripts invoke native `make`, CMake, or MSVC using `Release` or `opt`.
2. Build configuration injects optimization plus debug-information flags.
3. Outputs land in the existing release install or output directories.
4. Windows workflow copies release DLLs into `build/` as today.
5. Windows workflow separately gathers matching release PDBs, creates `build-with-pdb/`, and zips that directory.
6. GitHub release uploads both Windows archives.

## Validation

### Windows

- Confirm each module generates:
  - `metamod.dll` and `metamod.pdb`
  - `asext.dll` and `asext.pdb`
  - `fallguys.dll` and `fallguys.pdb`
  - `ascurl.dll` and `ascurl.pdb`
  - `asqcvar.dll` and `asqcvar.pdb`
  - `asusermsg.dll` and `asusermsg.pdb`
- Fail the workflow if any expected `.pdb` is missing before the `with-pdb` archive is created.
- Confirm both zip files are attached to tag releases.

### Linux

- Confirm native `make OPT=opt install` outputs `.so` files with `.debug_*` sections.
- Confirm CMake `Release` outputs `.so` files with `.debug_*` sections.
- Verify no packaging step strips those sections after installation or copy.

## Error Handling

- If a Windows module DLL exists but its `.pdb` does not, packaging should stop with a clear failure.
- If a Linux release artifact lacks `.debug_*` sections, verification should fail before claiming the release-debug-symbol policy is complete.
- If build-system behavior differs between native MSVC and CMake on Windows, the source-controlled configuration files take precedence and the mismatch must be resolved in source, not in generated files.

## Opportunistic Fixes Included In Scope

- Fix `scripts/build-metamod-cmake-x86-Debug.bat` so it copies from the `Debug` install directory instead of `Release`.
- Fix `scripts/build-metamod-cmake-opt.linux_i386.sh` so the `-DLINK_AGAINST_OLDER_GLIBC=TRUE` and `-DOLDER_GLIBC_PATH=...` arguments are separated correctly.

## Out Of Scope

- Adding a new Linux debug-symbol archive name
- Changing module names or destination directories
- Editing generated files under `build-cmake/`
- Introducing symbol-stripping and split-debug-file workflows

## Implementation Notes

- Prefer top-level CMake policy over per-target duplication where possible.
- Preserve current output paths so existing consumer scripts and release packaging stay compatible.
- Treat the extra Windows PDB archive as an additive release artifact, not a replacement for the current archive.
