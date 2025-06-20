SolutionDir=$(pwd)/..

cmake -B "$SolutionDir/build-cmake/opt.linux_i386" -DCMAKE_INSTALL_PREFIX="$SolutionDir/install/opt.linux_i386" -DCMAKE_BUILD_TYPE=Release  -DLINK_AGAINST_OLDER_GLIBC=TRUE-DOLDER_GLIBC_PATH="$SolutionDir/thirdparty/glibc_224/force_link_glibc_2.24.h" -S $SolutionDir

cmake --build "$SolutionDir/build-cmake/opt.linux_i386" --target install

mkdir -p "$SolutionDir/build/addons/metamod/dlls"

cp "$SolutionDir/install/opt.linux_i386/lib/metamod.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/opt.linux_i386/lib/asext.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/opt.linux_i386/lib/fallguys.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/opt.linux_i386/lib/ascurl.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/opt.linux_i386/lib/asqcvar.so" "$SolutionDir/build/addons/metamod/dlls/"
cp "$SolutionDir/install/opt.linux_i386/lib/asusermsg.so" "$SolutionDir/build/addons/metamod/dlls/"