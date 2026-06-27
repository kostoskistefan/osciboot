#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct font_t
{
    const uint8_t *glyph_data;

    uint8_t glyph_width;
    uint8_t glyph_height;

    uint8_t glyphs_per_row;
} font_t;

extern const font_t font8x16;

#endif
