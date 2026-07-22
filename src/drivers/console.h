#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdarg.h>

// Console configuration
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8
#define LINE_HEIGHT 10 // 8px font height + 2px margin

// Initialize the console system
void console_init(void);

// Print a single character with scrolling support
void console_putc(char c);

// Print a string
void console_puts(const char *str);

// Formatted print function (kprintf)
void kprintf(const char *fmt, ...);

// Clear console and reset cursor
void console_clear(void);

#endif