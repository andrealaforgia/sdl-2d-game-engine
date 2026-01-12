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

// Logging macros - simplified for context module
#define LOG_INFO(msg) printf("INFO: %s\n", msg)
#define LOG_INFO_FMT(fmt, ...) printf("INFO: " fmt "\n", __VA_ARGS__)
#define LOG_WARN(msg) printf("WARN: %s\n", msg)
#define LOG_WARN_FMT(fmt, ...) printf("WARN: " fmt "\n", __VA_ARGS__)
#define LOG_ERROR(msg) printf("ERROR: %s\n", msg)
#define LOG_SDL_ERROR(func) \
  printf("ERROR: %s failed: %s\n", func, SDL_GetError())

bool initialize_graphics_subsystems(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    LOG_SDL_ERROR("SDL_Init");
    return false;
  }

  // Initialize circle drawing lookup table for performance
  init_circle_lookup();

  // Set SDL hints for optimal performance
  if (!SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal")) {
    LOG_WARN("Failed to set Metal renderer hint");
  }

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

  SDL_ShowCursor(SDL_DISABLE);
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

SDL_Window* create_application_window(const char* title,
                                      window_mode_t window_mode, int display,
                                      int width, int height,
                                      SDL_DisplayMode* display_mode) {
  Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI;

  // Configure window flags based on mode
  switch (window_mode) {
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
      title, SDL_WINDOWPOS_CENTERED_DISPLAY(display),
      SDL_WINDOWPOS_CENTERED_DISPLAY(display), width, height, window_flags);

  if (!window) {
    LOG_SDL_ERROR("SDL_CreateWindow");
    return NULL;
  }

  // Set display mode for true fullscreen
  if (window_mode == FULLSCREEN) {
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

graphics_context_t initialize_graphics_context(int display, int display_mode,
                                               window_mode_t window_mode,
                                               bool vsync) {
  graphics_context_t context = {0};

  if (!initialize_graphics_subsystems()) {
    return context;  // Return empty context on failure
  }

  SDL_DisplayMode sdl_display_mode;
  if (!validate_display_configuration(&display, &display_mode,
                                      &sdl_display_mode)) {
    return context;
  }

  context.screen_width = sdl_display_mode.w;
  context.screen_height = sdl_display_mode.h;
  context.screen_center =
      point(context.screen_width / 2, context.screen_height / 2);

  context.window = create_application_window(
      "Asteroids", window_mode, display, context.screen_width,
      context.screen_height, &sdl_display_mode);
  if (!context.window) {
    return context;
  }

  context.renderer = create_application_renderer(context.window, vsync);
  if (!context.renderer) {
    SDL_DestroyWindow(context.window);
    context.window = NULL;
    return context;
  }

  int drawable_w, drawable_h;
  SDL_GL_GetDrawableSize(context.window, &drawable_w, &drawable_h);
  LOG_INFO_FMT("Drawable Size: w=%d h=%d", drawable_w, drawable_h);

  return context;
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

void shutdown_graphics_subsystems(void) {
  SDL_Quit();
}

void get_window_size(SDL_Window* window, int* width, int* height) {
  SDL_GetWindowSize(window, width, height);
}
