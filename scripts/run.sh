#!/bin/sh

scripts/build.sh && \
cp release/infect.exe shared/ && \
scripts/qemu_run_snapshot.sh
