#!/bin/sh

echo "### Prepare testing directory ###"
rm -rfv $PWD/shared/target && \
mkdir -pv $PWD/shared/target && \
cp -v $PWD/executables/*.exe $PWD/shared/target && \
cp -v $PWD/release/infect.exe $PWD/shared/target
