/**
 * @file graphics.h
 * @brief Core graphics context and rendering primitives
 *
 * Provides the main graphics context managing SDL window and renderer,
 * along with functions for drawing lines, pixels, circles, and polygons.
 * Handles screen wrapping, coordinate transformations, and display modes.
 */

#ifndef CORE_GRAPHICS_GRAPHICS_H_
#define CORE_GRAPHICS_GRAPHICS_H_

#include <SDL.h>
#include <stdbool.h>

#include "color.h"
#include "geometry.h"
#include "window_mode.h"
#include "graphics_context.h"

typedef graphics_context_t* graphics_context_ptr;

#include "drawing_primitives.h"
#include "display_manager.h"

graphics_context_t init_graphics_context(int display, int display_mode,
                                         window_mode_t window_mode, bool vsync);
double wrap_x(const graphics_context_ptr graphics_context, double x);
double wrap_y(const graphics_context_ptr graphics_context, double y);
point_t random_point(const graphics_context_ptr graphics_context);
color_t random_color(void);
bool out_of_bounds(const graphics_context_ptr graphics_context,
                   const point_ptr point);
point_t random_point_around(const point_ptr point, int min_tolerance,
                            int max_tolerance);

#endif  // CORE_GRAPHICS_GRAPHICS_H_
