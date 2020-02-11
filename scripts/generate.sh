#!/bin/sh

rm -rf release && \
mkdir -p release && \
export PATH=$PWD/mxe/usr/bin:$PATH && \
i686-w64-mingw32.static-cmake -B release \ -S . -DCMAKE_BUILD_TYPE="Release"
