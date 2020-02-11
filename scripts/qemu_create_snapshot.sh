#!/bin/sh
qemu-img create -f qcow2 -b $PWD/vms/clean/win10.qcow2 $PWD/vms/win10_snapshot.qcow2
