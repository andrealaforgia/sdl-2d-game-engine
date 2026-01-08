/**
 * @file drawing_primitives.h
 * @brief Drawing primitive functions for lines, pixels, circles, and polygons
 *
 * Contains all basic drawing operations that use the graphics context to render
 * geometric primitives to the screen. Separated from graphics.c for better
 * modularity.
 */

#ifndef CORE_GRAPHICS_DRAWING_PRIMITIVES_H_
#define CORE_GRAPHICS_DRAWING_PRIMITIVES_H_

#include <SDL.h>
#include <stdint.h>

#include "color.h"
#include "geometry.h"
#include "graphics_context.h"

/**
 * @brief Draw a line between two coordinate points
 * @param graphics_context Graphics context containing renderer
 * @param x1 Starting x coordinate
 * @param y1 Starting y coordinate
 * @param x2 Ending x coordinate
 * @param y2 Ending y coordinate
 * @param color Line color
 */
void draw_line(const graphics_context_ptr graphics_context, int x1, int y1,
               int x2, int y2, color_t color);

/**
 * @brief Draw a thick line with enhanced visibility
 * @param graphics_context Graphics context containing renderer
 * @param x1 Starting x coordinate
 * @param y1 Starting y coordinate
 * @param x2 Ending x coordinate
 * @param y2 Ending y coordinate
 * @param color Line color
 */
void draw_thick_line(const graphics_context_ptr graphics_context, int x1,
                     int y1, int x2, int y2, color_t color);

/**
 * @brief Draw a line between two point structures
 * @param graphics_context Graphics context containing renderer
 * @param p1 Starting point
 * @param p2 Ending point
 * @param color Line color
 */
void draw_line_between_points(const graphics_context_ptr graphics_context,
                              const point_ptr p1, const point_ptr p2,
                              color_t color);

/**
 * @brief Draw a single pixel at specified coordinates
 * @param graphics_context Graphics context containing renderer
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Pixel color
 */
void draw_pixel(const graphics_context_ptr graphics_context, int x, int y,
                color_t color);

/**
 * @brief Draw a single pixel at a point structure location
 * @param graphics_context Graphics context containing renderer
 * @param p Point location
 * @param color Pixel color
 */
void draw_point(const graphics_context_ptr graphics_context, const point_ptr p,
                color_t color);

/**
 * @brief Draw a fat pixel (5x5 square) for enhanced visibility
 * @param graphics_context Graphics context containing renderer
 * @param p Point location center
 * @param color Pixel color
 */
void draw_fat_pixel(const graphics_context_ptr graphics_context,
                    const point_ptr p, color_t color);

/**
 * @brief Draw a circle using optimized point rendering
 * @param graphics_context Graphics context containing renderer
 * @param centreX Circle center x coordinate
 * @param centreY Circle center y coordinate
 * @param radius Circle radius
 * @param color Circle color
 */
void draw_circle(const graphics_context_ptr graphics_context, int32_t centreX,
                 int32_t centreY, int32_t radius, color_t color);

/**
 * @brief Draw a filled polygon using triangle fan approach
 * @param graphics_context Graphics context containing renderer
 * @param points Array of polygon vertices
 * @param num_points Number of vertices in polygon
 * @param fill_color Fill color for polygon
 */
void draw_filled_polygon(const graphics_context_ptr graphics_context,
                         const SDL_Point* points, int num_points,
                         color_t fill_color);

/**
 * @brief Initialize circle drawing lookup tables for performance
 *
 * Must be called before using draw_circle() function. Creates pre-calculated
 * sin/cos lookup table for smooth circle rendering.
 */
void init_circle_lookup(void);

#endif  // CORE_GRAPHICS_DRAWING_PRIMITIVES_H_
