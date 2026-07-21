Pas de souci, voici le contenu brut exact du fichier `README.md` bilingue. Tu peux simplement copier-coller ce bloc dans ton éditeur (VS Code, Vim, etc.) et l'enregistrer sous le nom **`README.md`** à la racine de ton projet :

```markdown
# Custom 64-bit Bare-Metal Operating System / 64ビットベアメタルOS開発 🍓💻

[ English ] | [ 日本語 ]

---

## 🇬🇧 English

### Overview
A 64-bit (AArch64) bare-metal operating system kernel built from scratch in C and ARM64 Assembly for the Broadcom BCM2711 SoC (Raspberry Pi 4 Model B)[cite: 1, 3]. The project explores low-level system programming, bare-metal hardware interaction, Mailbox IPC protocols with the VideoCore GPU, and direct Framebuffer graphics rendering without standard C libraries (`-nostdlib`, `-freestanding`)[cite: 1, 3].

---

### Project Structure
```text
/
├── Makefile
├── link.ld
├── build.sh / run.sh
├── boot_files/           # Firmware, config.txt & bcm2711-rpi-4-b.dtb
└── src/
    ├── boot.s            # AArch64 entry point (stack, core 0 filter, BSS zeroing)
    ├── main.c            # Kernel main logic & graphics test
    └── drivers/
        └── framebuffer.c # BCM2711 Mailbox driver & pixel renderer

```

---

### Prerequisites & Toolchain

* **Cross-Compiler**: `aarch64-elf-gcc`, `aarch64-elf-ld`, `aarch64-elf-objcopy` (or `aarch64-linux-gnu-*`)


* **Emulator**: `qemu-system-aarch64`

* **Build Tools**: `make`, `bash`


---

### Building & Running

#### 1. Compilation

```bash
chmod +x build.sh run.sh
./build.sh
# or manually: make clean && make

```

#### 2. Running in QEMU

```bash
./run.sh

```

#### 3. Deployment on Physical Raspberry Pi 4

1. Format your MicroSD card or USB drive with a **primary FAT32 partition of approximately 1 GB**.


2. Copy all firmware files from `boot_files/` (`start4.elf`, `fixup4.dat`, `bcm2711-rpi-4-b.dtb`, `config.txt`) to the root of the partition.


3. Copy the compiled **`kernel8.img`** executable to the root.


4. Insert into the Raspberry Pi 4 and power on with an HDMI 0 display connected.



---

### Implementation Status & Technical Roadmap

#### ✅ Implemented

* **AArch64 Bare-Metal Bootloader (`boot.s`)**: Core 0 execution filtering, putting secondary cores (1–3) to sleep (`wfe`), stack setup below `0x80000`, and `.bss` section zero-initialization.


* **Linker Configuration (`link.ld`)**: Custom memory mapping setting kernel load address at `0x80000`.


* **BCM2711 VideoCore Mailbox Protocol**: Inter-Process Communication (IPC) via MMIO Property Mailbox Channel 8.


* **Framebuffer Driver (`framebuffer.c`)**: 1024x768 @ 32-bit ARGB graphic initialization and pixel drawing primitives.



#### 📋 To Be Implemented

1. **Hardware Interrupt & Exception Handling (Exceptions & IRQ)**

* Configure the AArch64 Vector Table in assembly to handle hardware exceptions and timer interrupts.




2. **Keyboard Driver (UART / USB)**

* Implement input processing via UART/USB to connect user input to graphic outputs (e.g., interactive text shell or a mini breakout game).




3. **Dynamic Memory Allocation (`malloc` / Heap Allocator)**

* Build a free-list heap memory manager to distribute system RAM dynamically for complex data structures.




4. **MMU & Virtual Memory Paging (Memory Management Unit)**

* Configure translation page tables, enable L1/L2 CPU caches cleanly, and isolate kernel memory space.




5. **Multitasking & Scheduler**

* Implement Process Control Blocks (PCB), CPU register context switching, and a Round-Robin task scheduler.




6. **Network Stack (Networking Layer - Optional)**

* Driver setup and network protocol processing if time permits.





---

## 🇯🇵 日本語

### 概要

Broadcom BCM2711 SoC (Raspberry Pi 4 Model B) 向けに、C言語およびARM64アセンブリでゼロから開発している64ビット (AArch64) ベアメタルOSカーネルです。標準Cライブラリに依存せず (`-nostdlib`, `-freestanding`)、ベアメタル環境におけるハードウェア制御、VideoCore GPUとのMailbox通信、およびFramebufferによるグラフィック描画を実装しています。

---

### プロジェクト構造

```text
/
├── Makefile
├── link.ld
├── build.sh / run.sh
├── boot_files/           # ファームウェア、config.txt、bcm2711-rpi-4-b.dtb
└── src/
    ├── boot.s            # AArch64 エントリポイント (スタック設定、Core 0 フィルタ、.bss初期化)
    ├── main.c            # カーネルメインロジック & 描画テスト
    └── drivers/
        └── framebuffer.c # BCM2711 Mailbox ドライバ & 描画処理

