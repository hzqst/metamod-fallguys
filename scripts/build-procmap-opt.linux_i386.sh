SolutionDir=$(pwd)/..

cmake -S "$SolutionDir/thirdparty/procmap_fork" -B "$SolutionDir/thirdparty/build/procmap/opt.linux_i386" -DCMAKE_INSTALL_PREFIX="$SolutionDir/thirdparty/install/procmap/opt.linux_i386" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32 -DLINK_AGAINST_OLDER_GLIBC=TRUE -DOLDER_GLIBC_PATH="$SolutionDir/thirdparty/glibc_224/force_link_glibc_2.24.h"

cmake --build "$SolutionDir/thirdparty/build/procmap/opt.linux_i386" --target install