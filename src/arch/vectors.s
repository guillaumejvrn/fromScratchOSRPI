.section ".text"

// Macro to save general-purpose registers (x0-x30) onto the stack
.macro save_registers
    sub     sp, sp, #256
    stp     x0, x1, [sp, #16 * 0]
    stp     x2, x3, [sp, #16 * 1]
    stp     x4, x5, [sp, #16 * 2]
    stp     x6, x7, [sp, #16 * 3]
    stp     x8, x9, [sp, #16 * 4]
    stp     x10, x11, [sp, #16 * 5]
    stp     x12, x13, [sp, #16 * 6]
    stp     x14, x15, [sp, #16 * 7]
    stp     x16, x17, [sp, #16 * 8]
    stp     x18, x19, [sp, #16 * 9]
    stp     x20, x21, [sp, #16 * 10]
    stp     x22, x23, [sp, #16 * 11]
    stp     x24, x25, [sp, #16 * 12]
    stp     x26, x27, [sp, #16 * 13]
    stp     x28, x29, [sp, #16 * 14]
    str     x30, [sp, #16 * 15]
.endm

// Macro for 128-byte vector entries
.macro ventry label
    .align 7
    b \label
.endm

// Vector Table must be 2048-byte aligned for VBAR_EL1
.align 11
.global exception_vector_table
exception_vector_table:
    // Current EL with SP0
    ventry sync_invalid
    ventry irq_invalid
    ventry fiq_invalid
    ventry serror_invalid

    // Current EL with SPx
    ventry sync_handler
    ventry irq_invalid
    ventry fiq_invalid
    ventry serror_invalid

    // Lower EL using AArch64
    ventry sync_invalid
    ventry irq_invalid
    ventry fiq_invalid
    ventry serror_invalid

    // Lower EL using AArch32
    ventry sync_invalid
    ventry irq_invalid
    ventry fiq_invalid
    ventry serror_invalid

sync_handler:
    save_registers
    mov     x0, sp                  // Pass stack pointer as first argument to C
    bl      handle_sync_exception
hang_loop:
    wfe
    b       hang_loop

sync_invalid:
irq_invalid:
fiq_invalid:
serror_invalid:
    save_registers
    mov     x0, sp
    bl      handle_invalid_exception
    b       hang_loop