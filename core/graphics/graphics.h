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
#include "display_manager.h"
#include "drawing_primitives.h"
#include "geometry.h"
#include "graphics_context.h"
#include "render_utils.h"
#include "window_mode.h"

graphics_context_t init_graphics_context(int display, int display_mode,
                                         window_mode_t window_mode, bool vsync);

#endif  // CORE_GRAPHICS_GRAPHICS_H_
