/**
 * @file render_utils.h
 * @brief Rendering utility functions for colors, coordinates, and bounds
 * checking
 *
 * Contains utility functions for color generation, coordinate wrapping,
 * random point generation, and bounds validation. These utilities support
 * various rendering operations throughout the graphics system.
 */

#ifndef CORE_GRAPHICS_RENDER_UTILS_H_
#define CORE_GRAPHICS_RENDER_UTILS_H_

#include <stdbool.h>

#include "color.h"
#include "geometry.h"
#include "graphics_context.h"

// graphics_context_ptr typedef is provided by graphics_context.h

/**
 * @brief Generate a random point within graphics context bounds
 * @param graphics_context Graphics context containing screen dimensions
 * @return Random point within screen bounds
 */
point_t random_screen_point(const graphics_context_ptr graphics_context);

/**
 * @brief Wrap X coordinate within screen bounds using toroidal wrapping
 * @param graphics_context Graphics context containing screen width
 * @param x X coordinate to wrap
 * @return Wrapped X coordinate
 */
double wrap_x(const graphics_context_ptr graphics_context, double x);

/**
 * @brief Wrap Y coordinate within screen bounds using toroidal wrapping
 * @param graphics_context Graphics context containing screen height
 * @param y Y coordinate to wrap
 * @return Wrapped Y coordinate
 */
double wrap_y(const graphics_context_ptr graphics_context, double y);

/**
 * @brief Check if a point is outside graphics context bounds
 * @param graphics_context Graphics context containing screen dimensions
 * @param point Point to check
 * @return True if point is outside bounds, false otherwise
 */
bool out_of_bounds(const graphics_context_ptr graphics_context,
                   const point_ptr point);

#endif  // CORE_GRAPHICS_RENDER_UTILS_H_
