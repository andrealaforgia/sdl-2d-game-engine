#include "graphics.h"
#include "graphics_context.h"
#include "drawing_primitives.h"
#include "display_manager.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "inline.h"
#include "logger.h"




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
