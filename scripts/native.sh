#!/bin/sh

BUILD_DIR="$PWD/build-native"
CMAKE="cmake"

MODE="BUILD"
while getopts "sbt" option
do
	case "$option"
	in
		s) MODE="SETUP";;
		b) MODE="BUILD";;
		t) MODE="TEST";;
	esac
done

if [ $MODE = "SETUP" ]
then
	rm -rf $BUILD_DIR && \
	mkdir -p $BUILD_DIR && \
	$CMAKE -B $BUILD_DIR -S $PWD -DCMAKE_BUILD_TYPE="Release" -DBUILD_TESTING="ON"
fi

if [ $MODE = "BUILD" ] || [ $MODE = "TEST" ]
then
	$CMAKE --build $BUILD_DIR -j8 --target disasm
fi

if [ $MODE = "TEST" ]
then
	pushd $BUILD_DIR
	ctest  --output-on-failure -j8
	popd
fi
