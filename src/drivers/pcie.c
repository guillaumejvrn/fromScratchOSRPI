#include "pcie.h"
#include "console.h"
#include "timer.h"
#include "arch/io.h"

#define BCM2711_PCIE_REG_BASE  0xFD500000

// Registres du pont PCIe Broadcom (BCM2711)
#define PCIE_BRIDGE_STATUS     (BCM2711_PCIE_REG_BASE + 0x4006)
#define PCIE_EXT_CFG_DATA      (BCM2711_PCIE_REG_BASE + 0x8000)

int pcie_init(pci_device_t *usb_dev) {
    if (!usb_dev) return -1;

    kprintf("[PCIE] Initialisation du pont PCIe Broadcom (0xFD500000)...\n");

    usb_dev->bus = 0;
    usb_dev->slot = 0;
    usb_dev->func = 0;
    usb_dev->vendor_id = VL805_VENDOR_ID;
    usb_dev->device_id = VL805_DEVICE_ID;

    // 1. Activer la retranscription MMIO sur le bus PCIe BCM2711
    // Registre Command/Status du pont PCIe : Activation Memory Space (bit 1) + Bus Master (bit 2)
    uint32_t bridge_cmd = mmio_read32(BCM2711_PCIE_REG_BASE + 0x04);
    mmio_write32(BCM2711_PCIE_REG_BASE + 0x04, bridge_cmd | 0x06);

    // 2. Configuration de l'espace PCI du périphérique VL805 (Bus 1, Slot 0)
    uintptr_t vl805_cfg = PCIE_EXT_CFG_DATA;
    uint32_t vl805_cmd = mmio_read32(vl805_cfg + 0x04);
    mmio_write32(vl805_cfg + 0x04, vl805_cmd | 0x06); // Memory Enable + Bus Master

    // 3. Récupérer l'adresse BAR0 (Base Address Register 0) configurée par start4.elf
    uint32_t bar0_low = mmio_read32(vl805_cfg + 0x10);
    uintptr_t xhci_base = (bar0_low & ~0x0F);

    // Si la BAR0 n'est pas initialisée par le firmware, fallback sur la plage d'adresse BCM2711
    if (xhci_base == 0 || xhci_base == 0xFFFFFFF0) {
        xhci_base = 0xFE100000;
    }

    kprintf("[PCIE] Controleur xHCI mappe a l'adresse MMIO %p\n", xhci_base);
    usb_dev->bar0 = xhci_base;

    wait_msec(10);
    return 0;
}