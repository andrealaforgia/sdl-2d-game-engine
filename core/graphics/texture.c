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

texture_t texture_from_pixels(const graphics_context_ptr graphics_context,
                              const uint32_t* pixels, int width, int height,
                              bool smooth, bool additive) {
  texture_t tex = {NULL, 0, 0};
  SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
      (void*)pixels, width, height, 32, width * 4, 0x000000ffu, 0x0000ff00u,
      0x00ff0000u, 0xff000000u);
  if (!surface) return tex;
  tex.texture =
      SDL_CreateTextureFromSurface(graphics_context->renderer, surface);
  SDL_FreeSurface(surface);
  if (!tex.texture) return tex;
  tex.width = width;
  tex.height = height;
  if (additive) SDL_SetTextureBlendMode(tex.texture, SDL_BLENDMODE_ADD);
  set_texture_smooth(&tex, smooth);
  return tex;
}

void set_texture_smooth(texture_ptr tex, bool smooth) {
  if (!tex || !tex->texture) return;
  SDL_SetTextureScaleMode(tex->texture, smooth ? SDL_ScaleModeLinear
                                                : SDL_ScaleModeNearest);
}

void render_sprite(const graphics_context_ptr graphics_context,
                   const texture_t* tex, const rect_t* src_rect,
                   frect_t dst_rect, const draw_options_t* options) {
  if (!graphics_context || !tex || !tex->texture) return;
  SDL_Rect source;
  if (src_rect) {
    source.x = src_rect->x;
    source.y = src_rect->y;
    source.w = src_rect->w;
    source.h = src_rect->h;
  }
  SDL_FRect target = {dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h};
  double angle = 0.0;
  SDL_RendererFlip flip = SDL_FLIP_NONE;
  Uint8 alpha = 255;
  Uint8 r = 255, g = 255, b = 255;
  if (options) {
    angle = options->angle;
    if (options->flip) flip = SDL_FLIP_HORIZONTAL;
    if (options->alpha > 0.0f)
      alpha = (Uint8)(clampf(options->alpha, 0.0f, 1.0f) * 255.0f);
    if (options->tinted) {
      r = options->r;
      g = options->g;
      b = options->b;
    }
  }
  SDL_SetTextureAlphaMod(tex->texture, alpha);
  SDL_SetTextureColorMod(tex->texture, r, g, b);
  SDL_RenderCopyExF(graphics_context->renderer, tex->texture,
                    src_rect ? &source : NULL, &target, angle, NULL, flip);
}

rect_t make_rect(int x, int y, int w, int h) {
  rect_t rect = {x, y, w, h};
  return rect;
}

frect_t make_frect(float x, float y, float w, float h) {
  frect_t rect = {x, y, w, h};
  return rect;
}
