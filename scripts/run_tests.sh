#!/bin/sh

export PATH=$PWD/mxe/usr/bin:$PATH && \
i686-w64-mingw32.static-cmake --build release -j8 && \
pushd release && \
ctest --output-on-failure -j8 && \
popd
