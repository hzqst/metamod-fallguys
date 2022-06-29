call cmake -S "%~dp0bullet3" -B "%~dp0bullet3/build" -A Win32 -DUSE_GLUT=FALSE -DUSE_GRAPHICAL_BENCHMARK=FALSE -DBUILD_BULLET2_DEMOS=FALSE -DBUILD_BULLET_ROBOTICS_EXTRA=FALSE -DBUILD_BULLET_ROBOTICS_GUI_EXTRA=FALSE -DBUILD_CLSOCKET=FALSE -DBUILD_CPU_DEMOS=FALSE -DBUILD_ENET=FALSE -DBUILD_GIMPACTUTILS_EXTRA=FALSE -DBUILD_HACD_EXTRA=FALSE -DBUILD_INVERSE_DYNAMIC_EXTRA=FALSE -DBUILD_OBJ2SDF=FALSE -DBUILD_OPENGL3_DEMOS=FALSE -DBUILD_UNIT_TESTS=FALSE -DUSE_MSVC_SSE2=TRUE -DUSE_MSVC_SSE=FALSE -DUSE_MSVC_AVX=FALSE

cd /d "%~dp0"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (

    "%InstallDir%\Common7\Tools\vsdevcmd.bat" -arch=x86

    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:Bullet3Collision /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:Bullet3Common /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:Bullet3Dynamics /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:Bullet3Geometry /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:BulletCollision /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:BulletDynamics /p:Configuration=Release /p:Platform="Win32"
    MSBuild.exe "bullet3\build\BULLET_PHYSICS.sln" /t:LinearMath /p:Configuration=Release /p:Platform="Win32"
)