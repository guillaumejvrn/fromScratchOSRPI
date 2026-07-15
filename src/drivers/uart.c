// Raspberry Pi 4 Memory-Mapped I/O Base Address (BCM2711)
#define MMIO_BASE       0xFE000000

// PL011 UART register definitions
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE + 0x201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE + 0x201018))

void uart_init() {
    // QEMU sets up the UART automatically.
    // On hardware, the GPU firmware handles this if enabled in config.txt.
}

// Transmit a single character over the serial port
void uart_putc(char c) {
    // Wait until the transmit FIFO has free space (TXFF flag is 0)
    while (*UART0_FR & 0x20);
    // Write the character byte directly into the data register
    *UART0_DR = c;
}

// Transmit a null-terminated string
void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r'); // Inject a carriage return for console formatting
        }
        uart_putc(*s++);
    }
}