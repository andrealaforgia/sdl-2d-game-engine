/**
 * @file graphics_context.h
 * @brief Graphics context initialization and management
 *
 * Provides centralized graphics subsystem initialization and context management.
 * Extracted from the god object graphics.c for better separation of concerns.
 */

#ifndef ENGINE_CORE_GRAPHICS_GRAPHICS_CONTEXT_H_
#define ENGINE_CORE_GRAPHICS_GRAPHICS_CONTEXT_H_

#include <SDL.h>
#include <stdbool.h>
#include "window_mode.h"
#include "geometry.h"

// Graphics context structure definition
typedef struct graphics_context {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int screen_width;
  int screen_height;
  point_t screen_center;
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
bool validate_display_configuration(int* display, int* display_mode, SDL_DisplayMode* sdl_display_mode);

/**
 * @brief Create game window with specified configuration
 * @param title Window title
 * @param window_mode Window display mode
 * @param display Display index
 * @param width Window width
 * @param height Window height
 * @param display_mode SDL display mode for fullscreen
 * @return Created window or NULL on failure
 */
SDL_Window* create_application_window(const char* title, window_mode_t window_mode, 
                                     int display, int width, int height, 
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
 * @param display Display index
 * @param display_mode Display mode index  
 * @param window_mode Window mode configuration
 * @param vsync Enable vertical synchronization
 * @return Initialized graphics context
 */
graphics_context_t initialize_graphics_context(int display, int display_mode,
                                              window_mode_t window_mode, bool vsync);

/**
 * @brief Cleanup and shutdown graphics context
 * @param context Graphics context to cleanup
 */
void terminate_graphics_context(graphics_context_t* context);

#endif  // ENGINE_CORE_GRAPHICS_GRAPHICS_CONTEXT_H_
