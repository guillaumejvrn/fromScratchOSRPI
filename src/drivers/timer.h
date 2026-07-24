#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Adresse de base du System Timer sur RPi 4 (BCM2711)
#define MMIO_BASE          0xFE000000
#define SYSTEM_TIMER_BASE  (MMIO_BASE + 0x3000)

// Structure représentant les registres du matériel
typedef struct {
    volatile uint32_t cs;   // Control / Status
    volatile uint32_t clo;  // Compteur (32 bits de poids faible)
    volatile uint32_t chi;  // Compteur (32 bits de poids fort)
    volatile uint32_t c0;   // Registre de comparaison 0
    volatile uint32_t c1;   // Registre de comparaison 1
    volatile uint32_t c2;   // Registre de comparaison 2
    volatile uint32_t c3;   // Registre de comparaison 3
} system_timer_t;

// Lit l'horloge système en microsecondes (µs)
uint64_t timer_get_ticks(void);

// Fait une pause de N microsecondes
void wait_usec(uint32_t usec);

// Fait une pause de N millisecondes
void wait_msec(uint32_t msec);

#endif