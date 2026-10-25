/**
 * @file graphics_context.h
 * @brief Graphics context initialization and management
 *
 * Provides centralized graphics subsystem initialization and context
 * management. Extracted from the god object graphics.c for better separation of
 * concerns.
 */

#ifndef CORE_GRAPHICS_GRAPHICS_CONTEXT_H_
#define CORE_GRAPHICS_GRAPHICS_CONTEXT_H_

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#include "geometry.h"
#include "window_mode.h"

// Graphics context structure definition
typedef struct graphics_context {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int screen_width;  /**< window size when it was created */
  int screen_height;
  point_t screen_center;
  float view_height;  /**< logical height of the view; 0 means pixels */
  float view_scale;   /**< pixels per logical unit, set by begin_frame */
  float view_width;   /**< logical width, set by begin_frame */
} graphics_context_t;

typedef graphics_context_t* graphics_context_ptr;

/**
 * @brief Initialize SDL subsystems and graphics hints
 * @return true if initialization successful, false otherwise
 */
bool initialize_graphics_subsystems(void);

/**
 * @brief Validate and normalize display mode parameters
 * @param display Pointer to display index (may be modified)
 * @param display_mode Pointer to display mode index (may be modified)
 * @param sdl_display_mode Output SDL display mode structure
 * @return true if validation successful, false otherwise
 */
bool validate_display_configuration(int* display, int* display_mode,
                                    SDL_DisplayMode* sdl_display_mode);

/** What a game asks of its window. */
typedef struct {
  const char* title;
  int width;   /**< 0 for the whole usable screen (windowed) or the display */
  int height;  /**< mode's size (other modes) */
  int display;
  int display_mode;
  window_mode_t window_mode;
  bool vsync;
  float view_height;  /**< logical height the game draws in; 0 for pixels */
} graphics_config_t;

/** A resizable window filling the screen, vsynced, drawn in pixels. */
graphics_config_t default_graphics_config(const char* title);

/**
 * @brief Create game window with specified configuration
 * @param config Title, size and mode
 * @param display_mode SDL display mode for fullscreen
 * @return Created window or NULL on failure
 */
SDL_Window* create_application_window(const graphics_config_t* config,
                                      SDL_DisplayMode* display_mode);

/**
 * @brief Create game renderer with hardware fallback
 * @param window Window to create renderer for
 * @param vsync Enable vertical synchronization
 * @return Created renderer or NULL on failure
 */
SDL_Renderer* create_application_renderer(SDL_Window* window, bool vsync);

/**
 * @brief Initialize complete graphics context
 * @return Initialized graphics context; window and renderer are NULL on
 *         failure
 */
graphics_context_t initialize_graphics_context(
    const graphics_config_t* config);

/**
 * @brief Start a frame: fit the view to the window, then clear it
 *
 * Sets view_scale and view_width from the window's current output size, so
 * everything drawn afterwards is in logical units.
 */
void begin_frame(graphics_context_t* context, uint8_t r, uint8_t g, uint8_t b);

/** Saves what the renderer has drawn this frame as a BMP file. */
bool save_frame_bmp(const graphics_context_t* context, const char* path);

/**
 * @brief Cleanup and shutdown graphics context
 * @param context Graphics context to cleanup
 */
void terminate_graphics_context(graphics_context_t* context);

/**
 * @brief Shutdown graphics subsystems without a graphics context
 *
 * Use this when you've called initialize_graphics_subsystems() but
 * haven't created a full graphics context (window/renderer).
 */
void shutdown_graphics_subsystems(void);

/**
 * @brief Get the size of a window
 * @param window Window to query
 * @param width Output width
 * @param height Output height
 */
void get_window_size(SDL_Window* window, int* width, int* height);

#endif  // CORE_GRAPHICS_GRAPHICS_CONTEXT_H_
