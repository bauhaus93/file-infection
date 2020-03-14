#!/bin/sh

export PATH=$PWD/mxe/usr/bin:$PATH && \
cmake --build release -j8 && \
pushd release && \
ctest --output-on-failure -j8 && \
popd
