#!/bin/sh

SHARED_DIR="$PWD/shared"
BUILD_ROOT="$PWD/build-mxe"
BUILD_DIR="$BUILD_ROOT/build"
MXE_DIR="$BUILD_ROOT/mxe"
MXE_TARGETS="i686-w64-mingw32.static"
CMAKE="i686-w64-mingw32.static-cmake"
TEST_TARGET_DIR="$PWD/resources/executables"

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

export PATH=$MXE_DIR/usr/bin:$PATH

if [ $MODE = "SETUP" ]
then
	rm -rf $BUILD_DIR && \
	mkdir -p $BUILD_DIR

	if [ ! -d $MXE_DIR ]
	then
		git clone https://github.com/mxe/mxe.git $MXE_DIR || \
		exit 1
	else
		pushd $MXE_DIR && \
		git pull --ff-only && \
		popd || \
		exit 1
	fi
	pushd $MXE_DIR && \
	make -j8 MXE_TARGETS="$MXE_TARGETS" gcc cmake && \
	popd && \
	$CMAKE -B $BUILD_DIR -S $PWD -DCMAKE_BUILD_TYPE="Release" -DBUILD_TESTING="ON"
fi

if [ $MODE = "BUILD" ] || [ $MODE = "TEST" ]
then
	$CMAKE --build $BUILD_DIR -j8 --target infect || \
	exit 1
fi

if [ $MODE = "TEST" ]
then
	mkdir -pv $SHARED_DIR && \
	rm -v $SHARED_DIR/* && \
	cp -v $TEST_TARGET_DIR/*.exe $SHARED_DIR/ && \
	cp -v "$BUILD_DIR/infect.exe" $SHARED_DIR
	cp -v "$BUILD_DIR/tests/test_relocation.exe" $SHARED_DIR
fi
