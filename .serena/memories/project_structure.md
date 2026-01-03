# Project Structure

## Root Directory Layout
```
metamod-fallguys/
├── .git/                   # Git repository
├── .github/                # GitHub workflows/actions
├── .serena/                # Serena agent configuration
├── .vs/                    # Visual Studio cache
├── metamod/                # Core metamod library
├── fallguys/               # Fall Guys plugin
├── asext/                  # AngelScript extension plugin
├── ascurl/                 # libcurl HTTP plugin
├── asqcvar/                # Client cvar query plugin
├── asusermsg/              # UserMsg hook plugin
├── hlsdk/                  # Half-Life SDK (submodule)
├── thirdparty/             # Third-party libraries
├── scripts/                # Build scripts
├── tools/                  # Development tools
├── build/                  # Build output directory
├── build-cmake/            # CMake build directory
├── install/                # Installation staging
├── intermediate/           # Intermediate build files
├── output/                 # Final output binaries
├── Debug/                  # Debug build artifacts
├── Release/                # Release build artifacts
├── CMakeLists.txt          # Root CMake configuration
├── metamod.sln             # Visual Studio solution
├── Makefile                # GNU Make configuration
├── Config.mak              # Make configuration
└── README*.md              # Documentation files
```

## Core Components

### metamod/
Core metamod library that loads and manages plugins.
- **Key files**: `metamod.cpp`, `metamod.h`, `mplugin.cpp`, `mutil.cpp`
- **API headers**: `meta_api.h`, `meta_eiface.h`
- **Plugin management**: `mlist.cpp`, `mplugin.cpp`
- **Engine hooks**: `engine_api.cpp`, `dllapi.cpp`
- **Platform abstraction**: `osdep.cpp`, `osdep_p.cpp`
- **Build**: CMakeLists.txt, Makefile, metamod.vcxproj

### fallguys/
Plugin for Fall Guys in Sven Co-op map.
- Provides game-specific functionality
- Required for Fall Guys map to work

### asext/
AngelScript extension plugin.
- Allows registering third-party hooks in AngelScript engine
- Required by Fall Guys map
- Foundation for other AS plugins

### ascurl/
HTTP request plugin using libcurl.
- Enables HTTP requests from AngelScript
- Optional for Fall Guys map

### asqcvar/
Client cvar retrieval plugin.
- Allows querying client console variables
- Optional for Fall Guys map

### asusermsg/
UserMsg hooking plugin.
- Enables hooking of user messages
- Optional for Fall Guys map

### hlsdk/
Half-Life SDK (git submodule).
- **common/**: Common headers and utilities
- **dlls/**: Game DLL interfaces
- **pm_shared/**: Player movement shared code
- **engine/**: Engine interfaces and callbacks

### thirdparty/
Third-party library dependencies.
- **capstone/**: Disassembly framework
- **bullet3/**: Physics engine
- **libcurl/**: HTTP client (for ascurl)
- **procmap/**: Process memory mapping (Linux)
- **install/**: Pre-built library binaries

### scripts/
Build automation scripts.
- `build-all-*.bat/sh`: Complete build scripts
- `build-bullet3-*.bat/sh`: Bullet3 library builds
- `build-capstone-*.bat/sh`: Capstone library builds
- `build-metamod-*.bat/sh`: Metamod builds (CMake/Make/MSVC)

## Build Output Structure
```
build/
└── addons/
    └── metamod/
        ├── dlls/
        │   ├── metamod.dll/so
        │   ├── asext.dll/so
        │   ├── ascurl.dll/so
        │   ├── asqcvar.dll/so
        │   ├── asusermsg.dll/so
        │   └── fallguys.dll/so
        └── plugins.ini
```

## Configuration Files
- **plugins.ini**: Lists plugins to load at startup
- **config.ini**: Generic metamod configuration
- **exec.cfg**: Commands to execute at startup
- **.gitmodules**: Git submodule configuration
- **.gitignore**: Git ignore patterns

## Documentation Files
- **README.md**: Main project documentation
- **README_FALLGUYS.md**: Fall Guys plugin documentation
- **README_ASEXT.md**: asext plugin documentation
- **README_ASCURL.md**: ascurl plugin documentation
- **README_ASQCVAR.md**: asqcvar plugin documentation
- **README_ASUSERMSG.md**: asusermsg plugin documentation
- **README_CMAKE.md**: CMake build documentation
- **LICENSE**: GPL license
- **GPL.txt**: Full GPL text
- **ABOUT.txt**: Project information
