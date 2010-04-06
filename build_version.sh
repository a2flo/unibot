#!/bin/sh

# determine the number of source files that have a timestamp newer than build_version.h
new_source_count=`find . -type f \( -name '*.h' -o -name '*.cpp' \) -newer build_version.h | wc -l | tr -cd [:digit:]`

if test ${new_source_count} -gt 0; then
	# if this is > 0, increase build version
	cat build_version | awk '{system("echo `expr " $1 " + 1` > build_version");}'
	cat build_version | awk '{system("echo \"#define UNIBOT_BUILD_VERSION " $1 "\" > build_version.h");}'

	build_version=`cat build_version`
	echo "# increased build version to "${build_version}
fi;
