# PepitOS 🍓💻
> **Bare-Metal 64-bit (AArch64) Operating System from Scratch for Raspberry Pi 4**

PepitOS est un noyau d'système d'exploitation *bare-metal* développé à partir de zéro en C (C11) et Assembleur ARM64 pour le SoC Broadcom BCM2711 (Raspberry Pi 4 Model B).

Ce projet explore la programmation système bas niveau, la manipulation directe de registres matériels (MMIO), le protocole d'IPC Mailbox avec le GPU VideoCore et le rendu graphique sur Framebuffer HDMI sans bibliothèque C standard (`-nostdlib`, `-freestanding`).

---

## 📋 Table des matières

- [Fonctionnalités & Points forts](#-fonctionnalités--points-forts)
- [Aperçu Matériel & Architecture](#-aperçu-matériel--architecture)
- [Structure du Projet](#-structure-du-projet)
- [Outils & Prérequis](#-outils--prérequis)
- [Compilation & Exécution](#-compilation--exécution)
  - [1. Compilation](#1-compilation)
  - [2. Test sous QEMU](#2-test-sous-qemu)
  - [3. Déploiement sur Raspberry Pi 4 réel](#3-déploiement-sur-raspberry-pi-4-réel)
- [Détails Techniques & Séquence de Boot](#-détails-techniques--séquence-de-boot)
  - [Séquence de Démarrage (`src/boot.s`)](#séquence-de-démarrage-srcboots)
  - [Mailbox VideoCore & Framebuffer (`src/drivers/framebuffer.c`)](#mailbox-videocore--framebuffer-srcdriversframebufferc)
  - [Différence de couleurs : QEMU vs Vrai Matériel](#différence-de-couleurs--qemu-vs-vrai-matériel)
- [Roadmap & Prochaines étapes](#-roadmap--prochaines-étapes)

---

## ✨ Fonctionnalités & Points forts

- **Architecture cible** : AArch64 (ARMv8-A, mode 64 bits).
- **Gestion des cœurs CPU** : Initialisation du Cœur 0 et mise en veille basse consommation (`wfe`) des Cœurs 1 à 3.
- **Configuration Mémoire** : Script de liaison personnalisé (`link.ld`) plaçant le point d'entrée à `0x80000`, paramétrant la pile (Stack Pointer) sous `0x80000` et nettoyant la section `.bss`.
- **Communication Inter-Processeurs (IPC)** : Protocole de tags de propriété VideoCore Mailbox sur le canal 8.
- **Moteur Graphique Framebuffer** : Affichage 1024x768 @ 32-bit ARGB avec effacement de l'écran et dessin pixel par pixel.

---

## 🔬 Aperçu Matériel & Architecture

| Composant | Spécification |
| :--- | :--- |
| **Carte cible** | Raspberry Pi 4 Model B |
| **SoC** | Broadcom BCM2711 (Quad-core Cortex-A72 @ 1.5GHz) |
| **Architecture** | AArch64 / ARMv8-A |
| **Adresse Base MMIO** | `0xFE000000` |
| **Adresse Registres Mailbox** | `0xFE00B880` (`PERIPHERAL_BASE + 0x0000B880`) |
| **Alias Bus VideoCore** | `0xC0000000` (Masque d'accès RAM non-cachée par le GPU) |
| **Masque d'Adresse ARM** | `0x3FFFFFFF` (Convertit l'adresse bus GPU en adresse physique ARM) |

---

## 📁 Structure du Projet

```text
fromScratchOSRPI/
├── Makefile              # Système de build générant kernel8.img
├── link.ld               # Script de liaison AArch64 (adresse 0x80000)
├── build.sh              # Script de compilation propre (make clean && make)
├── run.sh                # Script de compilation et lancement sous QEMU
├── README.md             # Documentation du projet
├── boot_files/           # Firmwares Broadcom & Device Tree pour Pi 4
│   ├── bcm2711-rpi-4-b.dtb # Fichier DTB (requis pour init HDMI & PMIC)
│   ├── config.txt        # Configuration de boot (arm_64bit=1, hdmi_force_hotplug)
│   ├── fixup4.dat        # Table de fixup mémoire SDRAM du VideoCore
│   └── start4.elf        # Firmware GPU Broadcom VideoCore VI
└── src/                  # Code source du noyau
    ├── boot.s            # Point d'entrée assembleur AArch64
    ├── main.c            # Point d'entrée C & boucle principale
    └── drivers/
        ├── framebuffer.h # En-tête des fonctions Framebuffer
        └── framebuffer.c # Pilote Mailbox BCM2711 et moteur de rendu
```

---

## 🛠️ Outils & Prérequis

Pour compiler et exécuter PepitOS, installez la chaîne de compilation croisée AArch64 et QEMU (macOS ou Linux) :

- **GCC Cross-Compiler** : `aarch64-elf-gcc`, `aarch64-elf-ld`, `aarch64-elf-objcopy`
- **Émulateur** : `qemu-system-aarch64`
- **Utilitaires** : `make`, `bash`

---

## 🚀 Compilation & Exécution

### 1. Compilation

Pour compiler le noyau et générer le binaire brut `kernel8.img` :

```bash
chmod +x build.sh run.sh
./build.sh
```

Ou manuellement :

```bash
make clean
make
```

---

### 2. Test sous QEMU

Pour exécuter le noyau dans l'émulateur QEMU Raspberry Pi 4 :

```bash
./run.sh
```

*(Commande exécutée : `qemu-system-aarch64 -M raspi4b -kernel kernel8.img`)*

---

### 3. Déploiement sur Raspberry Pi 4 réel

Pour démarrer sur la carte physique (carte MicroSD ou SSD USB) :

1. Formatez la partition de démarrage au format **FAT32**.
2. Copiez tous les fichiers du dossier `boot_files/` à la racine de la carte :
   - `start4.elf`
   - `fixup4.dat`
   - `bcm2711-rpi-4-b.dtb`
   - `config.txt`
3. Copiez le fichier **`kernel8.img`** généré à la racine de la carte.
4. Insérez la carte dans le Pi 4 et connectez un écran sur le port **HDMI 0**.

> ⚠️ **Note importante** : Le firmware VideoCore (`start4.elf`) exige impérativement la présence du fichier Device Tree `bcm2711-rpi-4-b.dtb` pour initialiser le contrôleur d'alimentation (PMIC), les horloges et la sortie HDMI avant de donner la main au processeur ARM64 à l'adresse `0x80000`.

---

## 🔍 Détails Techniques & Séquence de Boot

### Séquence de Démarrage (`src/boot.s`)

1. **Filtre Multi-cœur** : Lecture du registre `mpidr_el1`. Si l'ID du cœur est différent de `0`, le cœur est mis en boucle d'attente (`wfe`).
2. **Initialisation de la Pile** : Le pointeur de pile `sp` est fixé à `0x80000`. La pile grandit vers les adresses basses (`0x7FFFF...`), évitant ainsi d'écraser le code du noyau situé à `0x80000`.
3. **Nettoyage de la BSS** : Remise à zéro de la section `.bss` 8 octets par 8 octets (`str xzr`).
4. **Passage au C** : Appel de la fonction `main()` via `bl main`.

---

### Mailbox VideoCore & Framebuffer (`src/drivers/framebuffer.c`)

PepitOS communique avec le GPU via les tags de propriété sur le canal `8` du registre Mailbox MMIO.

1. **Préparation de la requête** : Création d'un buffer `mbox[36]` aligné sur 16 octets demandant une résolution physique et virtuelle de 1024x768 en 32-bit ARGB.
2. **Masquage d'adresse Bus** : Ajout du masque `0xC0000000` à l'adresse du buffer pour informer le GPU de lire en RAM non-cachée.
3. **Conversion d'adresse mémoire** : Le GPU renvoie l'adresse du Framebuffer. Application du masque `0x3FFFFFFF` pour obtenir l'adresse physique ARM accessible par le processeur.

---

### Différence de couleurs : QEMU vs Vrai Matériel

| Environnement | Rendu Visuel | Raison Technique |
| :--- | :--- | :--- |
| **Vrai Raspberry Pi 4** | Fond **bleu** + Carré blanc 200x200 au centre | Ordre natif des canaux de couleur ARGB VideoCore |
| **QEMU (`-M raspi4b`)** | Fond **rouge** + Carré blanc 200x200 au centre | Inversion des canaux Rouge/Bleu (RGB/BGR) dans le contrôleur d'affichage de QEMU |

---

## 🎯 Roadmap & Prochaines étapes

- [x] Initialisation bare-metal (`boot.s`) et environnement C.
- [x] Pilote Mailbox GPU BCM2711.
- [x] Allocation du Framebuffer et moteur de rendu graphique 32 bits.
- [ ] **Police Bitmap 8x8** : Intégration de la table de caractères ASCII.
- [ ] **Console de texte** : Fonctions `draw_char()`, `draw_string()`, gestion des saut de ligne `\n` et défilement de l'écran (*scroll*).
- [ ] **Pilote UART PL011** : Communication série pour le debug.
- [ ] **Gestion des Interruptions (GIC-400)** : Horloge système et timer matériel.
- [ ] **MMU (Memory Management Unit)** : Tables de pages et activation du cache CPU.
