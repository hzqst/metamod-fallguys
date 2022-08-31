cd /d "%~dp0"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86
 
    MSBuild.exe metamod.sln /t:metamod /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe metamod.sln /t:asext /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe metamod.sln /t:ascurl /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe metamod.sln /t:fallguys /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe metamod.sln /t:nodlfile /p:Configuration=Release /p:Platform="Win32"
)