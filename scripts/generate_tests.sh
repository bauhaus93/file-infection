#!/bin/sh

rm -rf build_native && \
mkdir -p build_native && \
cmake -B build_native \ -S . -DCMAKE_BUILD_TYPE="Release"
