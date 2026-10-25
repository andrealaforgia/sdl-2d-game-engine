/**
 * @file texture.h
 * @brief Textures: loading, creating from pixels, and drawing sprites
 */

#ifndef CORE_GRAPHICS_TEXTURE_H_
#define CORE_GRAPHICS_TEXTURE_H_

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "graphics.h"
#include "graphics_context.h"

typedef struct {
  SDL_Texture* texture;
  int width;
  int height;
} texture_t, *texture_ptr;

/** A source rectangle on a texture, in whole pixels. */
typedef struct {
  int x, y, w, h;
} rect_t;

/**
 * How to draw a sprite. A zero-initialised value is the plain draw (pass NULL
 * for the same): no flip, fully opaque, upright, untinted.
 */
typedef struct {
  bool flip;       /**< mirror left to right */
  float alpha;     /**< 0..1; 0 is treated as 1 */
  float angle;     /**< degrees about the centre */
  bool tinted;     /**< multiply the colour by r, g, b */
  uint8_t r, g, b;
} draw_options_t;

/** Loads an image (PNG, JPEG, ...) keeping its own alpha. */
texture_t load_texture(SDL_Renderer* renderer, const char* filepath);

/** Loads an image, making one colour transparent. */
texture_t load_texture_with_colorkey(SDL_Renderer* renderer,
                                     const char* filepath, int r, int g, int b);

/**
 * Makes a texture from RGBA pixels (red in the lowest byte).
 * `additive` blends by adding, for glows.
 */
texture_t texture_from_pixels(const graphics_context_ptr graphics_context,
                              const uint32_t* pixels, int width, int height,
                              bool smooth, bool additive);

/** Smooth (linear) or crisp (nearest) scaling for this texture. */
void set_texture_smooth(texture_ptr tex, bool smooth);

void free_texture(texture_ptr tex);

/** Draws the whole texture, unscaled, at a pixel position. */
void render_texture(SDL_Renderer* renderer, const texture_ptr tex, int x,
                    int y);

/**
 * @brief Draws part of a texture into a rectangle of the view
 * @param src_rect The part to take; NULL for the whole texture
 * @param dst_rect Where and how big, in logical units, at sub-pixel precision
 * @param options How to draw it; NULL for the plain draw
 */
void render_sprite(const graphics_context_ptr graphics_context,
                   const texture_ptr tex, const rect_t* src_rect,
                   frect_t dst_rect, const draw_options_t* options);

rect_t make_rect(int x, int y, int w, int h);
frect_t make_frect(float x, float y, float w, float h);

#endif  // CORE_GRAPHICS_TEXTURE_H_
