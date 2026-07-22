# Custom 64-bit Bare-Metal Operating System / 64ビットベアメタルOS開発

<div align="center">

**English** | **日本語**

Bare-metal AArch64 Operating System for the **Raspberry Pi 4 (BCM2711)**
Youtube video : available soon

</div>

---

# 🇬🇧 English

## Overview

This project is a **64-bit (AArch64) bare-metal operating system kernel** developed from scratch in **C** and **ARM64 Assembly** for the **Broadcom BCM2711 SoC (Raspberry Pi 4 Model B)**.

The project focuses on low-level systems programming, including:

- Bare-metal hardware programming
- ARM64 boot process
- VideoCore GPU Mailbox communication
- Framebuffer graphics rendering
- Freestanding development (`-nostdlib`, `-ffreestanding`)

---

## Project Structure

```text
.
├── Makefile
├── link.ld
├── build.sh
├── run.sh
├── boot_files/
│   ├── config.txt
│   ├── bcm2711-rpi-4-b.dtb
│   └── Raspberry Pi firmware files
└── src/
    ├── boot.s
    ├── main.c
    └── drivers/
        └── framebuffer.c
```

### File Description

| File | Description |
|------|-------------|
| `boot.s` | ARM64 entry point, stack initialization, core filtering, BSS clearing |
| `main.c` | Kernel entry and graphics demo |
| `framebuffer.c` | BCM2711 Mailbox framebuffer driver |
| `link.ld` | Kernel linker script |
| `Makefile` | Build configuration |

---

## Prerequisites

### Cross Compiler

- `aarch64-elf-gcc`
- `aarch64-elf-ld`
- `aarch64-elf-objcopy`

or

- `aarch64-linux-gnu-*`

### Emulator

```text
qemu-system-aarch64
```

### Build Tools

```text
make
bash
```

---

# Building

Make the helper scripts executable:

```bash
chmod +x build.sh run.sh
```

Build the project:

```bash
./build.sh
```

or manually:

```bash
make clean
make
```

---

# Running with QEMU

```bash
./run.sh
```

---

# Deploying to Raspberry Pi 4

1. Create a **FAT32** partition (~1 GB) on a MicroSD card or USB drive.

2. Copy the firmware files from `boot_files/`:

```text
start4.elf
fixup4.dat
bcm2711-rpi-4-b.dtb
config.txt
```

3. Copy the generated kernel:

```text
kernel8.img
```

4. Insert the storage device into the Raspberry Pi 4 and boot with a display connected to **HDMI0**.

---

# Current Features

## ✅ Implemented

### Bootloader (`boot.s`)

- ARM64 entry point
- Core 0 execution only
- Secondary cores parked with `wfe`
- Stack initialization
- `.bss` zero initialization

---

### Linker Script (`link.ld`)

- Kernel load address at:

```text
0x80000
```

---

### Mailbox Driver

- BCM2711 Property Mailbox
- MMIO communication
- Channel 8 support

---

### Framebuffer Driver

- Graphics initialization
- 1024 × 768
- 32-bit ARGB framebuffer
- Pixel drawing primitives

---

# Roadmap

## 📋 Planned Features

### 1. Interrupts & Exceptions

- ARM64 Vector Table
- IRQ handling
- Exception handlers
- Timer interrupts

---

### 2. Keyboard Driver

- UART input
- USB keyboard support
- Interactive shell
- Simple games

---

### 3. Dynamic Memory Allocation

- Heap allocator
- Free-list memory manager
- `malloc()`
- `free()`

---

### 4. MMU & Virtual Memory

- Translation tables
- Virtual memory
- Cache enable
- Memory protection

---

### 5. Multitasking

- PCB
- Context switching
- Round-Robin scheduler

---

### 6. Networking (Optional)

- Network driver
- Basic network stack

---

---

# 🇯🇵 日本語

## 概要

本プロジェクトは **Broadcom BCM2711 (Raspberry Pi 4 Model B)** 向けに、**C言語**および**ARM64アセンブリ**でゼロから開発している **64ビット (AArch64) ベアメタルOSカーネル**です。

標準Cライブラリを使用せず (`-nostdlib`, `-ffreestanding`)、以下の低レベル技術を学習・実装しています。

- ベアメタルプログラミング
- ARM64ブート処理
- VideoCore GPU Mailbox通信
- Framebufferグラフィックス
- ハードウェア直接制御

---

## プロジェクト構造

```text
.
├── Makefile
├── link.ld
├── build.sh
├── run.sh
├── boot_files/
│   ├── config.txt
│   ├── bcm2711-rpi-4-b.dtb
│   └── Raspberry Pi ファームウェア
└── src/
    ├── boot.s
    ├── main.c
    └── drivers/
        └── framebuffer.c
```

---

## 必要なツール

### クロスコンパイラ

```text
aarch64-elf-gcc
aarch64-elf-ld
aarch64-elf-objcopy
```

または

```text
aarch64-linux-gnu-*
```

### エミュレータ

```text
qemu-system-aarch64
```

### ビルドツール

```text
make
bash
```

---

# ビルド

```bash
chmod +x build.sh run.sh
./build.sh
```

または

```bash
make clean
make
```

---

# QEMUで実行

```bash
./run.sh
```

---

# Raspberry Pi 4への書き込み

1. 約1GBのFAT32パーティションを作成します。

2. `boot_files/` の以下のファイルをコピーします。

```text
start4.elf
fixup4.dat
bcm2711-rpi-4-b.dtb
config.txt
```

3. 生成された

```text
kernel8.img
```

をルートディレクトリへコピーします。

4. Raspberry Pi 4へ挿入し、HDMI0へディスプレイを接続して起動します。

---

# 実装済み

## ✅ ブートローダー

- ARM64エントリポイント
- Core0のみ実行
- Core1〜3を`wfe`で待機
- スタック初期化
- `.bss`ゼロクリア

---

## ✅ リンカスクリプト

- カーネルロードアドレス

```text
0x80000
```

---

## ✅ Mailboxドライバ

- BCM2711 Property Mailbox
- MMIO通信
- Channel 8

---

## ✅ Framebufferドライバ

- 1024×768
- 32-bit ARGB
- ピクセル描画

---

# 今後の実装予定

## 📋 ロードマップ

### 1. 割り込み・例外処理

- Vector Table
- IRQ
- Timer

### 2. キーボードドライバ

- UART
- USB
- シェル
- ミニゲーム

### 3. 動的メモリ管理

- Heap
- Free List
- malloc
- free

### 4. MMU

- 仮想メモリ
- Translation Table
- キャッシュ有効化

### 5. マルチタスク

- PCB
- コンテキストスイッチ
- Round-Robinスケジューラ

### 6. ネットワーク（任意）

- ドライバ
- ネットワークスタック
