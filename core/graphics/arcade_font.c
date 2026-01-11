#include "arcade_font.h"

#include "logger.h"

// Arcade font parameters based on sprite sheet analysis
// 16x8 characters grid = 128/16 = 8px char width
// 7 color rows with 32px spacing = 224/7 = 32px color offset
#define ARCADE_FONT_CHAR_WIDTH 8
#define ARCADE_FONT_CHAR_HEIGHT 7
#define ARCADE_FONT_ROW_SPACING 8
#define ARCADE_FONT_COLOR_OFFSET 32
#define ARCADE_FONT_PATH "engine/core/graphics/fonts/arcade-font.png"

arcade_font_t load_arcade_font(const graphics_context_ptr graphics_context) {
  arcade_font_t arcade_font = {0};

  arcade_font.bitmap_font = load_bitmap_font(
      graphics_context, ARCADE_FONT_PATH, ARCADE_FONT_CHAR_WIDTH,
      ARCADE_FONT_CHAR_HEIGHT, ARCADE_FONT_ROW_SPACING,
      ARCADE_FONT_COLOR_OFFSET);

  if (!arcade_font.bitmap_font.texture.texture) {
    LOG_ERROR("Failed to load arcade font");
  } else {
    LOG_INFO("Arcade font loaded successfully");
  }

  return arcade_font;
}

void render_arcade_text(const arcade_font_ptr font,
                        const graphics_context_ptr graphics_context,
                        const char* text, int x, int y, font_color_t color) {
  render_bitmap_text(&font->bitmap_font, graphics_context, text, x, y, color);
}

void render_arcade_text_scaled(const arcade_font_ptr font,
                               const graphics_context_ptr graphics_context,
                               const char* text, int x, int y,
                               font_color_t color, int scale) {
  render_bitmap_text_scaled(&font->bitmap_font, graphics_context, text, x, y,
                            color, scale);
}

void render_arcade_text_scaled_alpha(const arcade_font_ptr font,
                                     const graphics_context_ptr graphics_context,
                                     const char* text, int x, int y,
                                     font_color_t color, int scale, int alpha) {
  render_bitmap_text_scaled_alpha(&font->bitmap_font, graphics_context, text, x, y,
                                  color, scale, alpha);
}

int get_arcade_text_width(const arcade_font_ptr font, const char* text) {
  return get_bitmap_text_width(&font->bitmap_font, text);
}

int get_arcade_text_width_scaled(const arcade_font_ptr font, const char* text,
                                 int scale) {
  return get_bitmap_text_width_scaled(&font->bitmap_font, text, scale);
}

void free_arcade_font(arcade_font_ptr font) {
  if (font) {
    free_bitmap_font(&font->bitmap_font);
  }
}
