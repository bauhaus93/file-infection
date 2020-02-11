#!/bin/sh
qemu-system-x86_64 -m 4096 -drive file=$PWD/vms/win10_snapshot.qcow2,format=qcow2 --smp cpus=4 -enable-kvm -nic none
