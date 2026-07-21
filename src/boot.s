.section ".text.boot"
.global _start

_start:
    // Lire l'ID du cœur processeur (MPIDR_EL1)
    mrs     x0, mpidr_el1
    and     x0, x0, #3
    // Seul le Cœur 0 continue, les cœurs 1, 2 et 3 sont mis en attente
    cbz     x0, master

hang:
    wfe
    b       hang

master:
    // Placer le pointeur de pile (Stack Pointer) sous l'adresse 0x80000
    // Ainsi la pile grandit vers le bas (0x7FFFF, etc.) sans écraser le code à 0x80000
    mov     x0, #0x80000
    mov     sp, x0

    // Nettoyer la section .BSS pour réinitialiser les variables globales
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
1:  cbz     w2, 2f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 1b

2:
    // Saut vers la fonction main() en C
    bl      main
    b       hang