```

---

### 開発環境・必要ツール

* **クロスコンパイラ**: `aarch64-elf-gcc`, `aarch64-elf-ld`, `aarch64-elf-objcopy` (または `aarch64-linux-gnu-*`)


* **エミュレータ**: `qemu-system-aarch64`

* **ビルドツール**: `make`, `bash`


---

### ビルドと実行方法

#### 1. コンパイル

```bash
chmod +x build.sh run.sh
./build.sh
# 手動ビルド: make clean && make

```

#### 2. QEMUでの実行

```bash
./run.sh

```

#### 3. Raspberry Pi 4 実機へのデプロイ

1. MicroSDカードまたはUSBストレージ内に**約1GBのFAT32基本パーティション**を作成・フォーマットします。


2. `boot_files/` 内の全ファイル (`start4.elf`, `fixup4.dat`, `bcm2711-rpi-4-b.dtb`, `config.txt`) をパーティションのルートにコピーします。


3. 生成された **`kernel8.img`** をルートにコピーします。


4. Raspberry Pi 4に挿入し、HDMI 0ポートにディスプレイを接続して起動します。



---

### 実装状況とロードマップ

#### ✅ 実装済み機能

* **AArch64 ベアメタルブートローダー (`boot.s`)**: Core 0 のみの実行フィルタリング、サブコア (Core 1-3) の待機処理 (`wfe`)、`0x80000` 以下へのスタックポインタ設定、`.bss` セクションのゼロ初期化。


* **リンカスクリプト設定 (`link.ld`)**: カーネルの配置アドレスを `0x80000` に設定するメモリレイアウト定義。


* **BCM2711 VideoCore Mailbox プロトコル**: MMIOプロパティ Mailbox チャンネル8経由でのGPU通信。


* **フレームバッファードライバ (`framebuffer.c`)**: 1024x768 @ 32-bit ARGB グラフィック初期化およびピクセル描画処理。



#### 📋 今後の実装予定 (ロードマップ)

1. **ハードウェア割り込み & 例外処理 (Exceptions & IRQ)**

* ARM64 アセンブリでベクターテーブル (Vector Table) を構築し、ハードウェア例外やタイマー割り込みをキャッチ。




2. **キーボードドライバ (UART / USB)**

* UARTやUSB経由での入力処理を実装し、インタラクティブなテキストシェルやミニゲームの入力を構築。




3. **動的メモリ割り当て (`malloc` / ヒープアロケータ)**

* フリーリストに基づくヒープメモリ管理機構を実装し、動的なデータ構造をサポート。




4. **MMU & 仮想メモリページング (Memory Management Unit)**

* 変換テーブル (Translation Tables) を設定して仮想メモリを構築し、CPUキャッシュの有効化とカーネル空間の保護を実施。




5. **マルチタスク & スケジューラ (Multitasking & Scheduler)**

* プロセス制御ブロック (PCB) の作成、CPUレジスタのコンテキストスイッチ、ラウンドロビン・スケジューラの実装。




6. **ネットワークスルー (ネットワーク層 - 任意)**

* 時間的余裕がある場合、ネットワークドライバおよびプロトコル処理の追加。


