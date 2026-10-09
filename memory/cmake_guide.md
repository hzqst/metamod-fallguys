---
title: cmake_guide
type: note
permalink: metamod-fallguys/cmake-guide
---

# CMake Build Guide

Requirements: CMake 3.21+, Git, MSVC Win32 or Linux GCC i386 multilib.

```sh
git submodule update --init --recursive
cmake -S . -B build-cmake/native -A Win32
cmake --build build-cmake/native --config Release --parallel 6
cmake --install build-cmake/native --config Release --prefix build
```

Linux omits -A Win32 and sets -DCMAKE_BUILD_TYPE=Release; Debug is also supported.
Windows uses /MT (/MTd in Debug), Linux uses -m32 and PIC. Release includes debug symbols.
Installed components retain their DLL/SO names under addons/metamod/dlls; Windows PDBs
are installed alongside them. Metamod's definition file is src/metamod.def.

Each component is a standalone CMake project. Plugins depend on Metamod public headers;
ascurl, asqcvar, asusermsg and fallguys consume ASExt public headers and load ASExt at runtime.
The Metamod::SDK and ASExt::SDK interface targets do not build dependency DLLs/SOs.

Source overrides: METAMOD_SOURCE_PATH, ASEXT_SOURCE_PATH, ANGELSCRIPT_SOURCE_PATH,
CAPSTONE_SOURCE_PATH, PROCMAP_SOURCE_PATH; FallGuys adds BULLET3_SOURCE_PATH. Explicit
CMake variables override environment defaults; missing overrides fetch fixed commits.
Invalid supplied paths fail. Metamod and FallGuys first reuse their initialized nested
submodules (Capstone/procmap, bullet3_fork). The aggregate injects initialized shared
trees. Vendor libraries compile in the native build tree, with shared target guards;
no prebuilt thirdparty/install is required.

For formatting install clang-format==23.1.3, configure FORMAT_VALIDATION_ONLY=ON,
then build format-check or format. No native compiler is needed. Generated .clang-format
is ignored; vendor trees are excluded. Normal native builds do not install Python tooling.
