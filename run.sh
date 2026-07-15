#!/bin/bash

# Clear previous compilation objects and rebuild the kernel image
make clean
make

# Boot QEMU mimicking a genuine Raspberry Pi 4 system in 64-bit mode
qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial stdio