.section ".text.boot"

.global _start

_start:
    // 1. Lire l'ID du cœur CPU (MPIDR_EL1)
    mrs     x0, mpidr_el1
    and     x0, x0, #3

    // Park les cœurs 1, 2, 3 (Seul le cœur 0 démarre)
    cbz     x0, master

hang:
    wfe
    b       hang

master:
    // 2. Vérifier si on est en EL2
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    cmp     x0, #2
    b.ne    setup_el1

    // 3. Configuration pour descendre de EL2 vers EL1
    // Activer l'exécution 64-bit pour EL1
    mov     x0, #(1 << 31)          // Bit 31 (RW) = 1
    msr     hcr_el2, x0

    // Définir l'état du processeur au retour en EL1 (EL1h + masquage des interruptions)
    mov     x0, #0x3c5             // DAIF masqués, SPSR.M = EL1h
    msr     spsr_el2, x0

    // Définir le point d'atterrissage du saut (label setup_el1)
    adr     x0, setup_el1
    msr     elr_el2, x0
    eret                           // Effectue le saut atomique vers EL1 !

setup_el1:
    // 4. Initialiser le Pointeur de Pile SP_EL1 sous 0x80000
    mov     x0, #0x80000
    mov     sp, x0

    // 5. Activer le coprocesseur FPU / NEON en EL1
    mov     x0, #(3 << 20)          // CPACR_EL1.FPEN = 0b11
    msr     cpacr_el1, x0
    isb

    // 6. Enregistrer la Vector Table dans VBAR_EL1
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0

    // 7. Nettoyer la section .BSS
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

1:
    cbz     w2, 2f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 1b

2:
    // 8. Sauter vers main()
    bl      main
    b       hang
    