#ifndef XHCI_H
#define XHCI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Types de TRB xHCI
#define TRB_TYPE_NORMAL             1
#define TRB_TYPE_ENABLE_SLOT        9
#define TRB_TYPE_ADDRESS_DEVICE     11
#define TRB_TYPE_CONFIGURE_ENDPOINT 12
#define TRB_TYPE_COMMAND_COMPLETION 33
#define TRB_TYPE_TRANSFER_EVENT     32

#define TRB_TYPE(ctrl)              (((ctrl) >> 10) & 0x3F)
#define TRB_CYCLE_BIT               (1 << 0)

// Structure d'un TRB xHCI (16 octets)
typedef struct __attribute__((aligned(16))) {
    uint64_t parameter; 
    uint32_t status;    
    uint32_t control;   
} xhci_trb_t;

// Table de Segment d'Event Ring (ERST)
typedef struct __attribute__((aligned(64))) {
    uint64_t base_address;
    uint32_t size;
    uint32_t reserved;
} xhci_erst_entry_t;

// Rapport HID Clavier USB (8 octets)
typedef struct {
    uint8_t modifiers;  // Bit 1 = Left Shift, Bit 5 = Right Shift
    uint8_t reserved;   
    uint8_t keys[6];    // 6 scancodes
} hid_keyboard_report_t;

// Contexte d'entrée xHCI (Slot + Endpoint 0 + Endpoint 1)
typedef struct __attribute__((aligned(64))) {
    uint32_t drop_flags;
    uint32_t add_flags;
    uint32_t reserved[6];
    uint32_t slot_context[8];
    uint32_t ep0_context[8];
    uint32_t ep1_in_context[8];
} xhci_input_context_t;

// Structure du contrôleur xHCI
typedef struct __attribute__((aligned(16))) {
    uintptr_t bar0;
    uint64_t reserved_pad;

    // Registres MMIO xHCI
    volatile uint32_t *cap_regs;
    volatile uint32_t *op_regs;
    volatile uint32_t *rt_regs;
    volatile uint32_t *doorbells;
    volatile uint64_t *erdp;

    // Structures DMA RAM
    uint64_t *dcbaa;
    xhci_trb_t *cmd_ring;
    uint32_t cmd_pcs;
    uint32_t cmd_idx;

    xhci_trb_t *event_ring;
    xhci_trb_t *event_dequeue;
    uint32_t event_ccs;
    size_t event_ring_size;
    xhci_erst_entry_t *erst;

    // Endpoint Clavier USB
    uint8_t keyboard_slot_id;
    xhci_trb_t *ep1_ring;
    uint32_t ep1_pcs;
    hid_keyboard_report_t *keyboard_report;
} xhci_controller_t;

// Prototypes de l'API xHCI
int xhci_init(xhci_controller_t *ctrl, uintptr_t bar0);
int xhci_enable_slot(xhci_controller_t *ctrl, uint8_t *slot_id);
void xhci_setup_keyboard(xhci_controller_t *ctrl, uint8_t slot_id);
void xhci_poll_keyboard(xhci_controller_t *ctrl);

#endif // XHCI_H