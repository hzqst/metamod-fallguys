---
title: tech_stack
type: note
permalink: metamod-fallguys/tech-stack
---

# Tech Stack

## Programming Language
- **C++17** (CMAKE_CXX_STANDARD 17)
- C for some components

## Build Systems
- **CMake** (minimum version 3.21) — the ONLY supported build system
- The legacy GNU Make (`Makefile`/`Config.mak`) and Visual Studio
  (`metamod.sln`/`*.vcxproj`) build systems were removed (issue #42); do not
  reintroduce them. `build-cmake/`, `install/` and `build/addons/.../dlls` are
  build output and are gitignored.

## Supported Platforms
- **Windows**: Win32/x86 (32-bit)
  - Visual Studio 2017/2019/2022 with vc141/vc142/vc143 toolset
  - Multi-threaded static runtime library
- **Linux**: i386 (32-bit)
  - GCC with -m32 flag for 32-bit builds
  - No glibc pinning: the former thirdparty/glibc_224/force_link_glibc_2.24.h mechanism was removed (it never actually worked); Linux builds follow the build host glibc

## Third-Party Libraries
- **capstone**: Disassembly framework
- **bullet3**: Physics library (Bullet3Dynamics, BulletCollision, LinearMath, etc.)
- **libcurl**: HTTP client library (used in ascurl plugin)
- **procmap**: Process memory mapping (Linux only)

## SDK Dependencies
- **Half-Life SDK** (hlsdk): Provides engine interfaces, common headers, and game DLL APIs
  - Owned by Metamod in `metamod/include/HLSDK/`
  - Includes: common, dlls, pm_shared, engine headers

## Compiler Flags
### Windows
- `/D WIN32 /D _WINDOWS /D _USRDLL /D _CRT_SECURE_NO_WARNINGS`
- Multi-threaded static runtime
- Character set: NotSet (no UNICODE)

### Linux
- `-DPLATFORM_POSIX -DLINUX -D_LINUX`
- `-m32 -fPIC` for 32-bit position-independent code
- `-O2` for release builds, `-g` for debug builds
