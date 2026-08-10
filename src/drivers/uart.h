#ifndef UART_H
#define UART_H

#include <stdint.h>

// Adresse MMIO de l'UART0 (PL011) sur Raspberry Pi 4
#define UART0_BASE 0xFE201000

typedef struct {
    volatile uint32_t dr;        // 0x00 Data Register
    volatile uint32_t rsrecr;    // 0x04
    volatile uint32_t reserved1[4];
    volatile uint32_t fr;        // 0x18 Flag Register
    volatile uint32_t reserved2;
    volatile uint32_t ilpr;      // 0x20
    volatile uint32_t ibrd;      // 0x24 Integer Baud Rate Divisor
    volatile uint32_t fbrd;      // 0x28 Fractional Baud Rate Divisor
    volatile uint32_t lcrh;      // 0x2C Line Control Register
    volatile uint32_t cr;        // 0x30 Control Register
    volatile uint32_t ifls;      // 0x34 Interrupt FIFO Level Select
    volatile uint32_t imsc;      // 0x38 Interrupt Mask Set Clear
    volatile uint32_t ris;       // 0x3C Raw Interrupt Status
    volatile uint32_t mis;       // 0x40 Masked Interrupt Status
    volatile uint32_t icr;       // 0x44 Interrupt Clear Register
} uart_regs_t;

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
int uart_has_char(void);

#endif