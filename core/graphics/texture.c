/**
 * @file texture.c
 * @brief Texture loading and management implementation
 */

#include "texture.h"

#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

texture_t load_texture(SDL_Renderer* renderer, const char* filepath) {
  texture_t tex = {NULL, 0, 0};

  SDL_Surface* surface = IMG_Load(filepath);
  if (!surface) {
    LOG_ERROR_FMT("Failed to load image %s: %s", filepath, IMG_GetError());
    return tex;
  }

  // Set black color (RGB 0,0,0) as transparent
  Uint32 black = SDL_MapRGB(surface->format, 0, 0, 0);
  SDL_SetColorKey(surface, SDL_TRUE, black);

  tex.texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!tex.texture) {
    LOG_ERROR_FMT("Failed to create texture from %s: %s", filepath,
                  SDL_GetError());
    SDL_FreeSurface(surface);
    return tex;
  }

  tex.width = surface->w;
  tex.height = surface->h;

  SDL_FreeSurface(surface);

  LOG_INFO_FMT("Loaded texture: %s (%dx%d)", filepath, tex.width, tex.height);

  return tex;
}

void free_texture(texture_ptr tex) {
  if (tex && tex->texture) {
    SDL_DestroyTexture(tex->texture);
    tex->texture = NULL;
    tex->width = 0;
    tex->height = 0;
  }
}

void render_texture(SDL_Renderer* renderer, const texture_ptr tex, int x,
                    int y) {
  if (!tex || !tex->texture) {
    return;
  }

  SDL_Rect dest = {x, y, tex->width, tex->height};
  SDL_RenderCopy(renderer, tex->texture, NULL, &dest);
}
