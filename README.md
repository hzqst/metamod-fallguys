# metamod-fallguys

This is a modified version of metamod along with a group of metamod plugins mainly to provide server-side feature expansions for Sven Co-op.

This metamod is based on [Bots-United's metamod-p](https://github.com/Bots-United/metamod-p)

* Any third-party plugins such as [amxmodx](https://github.com/alliedmodders/amxmodx) are still compatible with this modified version of metamod.

* You are welcome to request for any new hooks or server-side features which is not implemented by Sven Co-op team yet.

* Compatibility with other games (e.g. Counter-Strike) is not guaranteed as it's designed only for Sven Co-op.

## The differences between this metamod and Bots-United's one ?

1. An older version of glibc (iirc it's 2.24, which is literally the glibc that linux build of Sven Co-op is using) is forced to be used in the makefile. The compiled binaries (`.so`) are portable now. No need to recompile them when running on an up-to-date linux distribution.

2. A fallback solution was added to mitigate a problem that `GiveFnptrsToDll` could not be found in plugin dll (This used to happen when plugin was compiled by a newer version of Visual Studio like VS2022). see also https://github.com/Bots-United/metamod-p/issues/24

3. A couple of new APIs were added to mutil API set.

## Related project

[Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys)

# Plugins

### fallguys.dll (fallguys.so)

* This is required by map `Fall Guys in Sven Co-op`

[Documentation](README_FALLGUYS.md)

### asext.dll (asext.so)

This plugin provides ability of registering third-party hooks or methods in Sven Co-op's AngelScript engine.

* This is required by map `Fall Guys in Sven Co-op`

[Documentation](README_ASEXT.md)

### ascurl.dll (ascurl.so)

This plugin provides ability of using libcurl to send HTTP request in angelscript. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](README_ASCURL.md)

### asqcvar.dll (asqcvar.so)

This plugin provides ability of retreiving cvars from client. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](README_ASQCVAR.md)

### asusermsg.dll (asusermsg.so)

This plugin provides ability of hooking UserMsg. mainly for server ops and developers to use in their own angelscript plugin.

* This is not required if you just gonna play `Fall Guys in Sven Co-op`

[Documentation](README_ASUSERMSG.md)

# Installation

1. Download latest build (-windows or -linux depending on your OS) from [GitHub Release](https://github.com/hzqst/metamod-fallguys/releases), then unzip it.

2. Copy everything from the previously unarchived `build` directory into `\steamapps\common\Sven Co-op\svencoop` 

* (Warning: neither `svencoop_addon` nor `svencoop_downloads` is supported)*

* The directory hierarchy should be something like this :

```
-- Sven Co-op (Sven Co-op Dedicated Server)
---- svencoop
------ addons
-------- metamod
---------- dlls
------------ asqcvar.dll (asqcvar.so)
------------ ascurl.dll (ascurl.so)
------------ asusermsg.dll (asusermsg.so)
------------ asext.dll (asext.so)
------------ fallguys.dll (fallguys.so)
---------- plugins.ini
---- svencoop_addons
---- svencoop_downloads
---- svencoop.exe (or svends.exe / svends_run.sh / svencoop.sh)
```

3. If you had installed metamod and metamod-plugins (which was loaded by metamod) from other sources such as [Bots-United's metamod-p](https://github.com/Bots-United/metamod-p) or [jkivilin's metamod-p](https://github.com/jkivilin/metamod-p), you will have to add those metamod-plugins back to `plugins.ini` which might be overwritten in step (1).

4. You should either 

* Use `-dll addons/metamod/dlls/metamod.dll`(Windows) or `-dll addons/metamod/dlls/metamod.so` (linux) as launch parameter to launch the game (launch the dedicated server)

or

* Edit `Sven Co-op/svencoop/liblist.gam`

and change

```
gamedll "dlls/server.dll"
gamedll_linux "dlls/server.so"
```

to
 
```
gamedll "addons/metamod/dlls/metamod.dll"
gamedll_linux "addons/metamod/dlls/metamod.so"
```

The edited `liblist.gam` should be something like this ![](/img/1.png)

* `addons` must be installed into `/Sven Co-op/svencoop`, neither `/Sven Co-op/svencoop_addons` nor `/Sven Co-op/svencoop_download`

* All plugins from this repository are not binary-compatible with `metamod-p` from other sources. You should always use metamod from [metamod-fallguys](https://github.com/hzqst/metamod-fallguys/tree/main/metamod) to load those plugins.

* Other third-party plugins ( e.g [amxmodx](https://github.com/alliedmodders/amxmodx) ) are still binary-compatible with [metamod-fallguys](https://github.com/hzqst/metamod-fallguys/tree/main/metamod). You don't have to re-compile them. Just put them in the `plugins.ini`.

# Build Requirements (Windows)

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Build Instruction (Windows)

1. `git clone --recursive https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. Run `build-win32-all.bat`

3. Be patient, `metamod.dll`, `asext.dll`, `ascurl.dll`, `asqcvar.dll` and `fallguys.dll` will be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Requirements (Linux)

1. GNU C++ Build environment. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if missing.

2. CMake. Install with `suto apt-get install cmake` if missing.

3. Git client. Install with `suto apt-get install git` if missing.

# Build Instruction (Linux)

1. `git clone --recursive https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys/scripts`

2. `sudo chmod +777 *.sh` to make all .sh executable, then `./build-all-linux.sh`

3. Be patient, `metamod.so`, `asext.so` and `fallguys.so` will be generated at `build/addons/metamod/dlls` if no error(s) occours.
