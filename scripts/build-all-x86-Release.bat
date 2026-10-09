@echo off
setlocal
set "MMFG_ROOT=%~dp0.."
git -C "%MMFG_ROOT%" submodule update --init --recursive
if errorlevel 1 exit /b 1
cmake -S "%MMFG_ROOT%" -B "%MMFG_ROOT%/build/x86/Release" -A Win32
if errorlevel 1 exit /b 1
cmake --build "%MMFG_ROOT%/build/x86/Release" --config Release --parallel
if errorlevel 1 exit /b 1
cmake --install "%MMFG_ROOT%/build/x86/Release" --config Release
if errorlevel 1 exit /b 1
