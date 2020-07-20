#!/bin/sh

SHARED_DIR="$PWD/shared"
VM_DIR="$PWD/resources/vms"


MODE="-snapshot"
while getopts "sb" option
do
	case "$option"
	in
		b) MODE="";;
		s) MODE="-snapshot";;
	esac
done

mkdir -p $SHARED_DIR && \
qemu-system-x86_64 \
    -m 4096 \
    --smp cpus=4 -enable-kvm \
    -cpu host \
    -drive file=$VM_DIR/clean/win10.qcow2,format=qcow2, \
    -nic user,smb=$SHARED_DIR \
	${MODE}
