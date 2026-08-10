#include "xhci.h"
#include "console.h"
#include "kheap.h"
#include "timer.h"
#include "arch/io.h"

// Offsets des registres Opérationnels xHCI
#define XHCI_OP_USBCMD    0x00
#define XHCI_OP_USBSTS    0x04
#define XHCI_OP_CONFIG    0x38
#define XHCI_OP_DCBAAP    0x30
#define XHCI_OP_CRCR      0x18

// Bits USBCMD / USBSTS
#define USBCMD_RS         (1 << 0)  // Run / Stop
#define USBCMD_HCRST      (1 << 1)  // Host Controller Reset

// Map AZERTY minuscules
static const char keymap_ascii[256] = {
    [0x04] = 'q', [0x05] = 'b', [0x06] = 'c', [0x07] = 'd', [0x08] = 'e',
    [0x09] = 'f', [0x0A] = 'g', [0x0B] = 'h', [0x0C] = 'i', [0x0D] = 'j',
    [0x0E] = 'k', [0x0F] = 'l', [0x10] = 'm', [0x11] = 'n', [0x12] = 'o',
    [0x13] = 'p', [0x14] = 'a', [0x15] = 'r', [0x16] = 's', [0x17] = 't',
    [0x18] = 'u', [0x19] = 'v', [0x1A] = 'z', [0x1B] = 'x', [0x1C] = 'y',
    [0x1D] = 'w', [0x1E] = '&', [0x1F] = 'e', [0x20] = '"', [0x21] = '\'',
    [0x22] = '(', [0x23] = '-', [0x24] = 'e', [0x25] = '_', [0x26] = 'c',
    [0x27] = 'a', [0x28] = '\n', [0x2A] = '\b', [0x2C] = ' '
};

// Map AZERTY majuscules
static const char keymap_shift[256] = {
    [0x04] = 'Q', [0x05] = 'B', [0x06] = 'C', [0x07] = 'D', [0x08] = 'E',
    [0x09] = 'F', [0x0A] = 'G', [0x0B] = 'H', [0x0C] = 'I', [0x0D] = 'J',
    [0x0E] = 'K', [0x0F] = 'L', [0x10] = 'M', [0x11] = 'N', [0x12] = 'O',
    [0x13] = 'P', [0x14] = 'A', [0x15] = 'R', [0x16] = 'S', [0x17] = 'T',
    [0x18] = 'U', [0x19] = 'V', [0x1A] = 'Z', [0x1B] = 'X', [0x1C] = 'Y',
    [0x1D] = 'W', [0x1E] = '1', [0x1F] = '2', [0x20] = '3', [0x21] = '4',
    [0x22] = '5', [0x23] = '6', [0x24] = '7', [0x25] = '8', [0x26] = '9',
    [0x27] = '0'
};

static uint8_t previous_keys[6] = {0};

static bool is_key_new(uint8_t scancode) {
    if (scancode == 0) return false;
    for (int i = 0; i < 6; i++) {
        if (previous_keys[i] == scancode) return false;
    }
    return true;
}

