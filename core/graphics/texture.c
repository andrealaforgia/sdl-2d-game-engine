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

texture_t load_texture_with_colorkey(SDL_Renderer* renderer,
                                     const char* filepath, int r, int g,
                                     int b) {
  texture_t tex = {NULL, 0, 0};

  SDL_Surface* surface = IMG_Load(filepath);
  if (!surface) {
    LOG_ERROR_FMT("Failed to load image %s: %s", filepath, IMG_GetError());
    return tex;
  }

  // Set specified color as transparent
  Uint32 colorkey = SDL_MapRGB(surface->format, r, g, b);
  SDL_SetColorKey(surface, SDL_TRUE, colorkey);

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

  LOG_INFO_FMT("Loaded texture with colorkey: %s (%dx%d)", filepath, tex.width,
               tex.height);

  return tex;
}

void render_sprite(const graphics_context_ptr graphics_context,
                   const texture_ptr tex, const rect_t* src_rect,
                   const rect_t* dst_rect) {
  if (!graphics_context || !tex || !tex->texture) {
    return;
  }

  SDL_Rect src = {0, 0, tex->width, tex->height};
  SDL_Rect dst = {0, 0, tex->width, tex->height};

  if (src_rect) {
    src.x = src_rect->x;
    src.y = src_rect->y;
    src.w = src_rect->w;
    src.h = src_rect->h;
  }

  if (dst_rect) {
    dst.x = dst_rect->x;
    dst.y = dst_rect->y;
    dst.w = dst_rect->w;
    dst.h = dst_rect->h;
  }

  SDL_RenderCopy(graphics_context->renderer, tex->texture, &src, &dst);
}

void render_sprite_scaled(const graphics_context_ptr graphics_context,
                          const texture_ptr tex, const rect_t* src_rect, int x,
                          int y, int scale) {
  if (!graphics_context || !tex || !tex->texture || scale <= 0) {
    return;
  }

  SDL_Rect src = {0, 0, tex->width, tex->height};
  if (src_rect) {
    src.x = src_rect->x;
    src.y = src_rect->y;
    src.w = src_rect->w;
    src.h = src_rect->h;
  }

  SDL_Rect dst = {x, y, src.w * scale, src.h * scale};

  SDL_RenderCopy(graphics_context->renderer, tex->texture, &src, &dst);
}

void render_sprite_scaled_alpha(const graphics_context_ptr graphics_context,
                                const texture_ptr tex, const rect_t* src_rect, int x,
                                int y, int scale, int alpha) {
  if (!graphics_context || !tex || !tex->texture || scale <= 0) {
    return;
  }

  // Save current alpha mod
  Uint8 current_alpha;
  SDL_GetTextureAlphaMod(tex->texture, &current_alpha);
  
  // Set desired alpha (clamp to 0-255 range)
  Uint8 target_alpha = (alpha < 0) ? 0 : ((alpha > 255) ? 255 : (Uint8)alpha);
  SDL_SetTextureAlphaMod(tex->texture, target_alpha);

  SDL_Rect src = {0, 0, tex->width, tex->height};
  if (src_rect) {
    src.x = src_rect->x;
    src.y = src_rect->y;
    src.w = src_rect->w;
    src.h = src_rect->h;
  }

  SDL_Rect dst = {x, y, src.w * scale, src.h * scale};

  SDL_RenderCopy(graphics_context->renderer, tex->texture, &src, &dst);
  
  // Restore original alpha mod
  SDL_SetTextureAlphaMod(tex->texture, current_alpha);
}

void render_sprite_flipped(const graphics_context_ptr graphics_context,
                           const texture_ptr tex, const rect_t* src_rect,
                           const rect_t* dst_rect, flip_t flip) {
  if (!graphics_context || !tex || !tex->texture) {
    return;
  }

  SDL_Rect src = {0, 0, tex->width, tex->height};
  SDL_Rect dst = {0, 0, tex->width, tex->height};

  if (src_rect) {
    src.x = src_rect->x;
    src.y = src_rect->y;
    src.w = src_rect->w;
    src.h = src_rect->h;
  }

  if (dst_rect) {
    dst.x = dst_rect->x;
    dst.y = dst_rect->y;
    dst.w = dst_rect->w;
    dst.h = dst_rect->h;
  }

  SDL_RendererFlip sdl_flip = SDL_FLIP_NONE;
  if (flip & FLIP_HORIZONTAL) {
    sdl_flip |= SDL_FLIP_HORIZONTAL;
  }
  if (flip & FLIP_VERTICAL) {
    sdl_flip |= SDL_FLIP_VERTICAL;
  }

  SDL_RenderCopyEx(graphics_context->renderer, tex->texture, &src, &dst, 0.0,
                   NULL, sdl_flip);
}

void render_sprite_rotated(const graphics_context_ptr graphics_context,
                           const texture_ptr tex, const rect_t* src_rect,
                           const rect_t* dst_rect, double angle, flip_t flip) {
  if (!graphics_context || !tex || !tex->texture) {
    return;
  }

  SDL_Rect src = {0, 0, tex->width, tex->height};
  SDL_Rect dst = {0, 0, tex->width, tex->height};

  if (src_rect) {
    src.x = src_rect->x;
    src.y = src_rect->y;
    src.w = src_rect->w;
    src.h = src_rect->h;
  }

  if (dst_rect) {
    dst.x = dst_rect->x;
    dst.y = dst_rect->y;
    dst.w = dst_rect->w;
    dst.h = dst_rect->h;
  }

  SDL_RendererFlip sdl_flip = SDL_FLIP_NONE;
  if (flip & FLIP_HORIZONTAL) {
    sdl_flip |= SDL_FLIP_HORIZONTAL;
  }
  if (flip & FLIP_VERTICAL) {
    sdl_flip |= SDL_FLIP_VERTICAL;
  }

  SDL_RenderCopyEx(graphics_context->renderer, tex->texture, &src, &dst, angle,
                   NULL, sdl_flip);
}

rect_t make_rect(int x, int y, int w, int h) {
  rect_t rect = {x, y, w, h};
  return rect;
}

void set_logical_size(const graphics_context_ptr graphics_context, int width,
                      int height) {
  if (!graphics_context) {
    return;
  }
  SDL_RenderSetLogicalSize(graphics_context->renderer, width, height);
}
