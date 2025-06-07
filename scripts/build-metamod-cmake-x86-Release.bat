@echo off

setlocal

:: Check if SolutionDir is already set and non-empty
if not defined SolutionDir (
    :: Only set SolutionDir if it's not already set
    SET "SolutionDir=%~dp0.."
)

:: Ensure the path ends with a backslash
if not "%SolutionDir:~-1%"=="\" SET "SolutionDir=%SolutionDir%\"

cd /d "%SolutionDir%"

cmake -G "Visual Studio 17 2022" -B "%SolutionDir%build-cmake\x86\Release" -A Win32 -DCMAKE_INSTALL_PREFIX="%SolutionDir%install\x86\Release" -DCMAKE_BUILD_TYPE=Release -S %SolutionDir%

cmake --build "%SolutionDir%build-cmake\x86\Release" --config Release --target install

copy "$SolutionDir/install/x86/Release/bin/*.dll" "$SolutionDir/build/addons/metamod/dlls/"

endlocal 