static void handle_hid_report(hid_keyboard_report_t *report) {
    if (!report) return;

    bool shift = (report->modifiers & 0x22) != 0;

    for (int i = 0; i < 6; i++) {
        uint8_t scancode = report->keys[i];
        if (scancode == 0) continue;

        if (is_key_new(scancode)) {
            char c = shift ? keymap_shift[scancode] : keymap_ascii[scancode];
            if (c != 0) {
                console_putc(c);
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        previous_keys[i] = report->keys[i];
    }
}

static void xhci_ring_command(xhci_controller_t *ctrl, xhci_trb_t trb) {
    uint32_t idx = ctrl->cmd_idx;
    trb.control = (trb.control & ~TRB_CYCLE_BIT) | ctrl->cmd_pcs;
    ctrl->cmd_ring[idx] = trb;

    ctrl->cmd_idx++;
    if (ctrl->cmd_idx >= 15) {
        ctrl->cmd_ring[15].parameter = (uintptr_t)ctrl->cmd_ring;
        ctrl->cmd_ring[15].control = (6 << 10) | ctrl->cmd_pcs;
        ctrl->cmd_idx = 0;
        ctrl->cmd_pcs ^= 1;
    }

    dsb();
    if (ctrl->doorbells) {
        ctrl->doorbells[0] = 0;
    }
}

int xhci_init(xhci_controller_t *ctrl, uintptr_t bar0) {
    if (!ctrl || bar0 == 0) return -1;
    ctrl->bar0 = bar0;
    ctrl->reserved_pad = 0;

    // 1. Contrôle de sécurité MMIO (Détection matériel/QEMU)
    uint32_t cap_first_dword = mmio_read32(bar0);
    uint8_t caplength = cap_first_dword & 0xFF;
    uint32_t dboff = mmio_read32(bar0 + 0x14);
    uint32_t rtsoff = mmio_read32(bar0 + 0x18);

    // Si les adresses MMIO ne sont pas mappées ou renvoient des offsets non alignés
    if (cap_first_dword == 0xFFFFFFFF || caplength == 0 || (rtsoff & 3) != 0 || (dboff & 3) != 0) {
        kprintf("[XHCI WARN] Materiel xHCI absent ou non initialise sur %p.\n", bar0);
        return -1; // Annulation propre sans crash !
    }

    ctrl->cap_regs = (volatile uint32_t *)bar0;
    ctrl->op_regs = (volatile uint32_t *)(bar0 + caplength);
    ctrl->rt_regs = (volatile uint32_t *)(bar0 + rtsoff);
    ctrl->doorbells = (volatile uint32_t *)(bar0 + dboff);

    // 2. Allocations des anneaux DMA
    ctrl->dcbaa = (uint64_t *)kmalloc_aligned(256 * sizeof(uint64_t), 64);
    ctrl->cmd_ring = (xhci_trb_t *)kmalloc_aligned(16 * sizeof(xhci_trb_t), 64);
    ctrl->event_ring_size = 16;
    ctrl->event_ring = (xhci_trb_t *)kmalloc_aligned(ctrl->event_ring_size * sizeof(xhci_trb_t), 64);
    ctrl->erst = (xhci_erst_entry_t *)kmalloc_aligned(sizeof(xhci_erst_entry_t), 64);
    ctrl->keyboard_report = (hid_keyboard_report_t *)kmalloc_aligned(sizeof(hid_keyboard_report_t), 64);

    if (!ctrl->dcbaa || !ctrl->cmd_ring || !ctrl->event_ring || !ctrl->erst || !ctrl->keyboard_report) {
        return -1;
    }

    ctrl->cmd_pcs = 1;
    ctrl->cmd_idx = 0;
    ctrl->event_dequeue = ctrl->event_ring;
    ctrl->event_ccs = 1;

    // 3. Réinitialisation matérielle (Reset xHCI)
    ctrl->op_regs[XHCI_OP_USBCMD / 4] |= USBCMD_HCRST;
    wait_msec(10);
    while (ctrl->op_regs[XHCI_OP_USBCMD / 4] & USBCMD_HCRST);

    // 4. Configuration des registres d'anneaux
    ctrl->op_regs[XHCI_OP_CONFIG / 4] = 16;
    
    uint64_t dcbaa_addr = (uintptr_t)ctrl->dcbaa;
    ctrl->op_regs[XHCI_OP_DCBAAP / 4] = (uint32_t)dcbaa_addr;
    ctrl->op_regs[XHCI_OP_DCBAAP / 4 + 1] = (uint32_t)(dcbaa_addr >> 32);

    uint64_t crcr_addr = (uintptr_t)ctrl->cmd_ring | 1;
    ctrl->op_regs[XHCI_OP_CRCR / 4] = (uint32_t)crcr_addr;
    ctrl->op_regs[XHCI_OP_CRCR / 4 + 1] = (uint32_t)(crcr_addr >> 32);

    ctrl->erst->base_address = (uintptr_t)ctrl->event_ring;
    ctrl->erst->size = ctrl->event_ring_size;
    ctrl->erst->reserved = 0;

    volatile uint32_t *ir0 = (volatile uint32_t *)((uintptr_t)ctrl->rt_regs + 0x20);
    ir0[2] = 1;
    
    uint64_t erstba_addr = (uintptr_t)ctrl->erst;
    ir0[4] = (uint32_t)erstba_addr;
    ir0[5] = (uint32_t)(erstba_addr >> 32);

    ctrl->erdp = (volatile uint64_t *)((uintptr_t)ir0 + 0x18);
    *(ctrl->erdp) = (uintptr_t)ctrl->event_ring | (1 << 3);

    // 5. Démarrage du contrôleur
    ctrl->op_regs[XHCI_OP_USBCMD / 4] |= USBCMD_RS;
    wait_msec(10);

    return 0;
}

int xhci_enable_slot(xhci_controller_t *ctrl, uint8_t *slot_id) {
    if (!ctrl || !slot_id || !ctrl->cmd_ring) return -1;

    xhci_trb_t trb = {0};
    trb.control = (9 << 10); // Enable Slot
    xhci_ring_command(ctrl, trb);

    wait_msec(50);
    *slot_id = 1;
    ctrl->keyboard_slot_id = 1;
    return 0;
}

void xhci_setup_keyboard(xhci_controller_t *ctrl, uint8_t slot_id) {
    if (!ctrl || slot_id == 0) return;

    ctrl->ep1_ring = (xhci_trb_t *)kmalloc_aligned(16 * sizeof(xhci_trb_t), 64);
    ctrl->ep1_pcs = 1;

    if (!ctrl->ep1_ring) return;

    ctrl->ep1_ring[0].parameter = (uintptr_t)ctrl->keyboard_report;
    ctrl->ep1_ring[0].status = sizeof(hid_keyboard_report_t);
    ctrl->ep1_ring[0].control = (1 << 10) | (1 << 5) | ctrl->ep1_pcs;

    dsb();
    if (ctrl->doorbells) {
        ctrl->doorbells[slot_id] = 3;
    }
}

void xhci_poll_keyboard(xhci_controller_t *ctrl) {
    if (!ctrl || !ctrl->event_dequeue) return;

    xhci_trb_t *trb = ctrl->event_dequeue;

    if ((trb->control & TRB_CYCLE_BIT) != ctrl->event_ccs) {
        return;
    }

    uint32_t type = TRB_TYPE(trb->control);

    if (type == TRB_TYPE_TRANSFER_EVENT) {
        hid_keyboard_report_t *report = (hid_keyboard_report_t *)(uintptr_t)trb->parameter;
        if (report != NULL) {
            handle_hid_report(report);
        } else {
            handle_hid_report(ctrl->keyboard_report);
        }

        if (ctrl->ep1_ring != NULL) {
            ctrl->ep1_ring[0].control = (1 << 10) | (1 << 5) | ctrl->ep1_pcs;
            dsb();
            if (ctrl->doorbells) {
                ctrl->doorbells[ctrl->keyboard_slot_id] = 3;
            }
        }
    }

    ctrl->event_dequeue++;
    if (ctrl->event_dequeue >= (ctrl->event_ring + ctrl->event_ring_size)) {
        ctrl->event_dequeue = ctrl->event_ring;
        ctrl->event_ccs ^= 1;
    }

    if (ctrl->erdp != NULL) {
        *(ctrl->erdp) = (uintptr_t)ctrl->event_dequeue | (1 << 3);
    }
}