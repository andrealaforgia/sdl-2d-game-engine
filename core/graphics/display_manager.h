/**
 * @file display_manager.h
 * @brief Display mode validation and window management
 *
 * Contains functions for display enumeration, mode validation, and window
 * controls. Handles SDL display queries, fullscreen toggling, and display
 * information reporting.
 */

#ifndef CORE_GRAPHICS_DISPLAY_MANAGER_H_
#define CORE_GRAPHICS_DISPLAY_MANAGER_H_

#include <SDL.h>
#include <stdbool.h>

#include "graphics_context.h"

/**
 * @brief Get the number of available video displays
 * @return Number of displays, or negative on error
 */
int get_display_count(void);

/**
 * @brief Get available display modes for a specific display
 * @param display_index Display index to query
 * @param p_display_mode_count Output parameter for number of modes found
 * @return Array of display modes (must be freed), or NULL on error
 */
SDL_DisplayMode* get_display_modes(int display_index,
                                   int* p_display_mode_count);

/**
 * @brief Print comprehensive graphics and display information
 *
 * Outputs information about all available displays, their modes,
 * bit depths, formats, and resolutions. Useful for debugging.
 */
void print_graphics_info(void);

/**
 * @brief Toggle fullscreen mode for a graphics context window
 * @param graphics_context Graphics context containing the window to toggle
 *
 * Switches between windowed and borderless fullscreen modes.
 * Logs the current mode change for user feedback.
 */
void toggle_fullscreen(const graphics_context_ptr graphics_context);

#endif  // CORE_GRAPHICS_DISPLAY_MANAGER_H_
