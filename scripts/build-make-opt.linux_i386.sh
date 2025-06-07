sh build-procmap-opt.linux_i386.sh
sh build-bullet3-opt.linux_i386.sh
sh build-capstone-opt.linux_i386.sh

SolutionDir=$(pwd)/..

cd "$SolutionDir"

make OPT=opt install