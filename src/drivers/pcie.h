#ifndef PCIE_H
#define PCIE_H

#include <stdint.h>

// MMIO registers for the BCM2711 PCIe controller
#define BCM2711_PCIE_REG_BASE  0xFD500000

// Vendor/Device IDs for the Pi 4's VL805 USB chip
#define VL805_VENDOR_ID  0x1106
#define VL805_DEVICE_ID  0x3483

// Structure describing a detected PCI device
typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint16_t vendor_id;
    uint16_t device_id;
    uintptr_t bar0; // Base Address Register 0 (xHCI MMIO address)
} pci_device_t;

// Initialize the PCIe bridge and locate the VL805 USB controller
int pcie_init(pci_device_t *usb_dev);

#endif