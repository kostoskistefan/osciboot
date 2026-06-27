#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <linux/fb.h>

typedef struct font_t font_t;

typedef struct framebuffer_t
{
    int fd;

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    uint16_t *memory;

    int width;
    int height;

    int stride;
} framebuffer_t;

void framebuffer_create(framebuffer_t *const framebuffer);
void framebuffer_destroy(framebuffer_t *const framebuffer);

void framebuffer_put_pixel(framebuffer_t *const framebuffer, const uint16_t x, const uint16_t y, const uint16_t color);

void framebuffer_fill(framebuffer_t *const framebuffer, const uint16_t color);

void framebuffer_fill_rectangle(
    framebuffer_t *const framebuffer,
    const uint16_t x,
    const uint16_t y,
    const uint16_t width,
    const uint16_t height,
    const uint16_t color
);

void framebuffer_render_text(
    framebuffer_t *const framebuffer,
    const font_t *font,
    const uint16_t x,
    const uint16_t y,
    const char *text,
    const uint16_t color
);

#endif
