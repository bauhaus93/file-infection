#!/bin/sh

SHARED_DIR="$PWD/shared"

rm -rf $SHARED_DIR && \
mkdir -p $SHARED_DIR && \
qemu-system-x86_64 \
    -m 4096 \
    --smp cpus=4 -enable-kvm \
    -drive file=$PWD/vms/win10_snapshot.qcow2,format=qcow2 \
    -net user,smb=$SHARED_DIR \
    -net nic,model=virtio

