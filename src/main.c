#include "framebuffer.h"
#include "console.h"
#include "timer.h"
#include "kheap.h"
#include "pcie.h"
#include "xhci.h"
#include "uart.h"
#include "arch/io.h"
#include <stdint.h>

static xhci_controller_t __attribute__((aligned(16))) usb_controller;

void main() {
    kheap_init();

    if (fb_init() == 0) {
        console_clear();
        uart_init();

        kprintf("=========================================\n");
        kprintf(" PepitOS Bare-Metal Kernel Console\n");
        kprintf(" Screen Resolution: %dx%d (32-bit ARGB)\n", width, height);
        kprintf(" Framebuffer Address: %p\n", fb);
        kprintf(" Pitch: %d bytes/line\n", pitch);
        kprintf("=========================================\n\n");

        pci_device_t usb_dev;
        pcie_init(&usb_dev);

        if (usb_dev.bar0 != 0) {
            if (xhci_init(&usb_controller, usb_dev.bar0) == 0) {
                uint8_t slot_id = 0;
                if (xhci_enable_slot(&usb_controller, &slot_id) == 0 && slot_id > 0) {
                    kprintf("[USB] Slot ID %d attribue au clavier.\n", slot_id);
                    xhci_setup_keyboard(&usb_controller, slot_id);
                    kprintf("[OK] Clavier USB pret ! Tapez vos touches :\n\n> ");
                } else {
                    kprintf("[WARN] Impossible d'activer le slot USB.\n");
                }
            }
        }
    }

    while (1) {
        xhci_poll_keyboard(&usb_controller);
        dsb();
    }
}