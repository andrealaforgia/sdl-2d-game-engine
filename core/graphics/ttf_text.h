/**
 * @file ttf_text.h
 * @brief TrueType font text rendering system using SDL_ttf
 *
 * Provides functions for loading TTF fonts and rendering text using SDL_ttf.
 * Supports antialiased text rendering with custom colors.
 */

#ifndef CORE_GRAPHICS_TTF_TEXT_H_
#define CORE_GRAPHICS_TTF_TEXT_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#include "graphics.h"

/**
 * Opaque handle to a TTF font
 */
typedef TTF_Font* ttf_font_t;

/**
 * Initialize the SDL_ttf subsystem
 * Must be called once before using any TTF functions
 *
 * @return true on success, false on failure
 */
bool init_ttf_system(void);

/**
 * Load a TrueType font from a file
 *
 * @param path Path to the .ttf font file
 * @param point_size Font size in points
 * @return Font handle, or NULL on failure
 */
ttf_font_t load_ttf_font(const char* path, int point_size);

/**
 * Render text to a texture using the specified font and color
 * The caller is responsible for destroying the returned texture
 *
 * @param graphics_context Graphics context containing the renderer
 * @param font Font to use for rendering
 * @param text Text string to render
 * @param color Text color
 * @return Texture containing the rendered text, or NULL on failure
 */
SDL_Texture* render_ttf_text(const graphics_context_ptr graphics_context,
                             ttf_font_t font, const char* text,
                             SDL_Color color);

/**
 * Get the dimensions of rendered text without actually rendering it
 *
 * @param font Font to use for measurement
 * @param text Text string to measure
 * @param width Output parameter for text width
 * @param height Output parameter for text height
 * @return true on success, false on failure
 */
bool get_ttf_text_size(ttf_font_t font, const char* text, int* width,
                       int* height);

/**
 * Free a loaded TTF font
 *
 * @param font Font to free
 */
void free_ttf_font(ttf_font_t font);

/**
 * Shut down the SDL_ttf subsystem
 * Should be called when done using TTF functions
 */
void quit_ttf_system(void);

#endif  // CORE_GRAPHICS_TTF_TEXT_H_
