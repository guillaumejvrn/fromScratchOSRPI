#include "exception.h"
#include "console.h"

void handle_sync_exception(register_frame_t *frame) {
    uint64_t esr, elr, far;

    // Read system registers containing fault details
    asm volatile("mrs %0, esr_el1" : "=r"(esr)); // Exception Syndrome Register
    asm volatile("mrs %0, elr_el1" : "=r"(elr)); // Exception Link Register (Faulting instruction)
    asm volatile("mrs %0, far_el1" : "=r"(far)); // Fault Address Register

    kprintf("\n");
    kprintf("==================================================\n");
    kprintf("                !!! KERNEL PANIC !!!              \n");
    kprintf("==================================================\n");
    kprintf(" Synchronous Exception Captured!\n\n");
    kprintf(" Fault Instruction (ELR_EL1) : %p\n", elr);
    kprintf(" Fault Address     (FAR_EL1) : %p\n", far);
    kprintf(" Exception Syndrome(ESR_EL1) : %p\n\n", esr);

    kprintf(" Register Dump:\n");
    for (int i = 0; i < 31; i += 2) {
        if (i == 30) {
            kprintf("  X30 (LR) : %p\n", frame->x[30]);
        } else {
            kprintf("  X%d : %p \t X%d : %p\n", i, frame->x[i], i + 1, frame->x[i + 1]);
        }
    }
    kprintf("==================================================\n");
    kprintf(" System Halted.\n");
}

void handle_invalid_exception(register_frame_t *frame) {
    (void)frame;
    kprintf("\n[KERNEL PANIC] Unhandled Exception Triggered!\n");
}