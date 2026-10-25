/**
 * @file graphics_context.c
 * @brief Graphics context implementation
 */

#include "graphics_context.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"
#include "graphics.h"
#include "viewport.h"

// Logging macros - simplified for context module
#define LOG_INFO(msg) printf("INFO: %s\n", msg)
#define LOG_INFO_FMT(fmt, ...) printf("INFO: " fmt "\n", __VA_ARGS__)
#define LOG_WARN(msg) printf("WARN: %s\n", msg)
#define LOG_WARN_FMT(fmt, ...) printf("WARN: " fmt "\n", __VA_ARGS__)
#define LOG_ERROR(msg) printf("ERROR: %s\n", msg)
#define LOG_SDL_ERROR(func) \
  printf("ERROR: %s failed: %s\n", func, SDL_GetError())

bool initialize_graphics_subsystems(void) {
#ifdef __EMSCRIPTEN__
  // Emscripten doesn't support haptic (force feedback)
  Uint32 sdl_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS |
                     SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER;
#else
  Uint32 sdl_flags = SDL_INIT_EVERYTHING;
#endif
  if (SDL_Init(sdl_flags) != 0) {
    LOG_SDL_ERROR("SDL_Init");
    return false;
  }

  // Initialize circle drawing lookup table for performance
  init_circle_lookup();

  // Set SDL hints for optimal performance
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
    LOG_WARN("Failed to set render scale quality hint");
  }

  if (!SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1")) {
    LOG_WARN("Failed to set render batching hint");
  } else {
    LOG_INFO("Render batching enabled");
  }

  if (!SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1")) {
    LOG_WARN("Failed to set framebuffer acceleration hint");
  }

  if (!SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0")) {
    LOG_WARN("Failed to set minimize on focus loss hint");
  }

  if (!SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1")) {
    LOG_WARN("Failed to set mouse relative mode hint");
  }

  if (!SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1")) {
    LOG_WARN("Failed to set mouse focus clickthrough hint");
  }

  return true;
}

bool validate_display_configuration(int* display, int* display_mode,
                                    SDL_DisplayMode* sdl_display_mode) {
  // Validate display index
  int num_displays = SDL_GetNumVideoDisplays();
  if (num_displays < 1) {
    LOG_SDL_ERROR("SDL_GetNumVideoDisplays");
    return false;
  }

  if (*display < 0 || *display >= num_displays) {
    LOG_WARN_FMT("Invalid display index %d (valid range: 0-%d)", *display,
                 num_displays - 1);
    LOG_INFO("Falling back to display 0");
    *display = 0;
  }

  // Validate display mode index
  int num_modes = SDL_GetNumDisplayModes(*display);
  if (num_modes < 1) {
    LOG_SDL_ERROR("SDL_GetNumDisplayModes");
    return false;
  }

  if (*display_mode < 0 || *display_mode >= num_modes) {
    LOG_WARN_FMT("Invalid display mode %d for display %d (valid range: 0-%d)",
                 *display_mode, *display, num_modes - 1);
    LOG_INFO("Falling back to display mode 0");
    *display_mode = 0;
  }

  if (SDL_GetDisplayMode(*display, *display_mode, sdl_display_mode) != 0) {
    LOG_SDL_ERROR("SDL_GetDisplayMode");
    return false;
  }

  LOG_INFO_FMT("Display Mode: w=%d h=%d refresh=%d", sdl_display_mode->w,
               sdl_display_mode->h, sdl_display_mode->refresh_rate);
  return true;
}

graphics_config_t default_graphics_config(const char* title) {
  graphics_config_t config = {title,   0, 0, 0, 0, WINDOWED, true, 0.0f};
  return config;
}

// A windowed game with no size asked fills the usable screen, borders
// included, instead of opening at some display mode's resolution.
static void fit_window_to_usable_screen(SDL_Window* window, int display) {
  SDL_Rect usable;
  if (SDL_GetDisplayUsableBounds(display, &usable) != 0) return;
  int top = 0, left = 0, bottom = 0, right = 0;
  if (SDL_GetWindowBordersSize(window, &top, &left, &bottom, &right) == 0) {
    SDL_SetWindowSize(window, usable.w - left - right,
                      usable.h - top - bottom);
    SDL_SetWindowPosition(window, usable.x + left, usable.y + top);
  }
}

SDL_Window* create_application_window(const graphics_config_t* config,
                                      SDL_DisplayMode* display_mode) {
  Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI;
  int width = config->width > 0 ? config->width : display_mode->w;
  int height = config->height > 0 ? config->height : display_mode->h;
  bool fill_screen = config->window_mode == WINDOWED &&
                     (config->width <= 0 || config->height <= 0);

  // Configure window flags based on mode
  switch (config->window_mode) {
    case WINDOWED:
      window_flags |= SDL_WINDOW_RESIZABLE;
      LOG_INFO("Window mode: Windowed");
      break;
    case FULLSCREEN:
      window_flags |= SDL_WINDOW_FULLSCREEN;
      LOG_INFO("Window mode: Fullscreen");
      break;
    case BORDERLESS:
      window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS;
      LOG_INFO("Window mode: Borderless");
      break;
    case MAXIMIZED:
      window_flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
      LOG_INFO("Window mode: Maximized");
      break;
  }

  SDL_Window* window = SDL_CreateWindow(
      config->title, SDL_WINDOWPOS_CENTERED_DISPLAY(config->display),
      SDL_WINDOWPOS_CENTERED_DISPLAY(config->display), width, height,
      window_flags);

  if (!window) {
    LOG_SDL_ERROR("SDL_CreateWindow");
    return NULL;
  }

  if (fill_screen) fit_window_to_usable_screen(window, config->display);

  // Set display mode for true fullscreen
  if (config->window_mode == FULLSCREEN) {
    if (SDL_SetWindowDisplayMode(window, display_mode) != 0) {
      LOG_SDL_ERROR("SDL_SetWindowDisplayMode");
      SDL_DestroyWindow(window);
      return NULL;
    }
  }

  return window;
}

