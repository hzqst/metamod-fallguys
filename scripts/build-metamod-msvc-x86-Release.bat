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

for /f "usebackq tokens=*" %%i in (`tools\vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat"

    MSBuild.exe metamod.sln "/target:metamod" /p:Configuration="Release" /p:Platform="Win32"
    MSBuild.exe metamod.sln "/target:asext" /p:Configuration="Release" /p:Platform="Win32"
    MSBuild.exe metamod.sln "/target:asusermsg" /p:Configuration="Release" /p:Platform="Win32"
    MSBuild.exe metamod.sln "/target:asqcvar" /p:Configuration="Release" /p:Platform="Win32"
    MSBuild.exe metamod.sln "/target:ascurl" /p:Configuration="Release" /p:Platform="Win32"
    MSBuild.exe metamod.sln "/target:fallguys" /p:Configuration="Release" /p:Platform="Win32"
)

endlocal