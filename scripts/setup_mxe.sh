#!/bin/sh

git clone https://github.com/mxe/mxe.git && \
cd mxe && \
make -j8 MXE_TARGETS='i686-w64-mingw32.static' gcc cmake
