# Release Debug Symbols Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Enable optimized release builds to retain debug information across native `make`, CMake, and MSVC, and publish an extra Windows archive containing matching `.pdb` files.

**Architecture:** Keep the current release layout and artifact names, then inject debug-symbol generation into each source-controlled build entry point instead of introducing a new build configuration. Use the existing Windows CMake release build as the CI source of truth, then assemble a second archive by pairing the release DLLs with their matching PDBs from the same build. Verification focuses on artifact presence on Windows and DWARF section presence on Linux.

**Tech Stack:** GNU make, CMake, MSVC/MSBuild, batch scripts, shell scripts, PowerShell, GitHub Actions

---

### Task 1: Native make `opt` builds retain DWARF

**Files:**
- Modify: `metamod/Makefile`
- Test: `build/addons/metamod/dlls/`
- Test: `metamod/opt.linux_i386` or the active native-make output directory

**Step 1: Capture the current `opt` behavior**

Run: `rg -n "CCDEBUG|CFLAGS := \\$\\(CCOPT\\) \\$\\(CFLAGS\\) \\$\\(ODEF\\)" metamod/Makefile`
Expected: `OPT=opt` uses optimized flags and `CCDEBUG` is only wired into the debug branch.

**Step 2: Add release-symbol flags to the `OPT=opt` branch**

Update `metamod/Makefile` so the `OPT=opt` path keeps the existing optimization flags and also appends the release DWARF flag, using the existing shared make logic that all bundled module wrappers include.

Suggested target shape:

```make
CCDEBUG += -ggdb3
RELEASE_DEBUG_INFO += -g

ifeq "$(OPT)" "opt"
    CFLAGS := $(CCOPT) $(RELEASE_DEBUG_INFO) $(CFLAGS) $(ODEF)
endif
```

**Step 3: Verify the file change is in place**

Run: `rg -n "RELEASE_DEBUG_INFO|\\$\\(CCOPT\\).*\\$\\(RELEASE_DEBUG_INFO\\)" metamod/Makefile`
Expected: the `OPT=opt` branch explicitly keeps optimization and adds debug information.

**Step 4: Verify the intended build behavior**

Run: `cd scripts && sh ./build-metamod-make-opt.linux_i386.sh`
Run: `readelf --sections build/addons/metamod/dlls/metamod.so | grep ".debug_"`
Expected: the native `opt` build still succeeds and the resulting `.so` exposes `.debug_*` sections.

**Step 5: Commit the native make change**

```bash
git add metamod/Makefile
git commit -m "build: keep dwarf info in native opt builds"
```

### Task 2: CMake release builds retain symbols and helper scripts stay correct

**Files:**
- Modify: `CMakeLists.txt`
- Modify: `scripts/build-metamod-cmake-opt.linux_i386.sh`
- Modify: `scripts/build-metamod-cmake-x86-Debug.bat`
- Test: `build-cmake/opt.linux_i386/`
- Test: `build-cmake/x86/Release/output/x86/Release/`

**Step 1: Capture the current release-policy gaps**

Run: `rg -n "CMAKE_BUILD_TYPE|CMAKE_CXX_FLAGS|CMAKE_C_FLAGS|GenerateDebugInformation|/DEBUG" CMakeLists.txt`
Expected: Linux `Release` only keeps `-O2`, Windows `Release` does not explicitly request compile and link debug information, and the helper scripts still contain the known copy-path and argument-spacing issues.

**Step 2: Add top-level release debug-symbol policy**

Update `CMakeLists.txt` so:

```cmake
if(WIN32)
    string(APPEND CMAKE_C_FLAGS_RELEASE " /Zi")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " /Zi")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE " /DEBUG")
elseif(UNIX)
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O2 -g")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O2 -g")
endif()
```

Keep the policy centralized at the top level so `metamod`, `asext`, `fallguys`, `ascurl`, `asqcvar`, and `asusermsg` inherit it automatically.

