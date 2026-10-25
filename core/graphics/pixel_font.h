/**
 * @file pixel_font.h
 * @brief A built-in 5x7 pixel font in any colour and scale
 *
 * Needs no asset: the glyphs are built into a texture when the font is made.
 * Digits, capitals and a little punctuation; lowercase draws as capitals.
 * For text that is not part of the art (HUDs, debug, titles).
 */

#ifndef CORE_GRAPHICS_PIXEL_FONT_H_
#define CORE_GRAPHICS_PIXEL_FONT_H_

#include <stdint.h>

#include "graphics_context.h"
#include "texture.h"

typedef struct {
  texture_t atlas;
} pixel_font_t, *pixel_font_ptr;

pixel_font_t build_pixel_font(const graphics_context_ptr graphics_context);

/** Draws `text` with its top-left corner at (x, y), `scale` units a pixel. */
void render_pixel_text(const pixel_font_ptr font,
                       const graphics_context_ptr graphics_context, float x,
                       float y, const char* text, float scale, uint8_t r,
                       uint8_t g, uint8_t b);

/** The width `text` takes at `scale`, without the gap after the last glyph. */
float pixel_text_width(const char* text, float scale);

void free_pixel_font(pixel_font_ptr font);

#endif  // CORE_GRAPHICS_PIXEL_FONT_H_
