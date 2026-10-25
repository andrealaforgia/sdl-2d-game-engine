/**
 * @file viewport.h
 * @brief A logical view of fixed height whose width follows the window
 *
 * Games draw in logical units: the view is always `view_height` units tall
 * and as wide as the window's shape allows, so a wide window shows more of
 * the world instead of black bars. Pure maths, tested without a window.
 */

#ifndef CORE_GRAPHICS_VIEWPORT_H_
#define CORE_GRAPHICS_VIEWPORT_H_

/**
 * @brief Fit a view of `view_height` units to an output of pixels
 * @param scale Receives pixels per logical unit
 * @param view_width Receives the logical width
 *
 * A `view_height` of 0 or less means one unit per pixel.
 */
void viewport_fit(int output_width, int output_height, float view_height,
                  float* scale, float* view_width);

#endif  // CORE_GRAPHICS_VIEWPORT_H_
