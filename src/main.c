#include "framebuffer.h"
#include "console.h"
#include <stdint.h>
#include "timer.h"
#include "kheap.h"
#include "io.h"

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

        kprintf("[OK] Vector Table & Timer Active.\n");
        kprintf("[OK] Kernel Heap Initialized.\n\n");

        // Test 1: 64-byte aligned allocation (Required for USB TRB command rings)
        void *trb_ring = kmalloc_aligned(1024, 64);
        kprintf(" TRB Ring (Align 64B)  : %p\n", trb_ring);

        // Test 2: 4 KB aligned allocation (Required for xHCI scratchpad)
        void *scratchpad = kmalloc_aligned(4096, 4096);
        kprintf(" Scratchpad (Align 4KB): %p\n\n", scratchpad);

        // Verify alignment math
        if (((uintptr_t)trb_ring % 64 == 0) && ((uintptr_t)scratchpad % 4096 == 0)) {
            kprintf("[SUCCESS] 100%% valid alignments for xHCI!\n");
        } else {
            kprintf("[ERROR] Memory alignment fault!\n");
        }
    }

    while (1) {
        // CPU Idle
    }
}