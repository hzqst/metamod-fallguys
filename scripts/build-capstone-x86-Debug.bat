@echo off
setlocal
set "MMFG_ROOT=%~dp0.."
git -C "%MMFG_ROOT%" submodule update --init --recursive
if errorlevel 1 exit /b 1
cmake -S "%MMFG_ROOT%" -B "%MMFG_ROOT%/build-cmake/x86-Debug" -A Win32
if errorlevel 1 exit /b 1
cmake --build "%MMFG_ROOT%/build-cmake/x86-Debug" --config Debug --parallel
if errorlevel 1 exit /b 1
cmake --install "%MMFG_ROOT%/build-cmake/x86-Debug" --config Debug --prefix "%MMFG_ROOT%/build"
if errorlevel 1 exit /b 1
