#ifndef IO_H
#define IO_H

#include <stdint.h>

// Data synchronization barrier (forces writes to complete to RAM)
static inline void dsb(void) {
    asm volatile("dsb sy" ::: "memory");
}

// Data memory barrier (ensures memory access ordering)
static inline void dmb(void) {
    asm volatile("dmb sy" ::: "memory");
}

// Safe 32-bit MMIO register write with barriers
static inline void mmio_write32(uintptr_t addr, uint32_t value) {
    dsb();
    *(volatile uint32_t *)addr = value;
    dsb();
}

// Safe 32-bit MMIO register read
static inline uint32_t mmio_read32(uintptr_t addr) {
    uint32_t val = *(volatile uint32_t *)addr;
    dmb();
    return val;
}

#endif