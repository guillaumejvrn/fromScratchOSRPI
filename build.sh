#!/bin/bash
set -e

echo "🧹 Nettoyage..."
make clean

echo "🔨 Compilation de PepitOS..."
make

echo "✅ Succès ! Le binaire kernel8.img est prêt."