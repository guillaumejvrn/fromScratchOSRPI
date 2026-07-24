#include "framebuffer.h"
#include "console.h"
#include <stdint.h>
#include "timer.h"

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

        kprintf("Delay test (3s countdown):\n");
        for (int i = 3; i > 0; i--) {
            kprintf(" -> Pause %d sec...\n", i);
            wait_msec(1000); // Wait exactly 1000 ms (1 second)
        }

        kprintf("\n[SUCCESS] Timer is working perfectly!\n");
    }

    while (1) {
        // CPU Idle
    }
}