.section ".text.boot"

.global _start

_start:
    // 1. Read the CPU core ID (MPIDR_EL1)
    mrs     x0, mpidr_el1
    and     x0, x0, #3

    // Park cores 1, 2, and 3 (only core 0 starts)
    cbz     x0, master

hang:
    wfe
    b       hang

master:
    // 2. Check whether we are in EL2
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    cmp     x0, #2
    b.ne    setup_el1

    // 3. Configure transition from EL2 down to EL1
    // Enable 64-bit execution for EL1
    mov     x0, #(1 << 31)          // Bit 31 (RW) = 1
    msr     hcr_el2, x0

    // Set the processor state for return to EL1 (EL1h + interrupts masked)
    mov     x0, #0x3c5             // DAIF masked, SPSR.M = EL1h
    msr     spsr_el2, x0

    // Set the return address for the jump (label setup_el1)
    adr     x0, setup_el1
    msr     elr_el2, x0
    eret                           // Perform the atomic jump to EL1!

setup_el1:
    // 4. Initialize SP_EL1 stack pointer below 0x80000
    mov     x0, #0x80000
    mov     sp, x0

    // 5. Enable the FPU / NEON coprocessor in EL1
    mov     x0, #(3 << 20)          // CPACR_EL1.FPEN = 0b11
    msr     cpacr_el1, x0
    isb

    // 6. Set the Vector Table base in VBAR_EL1
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0

    // 7. Clear the .BSS section
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size

1:
    cbz     w2, 2f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 1b

2:
    // 8. Jump to main()
    bl      main
    b       hang
