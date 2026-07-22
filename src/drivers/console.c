#include "console.h"
#include "framebuffer.h"

static int cursor_x = 0;
static int cursor_y = 0;

// Text color (default: white)
static unsigned char text_r = 255;
static unsigned char text_g = 255;
static unsigned char text_b = 255;

void console_init(void) {
    cursor_x = 0;
    cursor_y = 0;
}

void console_clear(void) {
    clear_screen(10, 10, 35); // Dark blue background
    cursor_x = 0;
    cursor_y = 0;
}

static void console_scroll(void) {
    // 1. Move pixel lines up by LINE_HEIGHT
    int line_pixels = LINE_HEIGHT * (pitch / 4);
    int total_pixels = height * (pitch / 4);

    for (int i = 0; i < total_pixels - line_pixels; i++) {
        fb[i] = fb[i + line_pixels];
    }

    // 2. Clear the bottom line
    unsigned int bg_color = (0xFF << 24) | (10 << 16) | (10 << 8) | 35;
    for (int i = total_pixels - line_pixels; i < total_pixels; i++) {
        fb[i] = bg_color;
    }

    // 3. Move cursor up to the last line
    cursor_y -= LINE_HEIGHT;
}

void console_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += LINE_HEIGHT;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x += CHAR_WIDTH * 4; // Tab = 4 spaces
    } else {
        draw_char(c, cursor_x, cursor_y, text_r, text_g, text_b);
        cursor_x += CHAR_WIDTH;

        // Line wrap if cursor exceeds screen width
        if (cursor_x + CHAR_WIDTH > (int)width) {
            cursor_x = 0;
            cursor_y += LINE_HEIGHT;
        }
    }

    // Scroll if cursor exceeds screen height
    if (cursor_y + LINE_HEIGHT > (int)height) {
        console_scroll();
    }
}

void console_puts(const char *str) {
    while (*str) {
        console_putc(*str);
        str++;
    }
}

// Helper: Convert integer to string (hexadecimal or decimal)
static void itoa(unsigned long val, int base, char *buf) {
    char tmp[64];
    int i = 0;

    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (val > 0) {
        int rem = val % base;
        tmp[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        val /= base;
    }

    int j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[64];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    console_putc(c);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char*);
                    if (!s) s = "(null)";
                    console_puts(s);
                    break;
                }
                case 'd':
                case 'i': {
                    long val = va_arg(args, long);
                    if (val < 0) {
                        console_putc('-');
                        val = -val;
                    }
                    itoa(val, 10, buf);
                    console_puts(buf);
                    break;
                }
                case 'u': {
                    unsigned long val = va_arg(args, unsigned long);
                    itoa(val, 10, buf);
                    console_puts(buf);
                    break;
                }
                case 'x':
                case 'p': {
                    unsigned long val = va_arg(args, unsigned long);
                    console_puts("0x");
                    itoa(val, 16, buf);
                    console_puts(buf);
                    break;
                }
                case '%': {
                    console_putc('%');
                    break;
                }
                default:
                    console_putc('%');
                    console_putc(*fmt);
                    break;
            }
        } else {
            console_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}