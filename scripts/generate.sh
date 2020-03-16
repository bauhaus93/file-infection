#!/bin/sh

TARGET_DIR="$PWD/build_mingw"
CMAKE="i686-w64-mingw32.static-cmake"

rm -rf $TARGET_DIR && \
mkdir -p $TARGET_DIR && \
export PATH=$PWD/mxe/usr/bin:$PATH && \
$CMAKE -B $TARGET_DIR \ -S . -DCMAKE_BUILD_TYPE="Release"
