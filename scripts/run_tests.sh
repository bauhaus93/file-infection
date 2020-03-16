#!/bin/sh

pushd build_native && \
make -j8 disasm && \
ctest -V --output-on-failure -j1
popd
