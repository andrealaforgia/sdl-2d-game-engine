#include "graphics.h"
#include "graphics_context.h"
#include "drawing_primitives.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "inline.h"
#include "logger.h"


static int get_display_count(void) { return SDL_GetNumVideoDisplays(); }

static SDL_DisplayMode* get_display_modes(int display_index,
                                          int* p_display_mode_count) {
  *p_display_mode_count = SDL_GetNumDisplayModes(display_index);
  if (*p_display_mode_count < 1) {
    LOG_SDL_ERROR("SDL_GetNumDisplayModes");
    return NULL;
  }

  SDL_DisplayMode* display_modes =
      calloc(*p_display_mode_count, sizeof(SDL_DisplayMode));

  for (int i = 0; i < *p_display_mode_count; i++) {
    if (SDL_GetDisplayMode(display_index, i, &display_modes[i]) != 0) {
      LOG_SDL_ERROR("SDL_GetDisplayMode");
      free(display_modes);
      return NULL;
    }
  }
  return display_modes;
}

void print_graphics_info(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    LOG_SDL_ERROR("SDL_Init");
    return;
  }
  int display_count = get_display_count();
  LOG_INFO_FMT("Number of available displays: %d", display_count);
  for (int display_index = 0; display_index < display_count; display_index++) {
    LOG_INFO_FMT("Display Index: %d", display_index);
    int display_mode_count;
    SDL_DisplayMode* display_modes =
        get_display_modes(display_index, &display_mode_count);
    if (display_modes) {
      for (int dm = 0; dm < display_mode_count; dm++) {
        LOG_INFO_FMT("Display Mode %d\tbpp %d\t%s\t%d x %d", dm,
                     SDL_BITSPERPIXEL(display_modes[dm].format),
                     SDL_GetPixelFormatName(display_modes[dm].format),
                     display_modes[dm].w, display_modes[dm].h);
      }
      free(display_modes);
    }
  }
  SDL_Quit();
}


graphics_context_t init_graphics_context(int display, int display_mode,
                                         window_mode_t window_mode,
                                         bool vsync) {
  graphics_context_t context = initialize_graphics_context(display, display_mode, window_mode, vsync);
  
  if (!context.window || !context.renderer) {
    LOG_ERROR("Failed to initialize graphics context");
    abort();
  }
  
  return context;
}


void toggle_fullscreen(const graphics_context_ptr graphics_context) {
  // Get current fullscreen state
  Uint32 flags = SDL_GetWindowFlags(graphics_context->window);
  bool is_fullscreen = (flags & SDL_WINDOW_FULLSCREEN) ||
                       (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);

  if (is_fullscreen) {
    // Switch to windowed mode
    if (SDL_SetWindowFullscreen(graphics_context->window, 0) == 0) {
      LOG_INFO("Switched to windowed mode (press F11 to toggle)");
    } else {
      LOG_SDL_ERROR("SDL_SetWindowFullscreen");
    }
  } else {
    // Switch to borderless fullscreen (faster toggle than true fullscreen)
    if (SDL_SetWindowFullscreen(graphics_context->window,
                                SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
      LOG_INFO("Switched to fullscreen mode (press F11 to toggle)");
    } else {
      LOG_SDL_ERROR("SDL_SetWindowFullscreen");
    }
  }
}


ALWAYS_INLINE double wrap(double value, double upper_bound) {
  if (value < 0) {
    return upper_bound - 1;
  }
  if (value > upper_bound - 1) {
    return 0;
  }
  return value;
}

ALWAYS_INLINE double wrap_x(const graphics_context_ptr graphics_context,
                            double x) {
  return wrap(x, graphics_context->screen_width);
}

ALWAYS_INLINE double wrap_y(const graphics_context_ptr graphics_context,
                            double y) {
  return wrap(y, graphics_context->screen_height);
}

ALWAYS_INLINE point_t
random_point(const graphics_context_ptr graphics_context) {
  return point(rand() % graphics_context->screen_width,
               rand() % graphics_context->screen_height);
}

ALWAYS_INLINE color_t random_color(void) {
  // Generate vibrant colors by using predefined bright color palette
  color_t colors[] = {
      0xFF0000,  // Red
      0x00FF00,  // Green
      0x0000FF,  // Blue
      0xFFFF00,  // Yellow
      0xFF00FF,  // Magenta
      0x00FFFF,  // Cyan
      0xFF8000,  // Orange
      0xFF0080,  // Pink
      0x80FF00,  // Lime
      0x0080FF,  // Sky blue
  };
  return colors[rand() % (sizeof(colors) / sizeof(color_t))];
}

ALWAYS_INLINE bool out_of_bounds(const graphics_context_ptr graphics_context,
                                 const point_ptr point) {
  return point->x < 0 || point->x >= graphics_context->screen_width ||
         point->y < 0 || point->y >= graphics_context->screen_height;
}