**Step 3: Fix the two helper scripts that were already called out in design**

Update:

- `scripts/build-metamod-cmake-opt.linux_i386.sh` so `-DLINK_AGAINST_OLDER_GLIBC=TRUE` and `-DOLDER_GLIBC_PATH=...` are two separate arguments
- `scripts/build-metamod-cmake-x86-Debug.bat` so it copies DLLs from `install\x86\Debug\bin\` instead of `install\x86\Release\bin\`

**Step 4: Verify Linux and Windows CMake release behavior**

Run: `cd scripts && ./build-metamod-cmake-opt.linux_i386.sh`
Run: `readelf --sections build/addons/metamod/dlls/metamod.so | grep ".debug_"`
Run: `cd scripts && build-metamod-cmake-x86-Release.bat`
Run: `Get-ChildItem build-cmake\\x86\\Release\\output\\x86\\Release\\*.pdb`
Expected: Linux release `.so` files contain DWARF sections and Windows release outputs now include PDB files.

**Step 5: Commit the CMake and helper-script changes**

```bash
git add CMakeLists.txt scripts/build-metamod-cmake-opt.linux_i386.sh scripts/build-metamod-cmake-x86-Debug.bat
git commit -m "build: retain release symbols in cmake builds"
```

### Task 3: Source-controlled MSVC projects emit usable release PDBs

**Files:**
- Modify: `metamod/metamod.vcxproj`
- Modify: `asext/asext.vcxproj`
- Modify: `ascurl/ascurl.vcxproj`
- Modify: `asqcvar/asqcvar.vcxproj`
- Modify: `asusermsg/asusermsg.vcxproj`
- Modify: `fallguys/fallguys.vcxproj`
- Test: `output/x86/Release/`

**Step 1: Capture the current release MSVC state**

Run: `Select-String -Path metamod\\metamod.vcxproj,asext\\asext.vcxproj,ascurl\\ascurl.vcxproj,asqcvar\\asqcvar.vcxproj,asusermsg\\asusermsg.vcxproj,fallguys\\fallguys.vcxproj -Pattern "<DebugInformationFormat>","<GenerateDebugInformation>","<ProgramDataBaseFile>"`
Expected: `metamod` release lacks `<GenerateDebugInformation>true</GenerateDebugInformation>`, and the release compile groups do not consistently request `ProgramDatabase` debug info.

**Step 2: Make release compile and link settings explicit**

For each release `ItemDefinitionGroup`, add or normalize:

```xml
<ClCompile>
  <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
</ClCompile>
<Link>
  <GenerateDebugInformation>true</GenerateDebugInformation>
  <ProgramDataBaseFile>$(OutDir)$(TargetName).pdb</ProgramDataBaseFile>
