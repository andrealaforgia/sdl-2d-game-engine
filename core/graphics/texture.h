/**
 * @file texture.h
 * @brief Texture loading and management for SDL
 */

#ifndef CORE_GRAPHICS_TEXTURE_H_
#define CORE_GRAPHICS_TEXTURE_H_

#include <SDL.h>
#include <stdbool.h>

#include "graphics.h"

typedef struct {
  SDL_Texture* texture;
  int width;
  int height;
} texture_t, *texture_ptr;

// Rectangle structure for sprite rendering
typedef struct {
  int x, y, w, h;
} rect_t;

// Flip flags for sprite rendering
typedef enum {
  FLIP_NONE = 0,
  FLIP_HORIZONTAL = 1,
  FLIP_VERTICAL = 2
} flip_t;

// Basic texture loading and management
texture_t load_texture(SDL_Renderer* renderer, const char* filepath);
texture_t load_texture_with_colorkey(SDL_Renderer* renderer, const char* filepath, 
                                     int r, int g, int b);
void free_texture(texture_ptr tex);
void render_texture(SDL_Renderer* renderer, const texture_ptr tex, int x, int y);

// Advanced sprite rendering functions
void render_sprite(const graphics_context_ptr graphics_context, 
                   const texture_ptr tex, 
                   const rect_t* src_rect, 
                   const rect_t* dst_rect);
void render_sprite_scaled(const graphics_context_ptr graphics_context,
                          const texture_ptr tex,
                          const rect_t* src_rect,
                          int x, int y, int scale);
void render_sprite_flipped(const graphics_context_ptr graphics_context,
                           const texture_ptr tex,
                           const rect_t* src_rect,
                           const rect_t* dst_rect,
                           flip_t flip);
void render_sprite_rotated(const graphics_context_ptr graphics_context,
                           const texture_ptr tex,
                           const rect_t* src_rect,
                           const rect_t* dst_rect,
                           double angle, flip_t flip);

// Utility functions
rect_t make_rect(int x, int y, int w, int h);
void set_logical_size(const graphics_context_ptr graphics_context, int width, int height);

#endif  // CORE_GRAPHICS_TEXTURE_H_
