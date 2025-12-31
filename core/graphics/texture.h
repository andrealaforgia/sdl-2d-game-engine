/**
 * @file texture.h
 * @brief Texture loading and management for SDL
 */

#ifndef CORE_GRAPHICS_TEXTURE_H_
#define CORE_GRAPHICS_TEXTURE_H_

#include <SDL.h>
#include <stdbool.h>

typedef struct {
  SDL_Texture* texture;
  int width;
  int height;
} texture_t, *texture_ptr;

texture_t load_texture(SDL_Renderer* renderer, const char* filepath);
void free_texture(texture_ptr tex);
void render_texture(SDL_Renderer* renderer, const texture_ptr tex, int x,
                    int y);

#endif  // CORE_GRAPHICS_TEXTURE_H_
