#include "framebuffer.h"
#include "font.h"

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef DISPLAY_WIDTH
#define DISPLAY_WIDTH INT_MAX
#endif

#ifndef DISPLAY_HEIGHT
#define DISPLAY_HEIGHT INT_MAX
#endif

static inline int min_int(const int a, const int b)
{
    return a < b ? a : b;
}

void framebuffer_create(framebuffer_t *const framebuffer, const char *device)
{
    if (framebuffer == NULL)
    {
        return;
    }

    framebuffer->fd = open(device, O_RDWR);

    if (framebuffer->fd < 0)
    {
        perror("open");
        return;
    }

    if (ioctl(framebuffer->fd, FBIOGET_VSCREENINFO, &framebuffer->vinfo) < 0)
    {
        perror("FBIOGET_VSCREENINFO");
        close(framebuffer->fd);
        framebuffer->fd = -1;
        return;
    }

    if (ioctl(framebuffer->fd, FBIOGET_FSCREENINFO, &framebuffer->finfo) < 0)
    {
        perror("FBIOGET_FSCREENINFO");
        close(framebuffer->fd);
        framebuffer->fd = -1;
        return;
    }

    framebuffer->width = framebuffer->vinfo.xres;
    framebuffer->height = framebuffer->vinfo.yres;

    framebuffer->visible_width = min_int(framebuffer->width, DISPLAY_WIDTH);
    framebuffer->visible_height = min_int(framebuffer->height, DISPLAY_HEIGHT);

    framebuffer->stride = framebuffer->finfo.line_length / sizeof(uint16_t);

    framebuffer->memory = mmap(
        NULL,
        framebuffer->finfo.smem_len,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        framebuffer->fd,
        0
    );

    if (framebuffer->memory == MAP_FAILED)
    {
        perror("mmap");
        close(framebuffer->fd);
        framebuffer->fd = -1;
        framebuffer->memory = NULL;
        return;
    }
}

void framebuffer_destroy(framebuffer_t *const framebuffer)
{
    if (framebuffer == NULL)
    {
        return;
    }

    if (framebuffer->memory != NULL && framebuffer->memory != MAP_FAILED)
    {
        munmap(framebuffer->memory, framebuffer->finfo.smem_len);
        framebuffer->memory = NULL;
    }

    if (framebuffer->fd >= 0)
    {
        close(framebuffer->fd);
        framebuffer->fd = -1;
    }
}

void framebuffer_put_pixel(framebuffer_t *const framebuffer, const uint16_t x, const uint16_t y, const uint16_t color)
{
    if (x >= framebuffer->width || y >= framebuffer->height)
    {
        return;
    }

    framebuffer->memory[y * framebuffer->stride + x] = color;
}

void framebuffer_fill(framebuffer_t *const framebuffer, const uint16_t color)
{
    const int total = framebuffer->width * framebuffer->height;

    for (int i = 0; i < total; ++i)
    {
        framebuffer->memory[i] = color;
    }
}

void framebuffer_fill_rectangle(
    framebuffer_t *const framebuffer,
    const uint16_t x,
    const uint16_t y,
    const uint16_t width,
    const uint16_t height,
    const uint16_t color)
{
    uint16_t x_end = x + width;
    uint16_t y_end = y + height;

    if (x_end > framebuffer->width)
    {
        x_end = framebuffer->width;
    }

    if (y_end > framebuffer->height)
    {
        y_end = framebuffer->height;
    }

    for (uint16_t j = y; j < y_end; ++j)
    {
        uint16_t *const row = framebuffer->memory + j * framebuffer->stride;

        for (uint16_t i = x; i < x_end; ++i)
        {
            row[i] = color;
        }
    }
}

static void framebuffer_render_character(
    framebuffer_t *const framebuffer,
    const font_t *font,
    const uint16_t x,
    const uint16_t y,
    const char character,
    const uint16_t color)
{
    const uint8_t *glyph = font->glyph_data + ((unsigned char)character * font->glyph_height);

    for (uint8_t row = 0; row < font->glyph_height; ++row)
    {
        const uint8_t bits = glyph[row];

        for (uint8_t col = 0; col < font->glyph_width; ++col)
        {
            if (bits & (0x80 >> col))
            {
                framebuffer_put_pixel(framebuffer, x + col, y + row, color);
            }
        }
    }
}

void framebuffer_render_text(
    framebuffer_t *const framebuffer,
    const font_t *font,
    const uint16_t x,
    const uint16_t y,
    const char *text,
    const uint16_t color)
{
    uint16_t cursor_x = x;

    for (uint16_t i = 0; text[i] != '\0'; ++i)
    {
        framebuffer_render_character(framebuffer, font, cursor_x, y, text[i], color);
        cursor_x += font->glyph_width;
    }
}
