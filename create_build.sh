#!/bin/bash
rm -r build/
mkdir build
cp replace.sh build/replace.sh
(cd build/ && CC=/usr/bin/i686-w64-mingw32-gcc exec cmake ..)