SDL_Renderer* create_application_renderer(SDL_Window* window, bool vsync) {
  // Try hardware acceleration first, fall back to software if needed
  Uint32 renderer_flags = SDL_RENDERER_ACCELERATED;
  if (vsync) {
    renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, renderer_flags);

  if (!renderer) {
    LOG_WARN("Hardware-accelerated renderer failed, trying software renderer");
    LOG_SDL_ERROR("SDL_CreateRenderer (hardware)");

    // Try software renderer as fallback
    renderer_flags = SDL_RENDERER_SOFTWARE;
    if (vsync) {
      renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    renderer = SDL_CreateRenderer(window, -1, renderer_flags);

    if (!renderer) {
      LOG_SDL_ERROR("SDL_CreateRenderer (software fallback)");
      LOG_ERROR("Failed to create any renderer - aborting");
      return NULL;
    } else {
      LOG_INFO("Using software renderer (performance may be reduced)");
    }
  }

  // Log renderer info for debugging
  SDL_RendererInfo renderer_info;
  if (SDL_GetRendererInfo(renderer, &renderer_info) == 0) {
    LOG_INFO_FMT("Renderer: %s", renderer_info.name);
    if (renderer_info.flags & SDL_RENDERER_ACCELERATED) {
      LOG_INFO("Renderer: Hardware-accelerated");
    } else {
      LOG_INFO("Renderer: Software");
    }

    if (renderer_info.flags & SDL_RENDERER_PRESENTVSYNC) {
      LOG_INFO("Renderer: V-Sync enabled");
    } else {
      LOG_INFO("Renderer: V-Sync disabled");
    }

    if (renderer_info.flags & SDL_RENDERER_TARGETTEXTURE) {
      LOG_INFO("Renderer: Target texture support");
    }
  }

  return renderer;
}

graphics_context_t initialize_graphics_context(
    const graphics_config_t* config) {
  graphics_context_t context = {0};
  graphics_config_t settings = *config;

  if (!initialize_graphics_subsystems()) {
    return context;  // Return empty context on failure
  }

  SDL_DisplayMode sdl_display_mode;
  if (!validate_display_configuration(&settings.display,
                                      &settings.display_mode,
                                      &sdl_display_mode)) {
    return context;
  }

  context.window = create_application_window(&settings, &sdl_display_mode);
  if (!context.window) {
    return context;
  }

  context.renderer = create_application_renderer(context.window, settings.vsync);
  if (!context.renderer) {
    SDL_DestroyWindow(context.window);
    context.window = NULL;
    return context;
  }
  SDL_SetRenderDrawBlendMode(context.renderer, SDL_BLENDMODE_BLEND);
  if (settings.window_mode != WINDOWED) SDL_ShowCursor(SDL_DISABLE);

  get_window_size(context.window, &context.screen_width,
                  &context.screen_height);
  context.screen_center =
      point(context.screen_width / 2, context.screen_height / 2);
  context.view_height = settings.view_height;

  int drawable_w, drawable_h;
  SDL_GL_GetDrawableSize(context.window, &drawable_w, &drawable_h);
  LOG_INFO_FMT("Drawable Size: w=%d h=%d", drawable_w, drawable_h);

  return context;
}

void begin_frame(graphics_context_t* context, uint8_t r, uint8_t g,
                 uint8_t b) {
  int out_w = 1, out_h = 1;
  SDL_GetRendererOutputSize(context->renderer, &out_w, &out_h);
  viewport_fit(out_w, out_h, context->view_height, &context->view_scale,
               &context->view_width);
  SDL_RenderSetScale(context->renderer, context->view_scale,
                     context->view_scale);
  SDL_SetRenderDrawColor(context->renderer, r, g, b, 255);
  SDL_RenderClear(context->renderer);
}

bool save_frame_bmp(const graphics_context_t* context, const char* path) {
  int w = 0, h = 0;
  SDL_GetRendererOutputSize(context->renderer, &w, &h);
  SDL_Surface* surface =
      SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
  if (!surface) return false;
  bool ok = SDL_RenderReadPixels(context->renderer, NULL,
                                 SDL_PIXELFORMAT_ARGB8888, surface->pixels,
                                 surface->pitch) == 0 &&
            SDL_SaveBMP(surface, path) == 0;
  SDL_FreeSurface(surface);
  return ok;
}

void terminate_graphics_context(graphics_context_t* context) {
  if (context->renderer) {
    SDL_DestroyRenderer(context->renderer);
    context->renderer = NULL;
  }

  if (context->window) {
    SDL_DestroyWindow(context->window);
    context->window = NULL;
  }

  SDL_Quit();
}

void shutdown_graphics_subsystems(void) { SDL_Quit(); }

void get_window_size(SDL_Window* window, int* width, int* height) {
  SDL_GetWindowSize(window, width, height);
}
