# metamod-fallguys

This is a metamod plugin for Fall Guys maps in Sven Co-op (Steam version) (Windows or Linux)

# Installation

1. Copy everything from `build` directory into `Sven Co-op/svencoop`

2. Done

# Build Instruction (Windows)

1. Visual Studio 2017 or 2019, with vc141 or vc142 toolset must be installed in your system.

2. run `build-all-win32.bat`

3. `metamod.dll` and `fallguys.dll` should be generated at `build/addons/metamod/dlls` if no error(s) occours.

# Build Instruction (Linux)

1. GNU C++ Build environment. Install with `sudo apt-get install make build-essential gcc gcc-multilib g++-multilib` if something is missing.

2. `make OPT=opt install` or `./build-all-linux.sh` (Remember to give execution privilege if permission denied)

3. `metamod.so` and `fallguys.so` should be generated at `build/addons/metamod/dlls` if no error(s) occours.