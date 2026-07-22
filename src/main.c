#include "framebuffer.h"
#include "console.h"

void main() {
    if (fb_init() == 0) {
        console_clear();

        // Print header info using kprintf
        kprintf("=========================================\n");
        kprintf(" PepitOS Bare-Metal Kernel Console\n");
        kprintf(" Screen Resolution: %dx%d (32-bit ARGB)\n", width, height);
        kprintf(" Framebuffer Address: %p\n", fb);
        kprintf(" Pitch: %d bytes/line\n", pitch);
        kprintf("=========================================\n\n");

    }

    while (1) {
        // CPU idle loop
    }
}