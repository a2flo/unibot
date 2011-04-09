#!/bin/sh

UNIBOT_OS="unknown"
UNIBOT_PLATFORM="x32"
UNIBOT_MAKE="make"
UNIBOT_MAKE_PLATFORM="32"
UNIBOT_ARGS=""
UNIBOT_CPU_COUNT=1

case $( uname | tr [:upper:] [:lower:] ) in
	"darwin")
		UNIBOT_OS="macosx"
		UNIBOT_CPU_COUNT=$(sysctl -a | grep 'machdep.cpu.thread_count' | sed -E 's/.*(: )([:digit:]*)/\2/g')
		;;
	"linux")
		UNIBOT_OS="linux"
		UNIBOT_CPU_COUNT=$(cat /proc/cpuinfo | grep -m 1 'cpu cores' | sed -E 's/.*(: )([:digit:]*)/\2/g')
		;;
	[a-z0-9]*"bsd")
		UNIBOT_OS="bsd"
		UNIBOT_MAKE="gmake"
		# TODO: get cpu/thread count on *bsd
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

chmod +x build_version.sh

echo ""
echo "###################################################"
echo "# NOTE: use '"${UNIBOT_MAKE}" config=release"${UNIBOT_MAKE_PLATFORM}"' to build unibot"
echo "###################################################"
echo ""
