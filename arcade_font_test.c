/**
 * @file arcade_font_test.c
 * @brief Test application for arcade bitmap font functionality
 *
 * Tests loading and rendering of the arcade font with all supported characters
 * and colors to verify the font is working correctly.
 */

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/graphics/arcade_font.h"
#include "core/graphics/graphics_context.h"
#include "core/utils/logger.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Arcade Font Test"

/**
 * Test all supported characters and colors
 */
void test_all_characters(const arcade_font_ptr font,
                         const graphics_context_ptr graphics_context) {
  int y = 50;
  int line_height = 40;

  // Test each color with different character sets
  font_color_t colors[] = {FONT_COLOR_WHITE, FONT_COLOR_RED, FONT_COLOR_PINK,
                           FONT_COLOR_CYAN,  FONT_COLOR_GOLD, FONT_COLOR_PEACH,
                           FONT_COLOR_YELLOW};
  const char* color_names[] = {"WHITE", "RED",   "PINK", "CYAN",
                               "GOLD",  "PEACH", "YELLOW"};

  for (int i = 0; i < 7; i++) {
    char line[100];
    sprintf(line, "%s: ABCDEFGHIJKLMNOPQRSTUVWXYZ", color_names[i]);
    render_arcade_text(font, graphics_context, line, 50, y, colors[i]);
    y += line_height;
  }

  // Test numbers and symbols
  y += 20;
  render_arcade_text(font, graphics_context, "NUMBERS: 0123456789", 50, y,
                     FONT_COLOR_WHITE);
  y += line_height;
  render_arcade_text(font, graphics_context, "SYMBOLS: ! / -", 50, y,
                     FONT_COLOR_WHITE);
  y += line_height;
  render_arcade_text(font, graphics_context, "SPACES:  A  B  C  D  E", 50, y,
                     FONT_COLOR_WHITE);

  // Test scaling
  y += 60;
  render_arcade_text_scaled(font, graphics_context, "SCALED 2X", 50, y,
                            FONT_COLOR_GOLD, 2);
  y += 80;
  render_arcade_text_scaled(font, graphics_context, "SCALED 3X", 50, y,
                            FONT_COLOR_CYAN, 3);
}

int main(int argc, char* argv[]) {
  (void)argc;  // Suppress unused parameter warning
  (void)argv;  // Suppress unused parameter warning

  LOG_INFO("Starting arcade font test application");

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LOG_ERROR_FMT("SDL initialization failed: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Create window
  SDL_Window* window = SDL_CreateWindow(
      WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    LOG_ERROR_FMT("Window creation failed: %s", SDL_GetError());
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Create renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    LOG_ERROR_FMT("Renderer creation failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Initialize graphics context
  graphics_context_t graphics_context = {0};
  graphics_context.window = window;
  graphics_context.renderer = renderer;

  // Load arcade font
  arcade_font_t arcade_font = load_arcade_font(&graphics_context);
  if (!arcade_font.bitmap_font.texture.texture) {
    LOG_ERROR("Failed to load arcade font");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  LOG_INFO("Font loaded successfully. Press ESC to exit.");

  // Main loop
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }
    }

    // Clear screen with black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Test all characters
    test_all_characters(&arcade_font, &graphics_context);

    // Present the frame
    SDL_RenderPresent(renderer);

    // Small delay to prevent high CPU usage
    SDL_Delay(16);  // ~60 FPS
  }

  // Cleanup
  free_arcade_font(&arcade_font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  LOG_INFO("Arcade font test application finished");
  return EXIT_SUCCESS;
}
