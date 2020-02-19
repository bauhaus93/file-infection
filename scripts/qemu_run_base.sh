#!/bin/sh

SHARED_DIR="$PWD/shared"

mkdir -p $SHARED_DIR && \
qemu-system-x86_64 \
    -m 4096 \
    --smp cpus=4 -enable-kvm \
    -cpu host \
    -drive file=$PWD/vms/clean/win10.qcow2,format=qcow2, \
    -nic user,smb=$SHARED_DIR
