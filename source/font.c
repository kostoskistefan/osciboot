#include "font.h"
#include "font8x16.h"

const font_t font8x16 = {
    .glyph_data = (const uint8_t *) font8x16_data,
    .glyph_width = 8,
    .glyph_height = 16,
    .glyphs_per_row = 16
};
