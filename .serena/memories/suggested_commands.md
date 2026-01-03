# Suggested Commands

## Build Commands

### Windows (Primary Method)
```batch
cd scripts
build-all-x86-Release.bat
```
This script will:
1. Build bullet3 library (`build-bullet3-x86-Release.bat`)
2. Build capstone library (`build-capstone-x86-Release.bat`)
3. Build metamod and all plugins using CMake (`build-metamod-cmake-x86-Release.bat`)

Output files will be in: `build/addons/metamod/dlls/`
- `metamod.dll`
- `asext.dll`
- `ascurl.dll`
- `asqcvar.dll`
- `fallguys.dll`
- `asusermsg.dll`

### Windows (Alternative Methods)
```batch
# Using Visual Studio solution
# Open metamod.sln in Visual Studio and build

# Using CMake directly
cd scripts
build-metamod-cmake-x86-Release.bat

# Using MSVC command line
cd scripts
build-metamod-msvc-x86-Release.bat
```

### Linux (Primary Method)
```bash
cd scripts
chmod +x *.sh
./build-all-opt.linux_i386.sh
```
This script will build all dependencies and plugins.

Output files will be in: `build/addons/metamod/dlls/`
- `metamod.so`
- `asext.so`
- `ascurl.so`
- `asqcvar.so`
- `fallguys.so`
- `asusermsg.so`

### Linux (Alternative Methods)
```bash
# Using CMake directly
cd scripts
./build-metamod-cmake-opt.linux_i386.sh

# Using Make directly
cd scripts
./build-metamod-make-opt.linux_i386.sh
```

## Debug Builds

### Windows Debug
```batch
cd scripts
build-bullet3-x86-Debug.bat
build-capstone-x86-Debug.bat
build-metamod-cmake-x86-Debug.bat
```

### Linux Debug
```bash
cd scripts
./build-bullet3-dbg.linux_i386.sh
./build-capstone-dbg.linux_i386.sh
./build-metamod-cmake-dbg.linux_i386.sh
```

## Git Commands
```bash
# Clone with submodules
git clone --recursive https://github.com/hzqst/metamod-fallguys

# Update submodules
git submodule update --init --recursive

# Check status
git status

# View commit history
git log --oneline

# View changes
git diff
```

## Windows System Commands
```cmd
# List files
dir

# Change directory
cd <path>

# Create directory
mkdir <dirname>

# Remove directory
rmdir /s /q <dirname>

# Copy files
copy <source> <dest>
xcopy /s /e <source> <dest>

# Find files
where <filename>

# Search in files (using findstr)
findstr /s /i "pattern" *.cpp

# View file content
type <filename>
more <filename>
```

## Project-Specific Notes
- Always build from the `scripts/` directory
- Third-party libraries must be built before metamod and plugins
- The build system supports both x86 (32-bit) and x64 (64-bit) on Windows
- Linux builds are 32-bit by default (i386)
- Output binaries are placed in `build/addons/metamod/dlls/`
