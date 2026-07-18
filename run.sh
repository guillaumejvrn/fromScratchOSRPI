#!/bin/bash 

# Clear previous compilation objects and rebuild the kernel image 
make clean 
make 

# Boot QEMU with a graphical window (HDMI emulation)
qemu-system-aarch64 -M raspi4b -kernel kernel8.img