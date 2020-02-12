#!/bin/sh

rm -rf debug && \
mkdir -p debug && \
export PATH=$PWD/mxe/usr/bin:$PATH && \
i686-w64-mingw32.static-cmake -B debug \ -S . -DCMAKE_BUILD_TYPE="Debug"
