#!/bin/bash
set -e

# Recompile le noyau
sh build.sh

# Lance la simulation QEMU de la Raspberry Pi 4 avec affichage graphique
qemu-system-aarch64 -M raspi4b -kernel kernel8.img