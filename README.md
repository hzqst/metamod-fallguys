# metamod-fallguys

This is a modified version of metamod along with a group of metamod plugins mainly to provide server-side feature expansions for Sven Co-op.

This metamod is based on [Bots-United's metamod-p](https://github.com/Bots-United/metamod-p)

* Any third-party plugins such as [amxmodx](https://github.com/alliedmodders/amxmodx) are still compatible with this modified version of metamod.

* You are welcome to request for any new hooks or server-side features which is not implemented by Sven Co-op team yet.

* Compatibility with other games (e.g. Counter-Strike) is not guaranteed as it's designed only for Sven Co-op.

## The differences between this metamod and Bots-United's one ?

1. A fallback solution was added to mitigate a problem that `GiveFnptrsToDll` could not be found in plugin dll (This used to happen when plugin was compiled by a newer version of Visual Studio like VS2022). see also https://github.com/Bots-United/metamod-p/issues/24

2. A couple of new APIs were added to mutil API set.

## Related project

[Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys)

[Physics vehicle demo in Sven Co-op](https://github.com/hzqst/sven-vehicle)

# Plugins

### fallguys.dll (fallguys.so)

* This is required by map `Fall Guys in Sven Co-op`

[Documentation](docs/README_FALLGUYS.md)

### asext.dll (asext.so)

This plugin provides ability of registering third-party hooks or methods in Sven Co-op's AngelScript engine.

* This is required by map `Fall Guys in Sven Co-op`

[Documentation](docs/README_ASEXT.md)

### ascurl.dll (ascurl.so)

This plugin provides ability of using libcurl to send HTTP request in angelscript. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](docs/README_ASCURL.md)

### asqcvar.dll (asqcvar.so)

This plugin provides ability of retreiving cvars from client. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](docs/README_ASQCVAR.md)

### asusermsg.dll (asusermsg.so)

This plugin provides ability of hooking UserMsg. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](docs/README_ASUSERMSG.md)

# Installation

tutorial video: https://www.youtube.com/watch?v=r2NRp5TZmUM

