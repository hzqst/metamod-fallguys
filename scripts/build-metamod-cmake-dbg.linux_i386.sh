SolutionDir=$(pwd)/..

cmake -B "$SolutionDir/build-cmake/dbg.linux_i386" -DCMAKE_INSTALL_PREFIX="$SolutionDir/install/dbg.linux_i386" -DCMAKE_BUILD_TYPE=Debug -S $SolutionDir

cmake --build "$SolutionDir/build-cmake/dbg.linux_i386" --target install

mkdir -p "$SolutionDir/build/addons/metamod/dlls"

cp "$SolutionDir/install/dbg.linux_i386/lib/metamod.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/dbg.linux_i386/lib/asext.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/dbg.linux_i386/lib/fallguys.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/dbg.linux_i386/lib/ascurl.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/dbg.linux_i386/lib/asqcvar.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/dbg.linux_i386/lib/asusermsg.so" "$SolutionDir/build/addons/metamod/dlls/"