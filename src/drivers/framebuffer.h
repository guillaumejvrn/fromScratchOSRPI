#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

extern unsigned int *fb;
extern unsigned int pitch;
extern unsigned int width;
extern unsigned int height;

int fb_init(void);
void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void clear_screen(unsigned char r, unsigned char g, unsigned char b);
void draw_char(char c, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_string(const char *str, int x, int y, unsigned char r, unsigned char g, unsigned char b);

#endif