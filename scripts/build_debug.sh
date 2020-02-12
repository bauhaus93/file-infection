#!/bin/sh

export PATH=$PWD/mxe/usr/bin:$PATH && \
i686-w64-mingw32.static-cmake --build debug -j8
