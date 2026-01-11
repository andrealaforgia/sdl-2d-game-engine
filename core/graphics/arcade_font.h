/**
 * @file arcade_font.h
 * @brief Arcade bitmap font wrapper for easy text rendering
 *
 * Provides a simple interface for loading and using the arcade bitmap font
 * with predefined parameters optimized for the arcade-font.png sprite sheet.
 */

#ifndef CORE_GRAPHICS_ARCADE_FONT_H_
#define CORE_GRAPHICS_ARCADE_FONT_H_

#include "bitmap_font.h"
#include "graphics_context.h"

/**
 * Arcade font structure wrapping the bitmap font
 */
typedef struct {
  bitmap_font_t bitmap_font;
} arcade_font_t, *arcade_font_ptr;

/**
 * Load the arcade font with predefined parameters
 *
 * @param graphics_context Graphics context for texture creation
 * @return Initialized arcade font structure
 */
arcade_font_t load_arcade_font(const graphics_context_ptr graphics_context);

/**
 * Render arcade text at the specified position
 *
 * @param font Arcade font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 */
void render_arcade_text(const arcade_font_ptr font,
                        const graphics_context_ptr graphics_context,
                        const char* text, int x, int y, font_color_t color);

/**
 * Render scaled arcade text at the specified position
 *
 * @param font Arcade font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 * @param scale Scale factor (1 = normal size, 2 = double size, etc.)
 */
void render_arcade_text_scaled(const arcade_font_ptr font,
                               const graphics_context_ptr graphics_context,
                               const char* text, int x, int y,
                               font_color_t color, int scale);

/**
 * Render scaled arcade text with alpha transparency at the specified position
 *
 * @param font Arcade font to use
 * @param graphics_context Graphics context for rendering
 * @param text Text string to render (supports A-Z, 0-9, !, /, -, space)
 * @param x X position to render at
 * @param y Y position to render at
 * @param color Font color to use
 * @param scale Scale factor (1 = normal size, 2 = double size, etc.)
 * @param alpha Alpha value (0 = transparent, 255 = opaque)
 */
void render_arcade_text_scaled_alpha(const arcade_font_ptr font,
                                     const graphics_context_ptr graphics_context,
                                     const char* text, int x, int y,
                                     font_color_t color, int scale, int alpha);

/**
 * Get the width in pixels of rendered arcade text
 *
 * @param font Arcade font to measure with
 * @param text Text string to measure
 * @return Width in pixels
 */
int get_arcade_text_width(const arcade_font_ptr font, const char* text);

/**
 * Get the width in pixels of rendered scaled arcade text
 *
 * @param font Arcade font to measure with
 * @param text Text string to measure
 * @param scale Scale factor
 * @return Width in pixels
 */
int get_arcade_text_width_scaled(const arcade_font_ptr font, const char* text,
                                 int scale);

/**
 * Free arcade font resources
 *
 * @param font Font to free
 */
void free_arcade_font(arcade_font_ptr font);

#endif  // CORE_GRAPHICS_ARCADE_FONT_H_
