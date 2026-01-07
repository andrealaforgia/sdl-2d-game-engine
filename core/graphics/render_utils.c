/**
 * @file render_utils.c
 * @brief Implementation of rendering utility functions
 */

#include "graphics.h"

#include <stdlib.h>

#include "render_utils.h"
#include "inline.h"

ALWAYS_INLINE double wrap(double value, double upper_bound) {
  if (value < 0) {
    return upper_bound - 1;
  }
  if (value > upper_bound - 1) {
    return 0;
  }
  return value;
}


ALWAYS_INLINE point_t
random_screen_point(const graphics_context_ptr graphics_context) {
  return random_point(graphics_context->screen_width,
                      graphics_context->screen_height);
}

ALWAYS_INLINE double wrap_x(const graphics_context_ptr graphics_context,
                            double x) {
  return wrap(x, graphics_context->screen_width);
}

ALWAYS_INLINE double wrap_y(const graphics_context_ptr graphics_context,
                            double y) {
  return wrap(y, graphics_context->screen_height);
}

ALWAYS_INLINE bool out_of_bounds(const graphics_context_ptr graphics_context,
                                 const point_ptr point) {
  return point->x < 0 || point->x >= graphics_context->screen_width ||
         point->y < 0 || point->y >= graphics_context->screen_height;
}
