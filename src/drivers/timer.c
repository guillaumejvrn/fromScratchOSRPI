#include "timer.h"

// Pointer to the device's exact MMIO base address
static system_timer_t *timer = (system_timer_t *)SYSTEM_TIMER_BASE;

uint64_t timer_get_ticks(void) {
    uint32_t hi = timer->chi;
    uint32_t lo = timer->clo;

    // Safety check if the timer rolls over while reading
    if (hi != timer->chi) {
        hi = timer->chi;
        lo = timer->clo;
    }

    return ((uint64_t)hi << 32) | lo;
}

void wait_usec(uint32_t usec) {
    uint64_t start = timer_get_ticks();
    // Wait until the elapsed time reaches the requested delay
    while (timer_get_ticks() - start < usec) {
        asm volatile("nop");
    }
}

void wait_msec(uint32_t msec) {
    wait_usec(msec * 1000);
}