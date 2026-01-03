# Tech Stack

## Programming Language
- **C++17** (CMAKE_CXX_STANDARD 17)
- C for some components

## Build Systems
- **CMake** (minimum version 3.15)
- **Visual Studio** solution files (.sln, .vcxproj)
- **GNU Make** (Makefile)

## Supported Platforms
- **Windows**: x86 (32-bit) and x64 (64-bit)
  - Visual Studio 2017/2019/2022 with vc141/vc142/vc143 toolset
  - Multi-threaded static runtime library
- **Linux**: i386 (32-bit) and amd64 (64-bit)
  - GCC with -m32 flag for 32-bit builds
  - Older glibc (2.24) for portability

## Third-Party Libraries
- **capstone**: Disassembly framework
- **bullet3**: Physics library (Bullet3Dynamics, BulletCollision, LinearMath, etc.)
- **libcurl**: HTTP client library (used in ascurl plugin)
- **procmap**: Process memory mapping (Linux only)

## SDK Dependencies
- **Half-Life SDK** (hlsdk): Provides engine interfaces, common headers, and game DLL APIs
  - Located in `hlsdk/` directory
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
