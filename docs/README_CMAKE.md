# CMake Build Guide

This project now supports building with CMake on both Windows and Linux.

## Prerequisites

### Windows
- Visual Studio 2017 or later
- CMake 3.10 or later
- Third-party libraries (such as capstone and bullet3) must be prebuilt and placed under the `thirdparty/` directory

### Linux
- GCC with 32-bit build support
- CMake 3.10 or later
- Third-party libraries must be prebuilt and placed under the `thirdparty/install/` directory

## Build Steps

### Windows

1. Use the provided batch file:
```bash
# Debug build
build.bat Debug

# Release build
build.bat Release
```

2. Or run CMake manually:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A Win32
cmake --build . --config Release
```

### Linux

1. Use the provided script:
```bash
# Add execute permission
chmod +x build.sh

# Debug build
./build.sh Debug

# Release build
./build.sh Release
```

2. Or run CMake manually:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Output Files

Compiled binaries are output to `output/<platform>/<configuration>/`:
- Windows: `output/Win32/Release/` or `output/Win32/Debug/`
- Linux: `output/i686/Release/` or `output/i686/Debug/`

## Project Dependencies

- `fallguys`, `ascurl`, and `asqcvar` depend on `asext`
- `metamod` is standalone
- `asusermsg` is standalone

## Third-Party Library Layout

Make sure the following third-party libraries are placed correctly:

### Windows
- Capstone: `thirdparty/capstone/`
- Bullet3: `thirdparty/bullet3/`
- FMOD: `thirdparty/fmod/`
- cURL: `thirdparty/curl/`
- OpenSSL: `thirdparty/openssl/`

### Linux
- All libraries should be under `thirdparty/install/<library_name>/linux-i386/`
- Each library should include `include/` and `lib/` subdirectories

## Notes

1. Linux builds default to 32-bit (`-m32`)
2. Windows uses the static runtime library (`/MT` or `/MTd`)
3. All project outputs have no prefix (for example, output is `metamod.dll` instead of `libmetamod.dll`)
4. The metamod project on Windows requires the `metamod.def` file
5. Some Linux targets may require linker scripts (such as `i386pe.merge`)