</Link>
```

Keep existing optimization settings such as `WholeProgramOptimization`, `OptimizeReferences`, and `EnableCOMDATFolding`.

**Step 3: Preserve current output directories**

Do not rename targets or move `OutDir`. The only accepted release-behavior change is that the corresponding `.pdb` now appears next to each `.dll` in `output/x86/Release/`.

**Step 4: Verify native MSVC release outputs**

Run: `cd scripts && build-metamod-msvc-x86-Release.bat`
Run: `Get-ChildItem output\\x86\\Release\\*.dll`
Run: `Get-ChildItem output\\x86\\Release\\*.pdb`
Expected: all six release DLLs and all six matching PDBs are present.

**Step 5: Commit the MSVC project changes**

```bash
git add metamod/metamod.vcxproj asext/asext.vcxproj ascurl/ascurl.vcxproj asqcvar/asqcvar.vcxproj asusermsg/asusermsg.vcxproj fallguys/fallguys.vcxproj
git commit -m "build: emit release pdbs for msvc projects"
```

### Task 4: Windows GitHub Actions publishes the extra PDB archive

**Files:**
- Modify: `.github/workflows/windows.yml`
- Test: `build/`
- Test: `build-with-pdb/`

**Step 1: Add a failing artifact assertion before packaging**

Extend `.github/workflows/windows.yml` with a PowerShell verification step that expects these files after `scripts/build-all-x86-Release.bat`:

```powershell
$expected = @(
  "metamod.pdb",
  "asext.pdb",
  "fallguys.pdb",
  "ascurl.pdb",
  "asqcvar.pdb",
  "asusermsg.pdb"
)
```

Point the check at `build-cmake\x86\Release\output\x86\Release\`. Before the build-system changes from Tasks 2 and 3, this assertion should fail for at least part of the set.

**Step 2: Build the second archive directory**

In `.github/workflows/windows.yml`, add a step that:

- copies `build\` to `build-with-pdb\`
- copies the six expected `.pdb` files from `build-cmake\x86\Release\output\x86\Release\` to `build-with-pdb\addons\metamod\dlls\`

Use a strict copy loop so missing files stop the workflow immediately.

**Step 3: Zip and upload both Windows artifacts**

Keep the existing zip step for `metamod-fallguys-windows-x86.zip` and add a second zip step for `metamod-fallguys-windows-x86-with-pdb.zip`. Then update the release upload list so both files are attached on tag builds.

**Step 4: Verify the archive contents**

Run after a successful workflow or local dry-run packaging:

```powershell
Expand-Archive metamod-fallguys-windows-x86-with-pdb.zip -DestinationPath tmp\with-pdb
Get-ChildItem tmp\with-pdb\build-with-pdb\addons\metamod\dlls\*.pdb
```

Expected: the archive contains exactly the six matching PDB files beside the release DLLs.

**Step 5: Commit the workflow change**

```bash
git add .github/workflows/windows.yml
git commit -m "ci: publish windows release archive with pdbs"
```

### Task 5: Run end-to-end verification and record the result

**Files:**
- Modify: `docs/README_ASEXT.md` only if behavior documentation is deemed necessary during implementation
- Test: `build/addons/metamod/dlls/`
- Test: `build-cmake/x86/Release/output/x86/Release/`
- Test: `output/x86/Release/`

**Step 1: Re-run the minimal verification matrix**

Run:

```bash
cd scripts
sh ./build-metamod-make-opt.linux_i386.sh
sh ./build-metamod-cmake-opt.linux_i386.sh
build-metamod-cmake-x86-Release.bat
build-metamod-msvc-x86-Release.bat
```

Expected: each release path completes without switching to `Debug` or `dbg`.

**Step 2: Inspect the symbol artifacts**

Run:

```bash
readelf --sections ../build/addons/metamod/dlls/metamod.so | grep ".debug_"
```

Run:

```powershell
Get-ChildItem ..\build-cmake\x86\Release\output\x86\Release\*.pdb
Get-ChildItem ..\output\x86\Release\*.pdb
```

Expected: Linux `.so` files expose DWARF sections and both Windows build paths expose matching PDBs.

**Step 3: Review the final diff with a release-risk lens**

Run: `git diff --stat HEAD~4..HEAD`
Run: `git diff -- .github/workflows/windows.yml CMakeLists.txt metamod/Makefile metamod/metamod.vcxproj asext/asext.vcxproj ascurl/ascurl.vcxproj asqcvar/asqcvar.vcxproj asusermsg/asusermsg.vcxproj fallguys/fallguys.vcxproj scripts/build-metamod-cmake-opt.linux_i386.sh scripts/build-metamod-cmake-x86-Debug.bat`
Expected: only the intended build, packaging, and helper-script files changed.

**Step 4: Request code review before merge**

Use `@requesting-code-review` after the verification commands succeed, focusing on:

- release symbol generation across all three build entry points
- Windows artifact completeness
- helper-script regressions

**Step 5: Commit any final doc touch-up and prepare handoff**

```bash
git status --short
git add <only-if-needed>
git commit -m "docs: note release debug symbol support"
```

Skip this commit if no documentation touch-up was needed after verification and review.
