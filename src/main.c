#include "framebuffer.h"

void main() {          
    // try to initialize HDMI with the mailbox
    if (fb_init() == 0) {
        
        // fill the screen with blue
        clear_screen(0, 0, 255);

        // draw a 200x200 white square in the center (resolution 1024x768)
        for (int y = 284; y < 484; y++) {
            for (int x = 412; x < 612; x++) {
                draw_pixel(x, y, 255, 255, 255);
            }
        }

        clear_screen(10, 10, 35);

        // Afficher des messages de boot sur le Framebuffer
        draw_string("Bare-Metal OS - Console initialized!\n", 40, 40, 0, 255, 0); // Vert
        draw_string("Resolution: 1024x768 32-bit ARGB\n", 40, 56, 255, 255, 255);  // Blanc
        draw_string("Running on Raspberry Pi 4 (AArch64)\n", 40, 72, 255, 255, 255);

        
    } else {
        // visual crash: the screen remains black
    }

    // keep the CPU awake
    while(1) {                  
        
    }  
}