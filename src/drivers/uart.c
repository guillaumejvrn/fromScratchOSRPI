#include "uart.h"
#include "io.h"

static uart_regs_t *uart = (uart_regs_t *)UART0_BASE;

void uart_init(void) {
    uart->cr = 0;
    uart->icr = 0x7FF;
    uart->ibrd = 26;
    uart->fbrd = 3;
    uart->lcrh = (1 << 4) | (1 << 5) | (1 << 6); // 8n1, FIFO
    uart->imsc = 0;
    uart->cr = (1 << 0) | (1 << 8) | (1 << 9); // UARTEN, TXE, RXE
}

int uart_has_char(void) {
    // Si le bit RXFE (Receive FIFO Empty, bit 4) est à 0, c'est qu'il y a un caractère
    return (uart->fr & (1 << 4)) == 0;
}

char uart_getc(void) {
    while (!uart_has_char()) {
        // Petite pause pour laisser respirer l'émulateur
        for(volatile int i = 0; i < 1000; i++);
    }
    return (char)(uart->dr & 0xFF);
}

void uart_putc(char c) {
    while (uart->fr & (1 << 5)) { // TXFF (Transmit FIFO Full)
        asm volatile("nop");
    }
    uart->dr = c;
}