#!/bin/sh

UNIBOT_OS="unknown"
UNIBOT_PLATFORM="x32"
UNIBOT_MAKE="make"
UNIBOT_MAKE_PLATFORM="32"
UNIBOT_ARGS=""

case $( uname | tr [:upper:] [:lower:] ) in
	"Darwin")
		UNIBOT_OS="macosx"
		;;
	"Linux")
		UNIBOT_OS="linux"
		;;
	[a-zA-Z0-9]*"BSD")
		UNIBOT_OS="bsd"
		UNIBOT_MAKE="gmake"
		;;
	"cygwin"* | "mingw"*)
		UNIBOT_OS="windows"
		UNIBOT_ARGS="cygwin"
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

chmod +x build_version.sh

echo ""
echo "###################################################"
echo "# NOTE: use '"${UNIBOT_MAKE}" config=release"${UNIBOT_MAKE_PLATFORM}"' to build unibot"
echo "###################################################"
echo ""
