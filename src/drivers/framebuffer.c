#include "framebuffer.h"
#include "font.h"

// BCM2711 MMIO Registers (Raspberry Pi 4)
#define PERIPHERAL_BASE 0xFE000000
#define MBOX_BASE    (PERIPHERAL_BASE + 0x0000B880)
#define MBOX_READ    (MBOX_BASE + 0x00)
#define MBOX_STATUS  (MBOX_BASE + 0x18) // Read status (MBOX 0)
#define MBOX_WRITE   (MBOX_BASE + 0x20)
#define MBOX_STATUS1 (MBOX_BASE + 0x38) // Write status (MBOX 1)
#define MBOX_FULL    0x80000000
#define MBOX_EMPTY   0x40000000
#define MBOX_SUCCESS 0x80000000

// Mailbox buffer aligned to 16 bytes
volatile unsigned int __attribute__((aligned(16))) mbox[36];

unsigned int *fb = 0;
unsigned int pitch = 0;
unsigned int width = 0;   // Dynamic width
unsigned int height = 0;  // Dynamic height

static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int *)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int *)reg;
}

int mbox_call(unsigned char channel) {
    // 1. Add VC Bus Alias 0xC0000000 so the GPU can read RAM
    unsigned int addr = ((unsigned int)(unsigned long)&mbox) | 0xC0000000;
    unsigned int r = (addr & ~0xF) | (channel & 0xF);

    // 2. Wait until the write buffer (MBOX1 = 0x38) is not full
    while (mmio_read(MBOX_STATUS1) & MBOX_FULL);

    // 3. Send request to GPU
    mmio_write(MBOX_WRITE, r);

    // 4. Wait for response on MBOX0 (0x18)
    while (1) {
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY);
        if (r == mmio_read(MBOX_READ)) {
            return mbox[1] == MBOX_SUCCESS;
        }
    }
    return 0;
}

int fb_init(void) {
    // --- STEP 1: Query physical resolution detected by VideoCore GPU ---
    for (int i = 0; i < 36; i++) mbox[i] = 0;

    mbox[0] = 8 * 4;
    mbox[1] = 0;          // Request code
    mbox[2] = 0x00040003; // Tag: Get Physical Width/Height
    mbox[3] = 8;          // Value buffer size
    mbox[4] = 0;          // Request indicator
    mbox[5] = 0;          // Will receive width
    mbox[6] = 0;          // Will receive height
    mbox[7] = 0;          // End tag

    if (mbox_call(8) && mbox[5] > 0 && mbox[6] > 0) {
        width = mbox[5];
        height = mbox[6];
    } else {
        // Fallback default resolution if EDID query fails
        width = 1024;
        height = 768;
    }

    // --- STEP 2: Allocate Framebuffer matching the detected screen size ---
    for (int i = 0; i < 36; i++) mbox[i] = 0;

    mbox[0] = 26 * 4;
    mbox[1] = 0; // Request code

    // Physical resolution
    mbox[2] = 0x00048003;
    mbox[3] = 8;
    mbox[4] = 0;
    mbox[5] = width;
    mbox[6] = height;

    // Virtual resolution
    mbox[7] = 0x00048004;
    mbox[8] = 8;
    mbox[9] = 0;
    mbox[10] = width;
    mbox[11] = height;

    // Color depth (32-bit ARGB)
    mbox[12] = 0x00048005;
    mbox[13] = 4;
    mbox[14] = 0;
    mbox[15] = 32;

    // Allocate Framebuffer
    mbox[16] = 0x00040001;
    mbox[17] = 8;
    mbox[18] = 0;
    mbox[19] = 4096; // Requested alignment
    mbox[20] = 0;

    // Pitch
    mbox[21] = 0x00040008;
    mbox[22] = 4;
    mbox[23] = 0;
    mbox[24] = 0;

    // End tag
    mbox[25] = 0;

    if (mbox_call(8)) {
        // Apply ARM physical address mask
        fb = (unsigned int*)((unsigned long)mbox[19] & 0x3FFFFFFF);
        pitch = mbox[24];
        return 0;
    }
    return -1;
}

void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    // Dynamic boundary check
    if (x < 0 || x >= (int)width || y < 0 || y >= (int)height) return;

    int offset = (y * (pitch / 4)) + x;
    unsigned int color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    fb[offset] = color;
}

void clear_screen(unsigned char r, unsigned char g, unsigned char b) {
    // Dynamic loop based on detected dimensions
    for (int y = 0; y < (int)height; y++) {
        for (int x = 0; x < (int)width; x++) {
            draw_pixel(x, y, r, g, b);
        }
    }
}

void draw_char(char c, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned char uc = (unsigned char)c;
    if (uc >= 128) return;

    for (int row = 0; row < 8; row++) {
        unsigned char bitmask = font8x8[uc][row];
        for (int col = 0; col < 8; col++) {
            if (bitmask & (1 << (7 - col))) {
                draw_pixel(x + col, y + row, r, g, b);
            }
        }
    }
}

void draw_string(const char *str, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    int start_x = x;
    while (*str) {
        if (*str == '\n') {
            x = start_x;
            y += 10; // Vertical spacing (8px character height + 2px margin)
        } else {
            draw_char(*str, x, y, r, g, b);
            x += 8;  // Horizontal spacing
        }
        str++;
    }
}