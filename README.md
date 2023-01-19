# metamod-fallguys

This is a group of metamod plugins for [Fall Guys in Sven Co-op](https://github.com/hzqst/sven-fallguys) as feature expansions.

The metamod core is based on [metamod-p](https://github.com/Bots-United/metamod-p)

* You are welcome to request for any new hooks or server-side features which is not implemented by Sven Co-op team yet.

# What it does ?

### fallguys.dll (fallguys.so)

[Documentation](README_FALLGUYS.md)

### asext.dll (asext.so)

[Documentation](README_ASEXT.md)

### ascurl.dll (ascurl.so)

[Documentation](README_ASCURL.md)

### asqcvar.dll (asqcvar.so)

[Documentation](README_ASQCVAR.md)

### asusermsg.dll (asusermsg.so)

[Documentation](README_ASUSERMSG.md)

# Installation

1. Download from [GitHub Release](https://github.com/hzqst/metamod-fallguys/releases), then unzip it.

1. Copy everything from `build` directory into `\steamapps\common\Sven Co-op\svencoop` *(Warning: either `svencoop_addon` or `svencoop_downloads` is not supported)*

The directory hierarchy should be something like this :
```
-- Sven Co-op (or Sven Co-op Dedicated Server)
---- svencoop
------ addons
-------- metamod
---------- dlls
------------ asqcvar.dll (asqcvar.so)
------------ ascurl.dll (ascurl.so)
------------ asext.dll (asext.so)
------------ fallguys.dll (fallguys.so)
---------- plugins.ini
---- svencoop_addons
---- svencoop_downloads
---- svencoop.exe (or svends.exe / svends_run.sh / svencoop.sh)
```

2. You should either 

* Use `-dll addons/metamod/dlls/metamod.dll`(Windows) or `-dll addons/metamod/dlls/metamod.so` (linux) as launch parameter to launch the game (or launch the dedicated server), 

* Or edit `Sven Co-op/svencoop/liblist.gam`

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

# Build Requirements (Windows)

1. [Visual Studio 2017 / 2019 / 2022, with vc141 / vc142 / vc143 toolset](https://visualstudio.microsoft.com/)

2. [CMake](https://cmake.org/download/)

3. [Git for Windows](https://gitforwindows.org/)

# Build Instruction (Windows)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. Run `build-win32-all.bat`

3. Be patient, `metamod.dll`, `asext.dll`, `ascurl.dll`, `asqcvar.dll` and `fallguys.dll` will be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Requirements (Linux)

1. GNU C++ Build environment. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if missing.

2. CMake. Install with `suto apt-get install cmake` if missing.

3. Git client. Install with `suto apt-get install git` if missing.

# Build Instruction (Linux)

1. `git clone https://github.com/hzqst/metamod-fallguys`, then `cd metamod-fallguys`

2. `sudo chmod +777 build-all-linux.sh` to make it executable, then `./build-all-linux.sh`

3. Be patient, `metamod.so`, `asext.so` and `fallguys.so` will be generated at `build/addons/metamod/dlls` if no error(s) occours.