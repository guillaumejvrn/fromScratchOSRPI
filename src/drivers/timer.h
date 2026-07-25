#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Base address of the System Timer on RPi 4 (BCM2711)
#define MMIO_BASE          0xFE000000
#define SYSTEM_TIMER_BASE  (MMIO_BASE + 0x3000)

// Structure representing the hardware registers
typedef struct {
    volatile uint32_t cs;   // Control / Status
    volatile uint32_t clo;  // Counter (lower 32 bits)
    volatile uint32_t chi;  // Counter (upper 32 bits)
    volatile uint32_t c0;   // Compare register 0
    volatile uint32_t c1;   // Compare register 1
    volatile uint32_t c2;   // Compare register 2
    volatile uint32_t c3;   // Compare register 3
} system_timer_t;

// Read the system timer in microseconds (µs)
uint64_t timer_get_ticks(void);

// Pause for N microseconds
void wait_usec(uint32_t usec);

// Pause for N milliseconds
void wait_msec(uint32_t msec);

#endif