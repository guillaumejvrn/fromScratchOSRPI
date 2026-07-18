#include "framebuffer.h"

// basic registers MMIO of BCM2711 (Raspberry Pi 4)
#define PERIPHERAL_BASE 0xFE000000

// specifics registers video mailbox
#define MBOX_BASE    (PERIPHERAL_BASE + 0x0000B880)
#define MBOX_READ    (MBOX_BASE + 0x00)
#define MBOX_STATUS  (MBOX_BASE + 0x18)
#define MBOX_WRITE   (MBOX_BASE + 0x20)

// material flag protocol Mailbox
#define MBOX_FULL    0x80000000
#define MBOX_EMPTY   0x40000000
#define MBOX_SUCCESS 0x80000000

// 16-byte aligned message buffer required by the GPU
volatile unsigned int __attribute__((aligned(16))) mbox[36];

unsigned int *fb = 0;
unsigned int pitch = 0;

// direct memory call functions MMIO
static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int *)reg = val;
} 

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int *)reg;
} 

// send message to gpu
int mbox_call(unsigned char channel) {
    // combine 28bits of memory address and 4 bits of the channel
    unsigned int r = ((unsigned int)((long)&mbox) & ~0xF) | (channel & 0xF);
    
    // wait the box not be overloaded
    while (mmio_read(MBOX_STATUS) & MBOX_FULL);
    
    // inject the message 
    mmio_write(MBOX_WRITE, r);
    
    // wait the gpu respond
    while (1) {
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY);
        
        // if the answer is same as initial message succes code
        if (r == mmio_read(MBOX_READ)) {
            return mbox[1] == MBOX_SUCCESS;
        }
    }
    return 0;
} 

// init et screen allocation 1024x768
int fb_init(void) {
    mbox[0] = 35 * 4;        // total structural size in bytes
    mbox[1] = 0;             // request

    // physical resolution
    mbox[2] = 0x00048003;         
    mbox[3] = 8; 
    mbox[4] = 0;    
    mbox[5] = 1024; 
    mbox[6] = 768;

    // virtual resolution
    mbox[7] = 0x00048004;         
    mbox[8] = 8; 
    mbox[9] = 0;    
    mbox[10] = 1024; 
    mbox[11] = 768;

 // color depth (32-bit = ARGB)
    mbox[12] = 0x00048005;        
    mbox[13] = 4;
    mbox[14] = 0;            // <-- À RAJOUTER
    mbox[15] = 32;                

    // space allocation Framebuffer in the RAM
    mbox[16] = 0x00040001;        
    mbox[17] = 8; 
    mbox[18] = 0;           
    mbox[19] = 4096;      
    mbox[20] = 0;

    // Pitch (byte size of the width)
    mbox[21] = 0x00040008;        
    mbox[22] = 4; 
    mbox[23] = 0;            // <-- À RAJOUTER
    mbox[24] = 0;
    
    mbox[25] = 0;    // end tag

    // bloc send to the channel 8 (firmware propriety)
    if (mbox_call(8)) {
        // succes
        fb = (unsigned int*)((long)mbox[20] & 0x3FFFFFFF);
        pitch = mbox[24];
        return 0; 
    }
    return -1; // gpu failed
} 

void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    int index = (y * (pitch / 4)) + x;
    unsigned int color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    fb[index] = color;
} 

void clear_screen(unsigned char r, unsigned char g, unsigned char b) {
    unsigned int color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    int total_pixels = 1024 * 768;
    for (int i = 0; i < total_pixels; i++) {
        fb[i] = color;
    }
}