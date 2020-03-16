#!/bin/sh

TARGET_DIR="$PWD/build_native"
CMAKE="cmake"

rm -rf $TARGET_DIR && \
mkdir -p $TARGET_DIR && \
export PATH=$PWD/mxe/usr/bin:$PATH && \
$CMAKE -B $TARGET_DIR \ -S . -DCMAKE_BUILD_TYPE="Release"