1. Download latest build (-windows or -linux depending on your OS) from [GitHub Release](https://github.com/hzqst/metamod-fallguys/releases), then unzip it.

2. Copy everything from the previously unarchived `build` directory into `\steamapps\common\Sven Co-op\svencoop` 

* (Warning: neither `svencoop_addon` nor `svencoop_downloads` is supported)*

* The directory hierarchy should be something like this :

```
Sven Co-op/ (or Sven Co-op Dedicated Server/)
|-- svencoop/
|   `-- addons/
|       `-- metamod/
|           |-- dlls/
|           |   |-- asqcvar.dll (asqcvar.so)
|           |   |-- ascurl.dll (ascurl.so)
|           |   |-- asusermsg.dll (asusermsg.so)
|           |   |-- asext.dll (asext.so)
|           |   `-- fallguys.dll (fallguys.so)
|           `-- plugins.ini
|-- svencoop_addons/
|-- svencoop_downloads/
`-- svencoop.exe (or svends.exe / svends_run.sh / svencoop.sh)
```

3. If you had installed metamod and metamod plugins from other sources such as [Bots-United's metamod-p](https://github.com/Bots-United/metamod-p) or [jkivilin's metamod-p](https://github.com/jkivilin/metamod-p), you will have to add those metamod plugins back to `plugins.ini` which might have been overwritten in the step (1).

4. You should either 

* Add `-dll addons/metamod/dlls/metamod.dll` (Windows) or `-dll addons/metamod/dlls/metamod.so` (linux) to launch parameter.

or

* Edit `Sven Co-op/svencoop/liblist.gam`, and change:

```
gamedll "dlls/server.dll"
gamedll_linux "dlls/server.so"
```

to
 
```
gamedll "addons/metamod/dlls/metamod.dll"
gamedll_linux "addons/metamod/dlls/metamod.so"
```

to make metamod work.

The edited `liblist.gam` should be something like this ![](/img/1.png)

* `addons` must be installed into `/Sven Co-op/svencoop`, neither `/Sven Co-op/svencoop_addons` nor `/Sven Co-op/svencoop_download`

* All plugins from this repository are not binary-compatible with `metamod-p` from other sources. You should always use metamod from [metamod-fallguys](https://github.com/hzqst/metamod-fallguys/tree/main/metamod) to load those plugins.

* Other third-party plugins ( e.g [amxmodx](https://github.com/alliedmodders/amxmodx) ) are still binary-compatible with [metamod-fallguys](https://github.com/hzqst/metamod-fallguys/tree/main/metamod). You don't have to re-compile them. Just put them in the `plugins.ini`.

# Build

The six components are independent repositories under the
[metamod-fallguys organization](https://github.com/metamod-fallguys), tracked here as
submodules. Public headers are in `include/`, implementation in `src/`. Metamod owns
HLSDK under `metamod/include/HLSDK/` and its private Capstone/procmap submodules.

Requirements: CMake 3.21+, Git, and Windows MSVC Win32 or Linux i386 GCC multilib.

```sh
git clone --recursive https://github.com/hzqst/metamod-fallguys
cd metamod-fallguys
git submodule update --init --recursive
cmake -S . -B build/x86/Release -A Win32
cmake --build build/x86/Release --config Release --parallel 6
cmake --install build/x86/Release --config Release
```

On Linux omit `-A Win32` and use `-DCMAKE_BUILD_TYPE=Release`; install `gcc-multilib`
and `g++-multilib`. For Debug use `build/x86/Debug` and `--config Debug` on Windows,
or `-DCMAKE_BUILD_TYPE=Debug` on Linux. Configure a separate tree for each configuration.
Installation places the complete `addons/` payload under `install/x86/<Config>/`,
including Windows PDBs and the default plugin list. Copy that directory's contents
into the game's `svencoop/` directory. An explicit `CMAKE_INSTALL_PREFIX` or
`cmake --install --prefix` overrides the default; existing trees retain their cached prefix.
Release archives retain their existing `build/addons/` layout for installation.
The existing `scripts/build-*.bat` and `scripts/build-*.sh` names remain compatibility
entry points to this unified build, independent of the caller's working directory.
Dependencies are compiled within the same CMake tree; no `thirdparty/install` is needed.

## Standalone components and dependencies

Each component can be cloned and built directly; see its own README. Plugins accept
`METAMOD_SOURCE_PATH` and `ASEXT_SOURCE_PATH` for local component clones. Additional
source overrides are `ANGELSCRIPT_SOURCE_PATH`, `CAPSTONE_SOURCE_PATH`, and
`PROCMAP_SOURCE_PATH`. FallGuys additionally accepts `BULLET3_SOURCE_PATH`, defaulting
to its nested `thirdparty/bullet3_fork` submodule. Explicit CMake paths override
environment defaults. Empty paths fetch fixed commits, preserving the customized
AngelScript ABI. Invalid explicit paths fail; dependency source trees are read-only.

The aggregate injects its initialized local dependency trees and reuses vendor targets.
`MMFG_BUILD_METAMOD`, `MMFG_BUILD_ASEXT`, `MMFG_BUILD_ASCURL`, `MMFG_BUILD_ASQCVAR`,
`MMFG_BUILD_ASUSERMSG`, and `MMFG_BUILD_FALLGUYS` may disable native components.
SDK consumption does not compile dependency plugins; their runtime loading stays unchanged.

## Formatting

```sh
python -m pip install clang-format==23.1.3
cmake -S . -B build-cmake/format -DFORMAT_VALIDATION_ONLY=ON
cmake --build build-cmake/format --target format-check
cmake --build build-cmake/format --target format
```

Formatting works without a native compiler. The aggregate targets cover all six
components, including disabled native components; `<component>-format-check` and
`<component>-format` select one. Normal native builds do not install or execute formatting.
The shared FormatValidation commit is fixed at `13c9fabe058e1f887ad1b03bb6884de911192c6a`;
`FORMAT_VALIDATION_SOURCE_PATH` can override its local path. Generated `.clang-format`
files are ignored; HLSDK and vendor sources are excluded.

## Updating components

Commit and publish changes in the component repository first, then commit the corresponding
gitlink in this repository. FetchContent dependency revisions are full commit IDs; update
them deliberately when changing a shared interface. The original aggregate history remains
available; component histories retain their relevant commits and contributors.
