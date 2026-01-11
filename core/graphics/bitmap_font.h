/**
 * @file bitmap_font.h
 * @brief Bitmap font rendering system using sprite sheets
 *
 * Provides functions for rendering text using bitmap font sprite sheets.
 * Supports multiple colors by using different rows in the sprite sheet.
 */

#ifndef CORE_GRAPHICS_BITMAP_FONT_H_
#define CORE_GRAPHICS_BITMAP_FONT_H_

#include <SDL.h>
#include <stdbool.h>

#include "graphics.h"
#include "graphics_context.h"
#include "texture.h"

/**
 * Font color enumeration matching sprite sheet layout
 */
typedef enum {
  FONT_COLOR_WHITE = 0,
  FONT_COLOR_RED = 1,
  FONT_COLOR_PINK = 2,
  FONT_COLOR_CYAN = 3,
  FONT_COLOR_GOLD = 4,
  FONT_COLOR_PEACH = 5,
  FONT_COLOR_YELLOW = 6,
  FONT_COLOR_GREEN = 7
} font_color_t;

/**
 * Bitmap font structure
 */
typedef struct {
  texture_t texture;
  int char_width;    // Width of each character in pixels
  int char_height;   // Height of each character in pixels
  int row_spacing;   // Vertical spacing between character rows
  int color_offset;  // Vertical offset between color variations
} bitmap_font_t, *bitmap_font_ptr;

/**
 * Load a bitmap font from a sprite sheet
 *
 * @param graphics_context Graphics context for texture creation
 * @param sprite_sheet_path Path to the sprite sheet image
 * @param char_width Width of each character in pixels (default: 8)
 * @param char_height Height of each character in pixels (default: 7)
 * @param row_spacing Vertical spacing between character rows (default: 8)
 * @param color_offset Vertical offset between color variations (default: 32)
 * @return Initialized bitmap font structure
 */
bitmap_font_t load_bitmap_font(const graphics_context_ptr graphics_context,
                               const char* sprite_sheet_path, int char_width,
                               int char_height, int row_spacing,
                               int color_offset);

/**
 * Render text at the specified position using the bitmap font
 *
 * @param font Bitmap font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 */
void render_bitmap_text(const bitmap_font_ptr font,
                        const graphics_context_ptr graphics_context,
                        const char* text, int x, int y, font_color_t color);

/**
 * Render scaled text at the specified position using the bitmap font
 *
 * @param font Bitmap font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 * @param scale Scale factor (1 = normal size, 2 = double size, etc.)
 */
void render_bitmap_text_scaled(const bitmap_font_ptr font,
                               const graphics_context_ptr graphics_context,
                               const char* text, int x, int y,
                               font_color_t color, int scale);

/**
 * Render scaled text with alpha transparency at the specified position
 *
 * @param font Bitmap font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 * @param scale Scale factor (1 = normal size, 2 = double size, etc.)
 * @param alpha Alpha value (0 = transparent, 255 = opaque)
 */
void render_bitmap_text_scaled_alpha(const bitmap_font_ptr font,
                                     const graphics_context_ptr graphics_context,
                                     const char* text, int x, int y,
                                     font_color_t color, int scale, int alpha);

/**
 * Get the width in pixels of the rendered text
 *
 * @param font Bitmap font to measure with
 * @param text Text string to measure
 * @return Width in pixels
 */
int get_bitmap_text_width(const bitmap_font_ptr font, const char* text);

/**
 * Get the width in pixels of the rendered scaled text
 *
 * @param font Bitmap font to measure with
 * @param text Text string to measure
 * @param scale Scale factor
 * @return Width in pixels
 */
int get_bitmap_text_width_scaled(const bitmap_font_ptr font, const char* text,
                                 int scale);

/**
 * Free bitmap font resources
 *
 * @param font Font to free
 */
void free_bitmap_font(bitmap_font_ptr font);

#endif  // CORE_GRAPHICS_BITMAP_FONT_H_
