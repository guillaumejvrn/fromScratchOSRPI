.section ".text.boot"
.global _start

_start:
    // Read the processor core ID (MPIDR_EL1)
    mrs     x0, mpidr_el1
    and     x0, x0, #3
    // Only Core 0 continues; cores 1, 2, and 3 are parked
    cbz     x0, master

hang:
    wfe
    b       hang

master:
    // Set the stack pointer below address 0x80000
    // So the stack grows downward (0x7FFFF, etc.) without overwriting code at 0x80000
    mov     x0, #0x80000
    mov     sp, x0

    // Clear the .BSS section to reset global variables
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
1:  cbz     w2, 2f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 1b

2:
    // Jump to the C main() function
    bl      main
    b       hang
