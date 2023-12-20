#!/bin/bash

qemu-system-x86_64 -enable-kvm -m 2G \
    -kernel output/bzImage \
    -initrd output/initramfs.igz \
    -cpu host
    
    

