#include "ttf_text.h"

#include "logger.h"

bool init_ttf_system(void) {
  if (TTF_Init() == -1) {
    LOG_ERROR_FMT("Failed to initialize SDL_ttf: %s", TTF_GetError());
    return false;
  }
  LOG_INFO("SDL_ttf initialized successfully");
  return true;
}

ttf_font_t load_ttf_font(const char* path, int point_size) {
  TTF_Font* font = TTF_OpenFont(path, point_size);
  if (!font) {
    LOG_ERROR_FMT("Failed to load font %s: %s", path, TTF_GetError());
    return NULL;
  }
  LOG_INFO_FMT("Loaded TTF font: %s at %dpt", path, point_size);
  return font;
}

SDL_Texture* render_ttf_text(const graphics_context_ptr graphics_context,
                             ttf_font_t font, const char* text,
                             SDL_Color color) {
  if (!font || !text || !graphics_context) {
    LOG_ERROR("Invalid parameters to render_ttf_text");
    return NULL;
  }

  // Render text to surface using blended mode for high quality
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  if (!surface) {
    LOG_ERROR_FMT("Failed to render text surface: %s", TTF_GetError());
    return NULL;
  }

  // Create texture from surface
  SDL_Texture* texture =
      SDL_CreateTextureFromSurface(graphics_context->renderer, surface);
  if (!texture) {
    LOG_ERROR_FMT("Failed to create texture from text surface: %s",
                  SDL_GetError());
  }

  // Free the surface (texture owns the pixel data now)
  SDL_FreeSurface(surface);

  return texture;
}

bool get_ttf_text_size(ttf_font_t font, const char* text, int* width,
                       int* height) {
  if (!font || !text) {
    LOG_ERROR("Invalid parameters to get_ttf_text_size");
    return false;
  }

  if (TTF_SizeText(font, text, width, height) != 0) {
    LOG_ERROR_FMT("Failed to get text size: %s", TTF_GetError());
    return false;
  }

  return true;
}

void free_ttf_font(ttf_font_t font) {
  if (font) {
    TTF_CloseFont(font);
  }
}

void quit_ttf_system(void) {
  TTF_Quit();
  LOG_INFO("SDL_ttf shut down");
}
