.section ".text.boot"

.global _start

_start:
    // Read processor core ID from MPIDR_EL1 register
    mrs     x0, mpidr_el1
    and     x0, x0, #3
    // If core ID is 0 jump to master initialization
    cbz     x0, master
    
hang: 
    // Cores 1, 2, and 3 wait here indefinitely in low-power state
    wfe
    b       hang

master:
    // Configure stack on 0x80000
    mov     x0, #0x80000
    mov     sp, x0
    
    // Jump to the main C function entry point
    bl      main
    b       hang