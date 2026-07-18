#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

extern unsigned int *fb;
extern unsigned int pitch;

int fb_init(void);
void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void clear_screen(unsigned char r, unsigned char g, unsigned char b);

#endif