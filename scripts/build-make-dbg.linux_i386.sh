sh build-procmap-dbg.linux_i386.sh
sh build-bullet3-dbg.linux_i386.sh
sh build-capstone-dbg.linux_i386.sh

SolutionDir=$(pwd)/..

cd "$SolutionDir"

make OPT=dbg install