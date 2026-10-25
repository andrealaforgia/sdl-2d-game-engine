#include "pixel_font.h"

#include <stdlib.h>
#include <string.h>

#include "font5x7.h"

#define GLYPH_CELL_W 6
#define GLYPH_CELL_H 8
#define GLYPH_FIRST 32
#define GLYPH_COUNT 96
#define GLYPH_COLUMNS 16

pixel_font_t build_pixel_font(const graphics_context_ptr graphics_context) {
  pixel_font_t font = {{NULL, 0, 0}};
  int width = GLYPH_COLUMNS * GLYPH_CELL_W;
  int rows = (GLYPH_COUNT + GLYPH_COLUMNS - 1) / GLYPH_COLUMNS;
  int height = rows * GLYPH_CELL_H;
  uint32_t* pixels = calloc((size_t)width * (size_t)height, sizeof(uint32_t));
  if (!pixels) return font;
  for (int i = 0; i < GLYPH_COUNT; i++) {
    char c = (char)(GLYPH_FIRST + i);
    int ox = (i % GLYPH_COLUMNS) * GLYPH_CELL_W;
    int oy = (i / GLYPH_COLUMNS) * GLYPH_CELL_H;
    for (int row = 0; row < FONT5X7_HEIGHT; row++)
      for (int col = 0; col < FONT5X7_WIDTH; col++)
        if (font5x7_pixel(c, row, col))
          pixels[(oy + row) * width + ox + col] = 0xffffffffu;
  }
  font.atlas = texture_from_pixels(graphics_context, pixels, width, height,
                                   false, false);
  free(pixels);
  return font;
}

void render_pixel_text(const pixel_font_ptr font,
                       const graphics_context_ptr graphics_context, float x,
                       float y, const char* text, float scale, uint8_t r,
                       uint8_t g, uint8_t b) {
  if (!font || !font->atlas.texture || !text) return;
  draw_options_t options = {false, 1.0f, 0.0f, true, r, g, b};
  float cursor = x;
  for (const char* p = text; *p; p++) {
    int index = (unsigned char)*p - GLYPH_FIRST;
    if (*p >= 'a' && *p <= 'z') index -= 32;
    if (index >= 0 && index < GLYPH_COUNT && *p != ' ') {
      rect_t source = {(index % GLYPH_COLUMNS) * GLYPH_CELL_W,
                       (index / GLYPH_COLUMNS) * GLYPH_CELL_H, FONT5X7_WIDTH,
                       FONT5X7_HEIGHT};
      frect_t target = {cursor, y, FONT5X7_WIDTH * scale,
                        FONT5X7_HEIGHT * scale};
      render_sprite(graphics_context, &font->atlas, &source, target, &options);
    }
    cursor += GLYPH_CELL_W * scale;
  }
}

float pixel_text_width(const char* text, float scale) {
  return (float)strlen(text) * GLYPH_CELL_W * scale - scale;
}

void free_pixel_font(pixel_font_ptr font) { free_texture(&font->atlas); }
