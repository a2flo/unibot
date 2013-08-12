#!/bin/sh

UNIBOT_OS="unknown"
UNIBOT_PLATFORM="x32"
UNIBOT_MAKE="make"
UNIBOT_MAKE_PLATFORM="32"
UNIBOT_ARGS=""
UNIBOT_CPU_COUNT=1
UNIBOT_USE_CLANG=0

if [ $# -gt 0 ] && [ "$1" == "gcc" ]; then
	UNIBOT_ARGS="--gcc"
else
	UNIBOT_ARGS="--clang"
	UNIBOT_USE_CLANG=1
fi

case $( uname | tr [:upper:] [:lower:] ) in
	"darwin")
		UNIBOT_OS="macosx"
		UNIBOT_CPU_COUNT=$(sysctl machdep.cpu.thread_count | sed -E 's/.*(: )([:digit:]*)/\2/g')
		;;
	"linux")
		UNIBOT_OS="linux"
		UNIBOT_CPU_COUNT=$(cat /proc/cpuinfo | grep -m 1 'cpu cores' | sed -E 's/.*(: )([:digit:]*)/\2/g')
		;;
	[a-z0-9]*"bsd")
		UNIBOT_OS="bsd"
		UNIBOT_MAKE="gmake"
		UNIBOT_CPU_COUNT=$(sysctl hw.ncpu | sed -E 's/.*(: )([:digit:]*)/\2/g')
		;;
	"cygwin"* | "mingw"*)
		UNIBOT_OS="windows"
		UNIBOT_ARGS="--env cygwin"
		UNIBOT_CPU_COUNT=$(env | grep 'NUMBER_OF_PROCESSORS' | sed -E 's/.*=([:digit:]*)/\1/g')
		;;
	*)
		echo "unknown operating system - exiting"
		exit
		;;
esac

case $( uname -m ) in
	"i386"|"i486"|"i586"|"i686")
		UNIBOT_PLATFORM="x32"
		UNIBOT_MAKE_PLATFORM="32"
		;;
	"x86_64"|"amd64")
		UNIBOT_PLATFORM="x64"
		UNIBOT_MAKE_PLATFORM="64"
		;;
	*)
		echo "unknown architecture - using "${UNIBOT_PLATFORM}
		exit;;
esac


echo "using: premake4 --cc=gcc --os="${UNIBOT_OS}" gmake "${UNIBOT_ARGS}

premake4 --cc=gcc --os=${UNIBOT_OS} gmake ${UNIBOT_ARGS}
sed -i -e 's/\${MAKE}/\${MAKE} -j '${UNIBOT_CPU_COUNT}'/' Makefile

if [ $UNIBOT_USE_CLANG == 1 ]; then
	# this seems to be the most portable way of insert chars in front of a file
	# note that "sed -i "1i ..." file" is not portable!
	mv Makefile Makefile.tmp
	echo "export CC=clang" > Makefile
	echo "export CXX=clang++" >> Makefile
	cat Makefile.tmp >> Makefile
	rm Makefile.tmp
fi

chmod +x build_version.sh

echo ""
echo "###################################################"
echo "# NOTE: use '"${UNIBOT_MAKE}" config=release"${UNIBOT_MAKE_PLATFORM}"' to build unibot"
echo "###################################################"
echo ""
