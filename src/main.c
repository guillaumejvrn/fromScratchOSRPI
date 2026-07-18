#include "framebuffer.h"

void main() {          
    // try to init hmdi with mailbox
    if (fb_init() == 0) {
        
        // put all the screen in blue
        clear_screen(0, 0, 255);

        // draw 200x200 white square pixel at the center (resolution 1024x768)
        for (int y = 284; y < 484; y++) {
            for (int x = 412; x < 612; x++) {
                draw_pixel(x, y, 255, 255, 255);
            }
        }
    } else {
        // visual crash the screen stays black
    }

    // keep the cpu awake
    while(1) {                  
        
    }  
}