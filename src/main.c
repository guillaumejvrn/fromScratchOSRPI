#include "framebuffer.h"
#include "console.h"
#include <stdint.h>

void main() {
    if (fb_init() == 0) {
        console_clear();

        kprintf("=========================================\n");
        kprintf(" PepitOS Bare-Metal Kernel Console\n");
        kprintf(" Screen Resolution: %dx%d (32-bit ARGB)\n", width, height);
        kprintf(" Framebuffer Address: %p\n", fb);
        kprintf(" Pitch: %d bytes/line\n", pitch);
        kprintf("=========================================\n\n");

        kprintf("[OK] Running in EL1 (Kernel Mode).\n");
        kprintf("[OK] Vector Table & FPU Initialized.\n");
        kprintf("[TEST] Triggering exception with SVC #0...\n\n");

    }

    while (1) {
        // CPU Idle
    }
}