#include "bitmap_font.h"

#include <ctype.h>
#include <string.h>

#include "logger.h"
#include "texture.h"

// Get the sprite coordinates for a character
static bool get_char_sprite_rect(char c, int* out_x, int* out_y, int char_width,
                                 int char_height, int row_spacing) {
  (void)char_height;  // Suppress unused parameter warning
  c = toupper(c);

  // Row 1: A-O (0-14)
  if (c >= 'A' && c <= 'O') {
    *out_x = (c - 'A') * char_width;
    *out_y = 0;
    return true;
  }

  // Row 2: P-Z and ! (0-11)
  if (c >= 'P' && c <= 'Z') {
    *out_x = (c - 'P') * char_width;
    *out_y = row_spacing;
    return true;
  }
  if (c == '!') {
    *out_x = 11 * char_width;
    *out_y = row_spacing;
    return true;
  }

  // Row 3: 0-9, /, - (0-11)
  if (c >= '0' && c <= '9') {
    *out_x = (c - '0') * char_width;
    *out_y = row_spacing * 2;
    return true;
  }
  if (c == '/') {
    *out_x = 10 * char_width;
    *out_y = row_spacing * 2;
    return true;
  }
  if (c == '-') {
    *out_x = 11 * char_width;
    *out_y = row_spacing * 2;
    return true;
  }

  // Space character - no sprite to render
  if (c == ' ') {
    return false;
  }

  LOG_WARN_FMT("Unsupported character in bitmap font: '%c'", c);
  return false;
}

bitmap_font_t load_bitmap_font(const graphics_context_ptr graphics_context,
                               const char* sprite_sheet_path, int char_width,
                               int char_height, int row_spacing,
                               int color_offset) {
  bitmap_font_t font = {0};

  font.texture = load_texture(graphics_context->renderer, sprite_sheet_path);
  font.char_width = char_width;
  font.char_height = char_height;
  font.row_spacing = row_spacing;
  font.color_offset = color_offset;

  if (!font.texture.texture) {
    LOG_ERROR_FMT("Failed to load bitmap font sprite sheet: %s",
                  sprite_sheet_path);
  } else {
    LOG_INFO_FMT("Loaded bitmap font: %s", sprite_sheet_path);
  }

  return font;
}

void render_bitmap_text(const bitmap_font_ptr font,
                        const graphics_context_ptr graphics_context,
                        const char* text, int x, int y, font_color_t color) {
  if (!font || !font->texture.texture || !text || !graphics_context) {
    return;
  }

  int cursor_x = x;
  int color_y_offset = color * font->color_offset;

  for (size_t i = 0; i < strlen(text); i++) {
    char c = text[i];

    // Handle space character
    if (c == ' ') {
      cursor_x += font->char_width;
      continue;
    }

    int char_x, char_y;
    if (get_char_sprite_rect(c, &char_x, &char_y, font->char_width,
                             font->char_height, font->row_spacing)) {
      SDL_Rect src_rect = {char_x, char_y + color_y_offset, font->char_width,
                           font->char_height};

      SDL_Rect dest_rect = {cursor_x, y, font->char_width, font->char_height};

      SDL_RenderCopy(graphics_context->renderer, font->texture.texture,
                     &src_rect, &dest_rect);
    }

    cursor_x += font->char_width;
  }
}

void render_bitmap_text_scaled(const bitmap_font_ptr font,
                               const graphics_context_ptr graphics_context,
                               const char* text, int x, int y,
                               font_color_t color, int scale) {
  if (!font || !font->texture.texture || !text || !graphics_context ||
      scale <= 0) {
    return;
  }

  int cursor_x = x;
  int color_y_offset = color * font->color_offset;
  int scaled_char_width = font->char_width * scale;

  for (size_t i = 0; i < strlen(text); i++) {
    char c = text[i];

    // Handle space character
    if (c == ' ') {
      cursor_x += scaled_char_width;
      continue;
    }

    int char_x, char_y;
    if (get_char_sprite_rect(c, &char_x, &char_y, font->char_width,
                             font->char_height, font->row_spacing)) {
      rect_t src_rect = make_rect(char_x, char_y + color_y_offset,
                                  font->char_width, font->char_height);

      render_sprite_scaled(graphics_context, &font->texture, &src_rect,
                           cursor_x, y, scale);
    }

    cursor_x += scaled_char_width;
  }
}

void render_bitmap_text_scaled_alpha(const bitmap_font_ptr font,
                                     const graphics_context_ptr graphics_context,
                                     const char* text, int x, int y,
                                     font_color_t color, int scale, int alpha) {
  if (!font || !font->texture.texture || !text || !graphics_context ||
      scale <= 0) {
    return;
  }

  int cursor_x = x;
  int color_y_offset = color * font->color_offset;
  int scaled_char_width = font->char_width * scale;

  for (size_t i = 0; i < strlen(text); i++) {
    char c = text[i];

    // Handle space character
    if (c == ' ') {
      cursor_x += scaled_char_width;
      continue;
    }

    int char_x, char_y;
    if (get_char_sprite_rect(c, &char_x, &char_y, font->char_width,
                             font->char_height, font->row_spacing)) {
      rect_t src_rect = make_rect(char_x, char_y + color_y_offset,
                                  font->char_width, font->char_height);

      render_sprite_scaled_alpha(graphics_context, &font->texture, &src_rect,
                                 cursor_x, y, scale, alpha);
    }

    cursor_x += scaled_char_width;
  }
}

int get_bitmap_text_width(const bitmap_font_ptr font, const char* text) {
  if (!font || !text) {
    return 0;
  }

  return strlen(text) * font->char_width;
}

int get_bitmap_text_width_scaled(const bitmap_font_ptr font, const char* text,
                                 int scale) {
  if (!font || !text || scale <= 0) {
    return 0;
  }

  return strlen(text) * font->char_width * scale;
}

void free_bitmap_font(bitmap_font_ptr font) {
  if (font && font->texture.texture) {
    free_texture(&font->texture);
  }
}
