#include "graphics.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "display_manager.h"
#include "drawing_primitives.h"
#include "graphics_context.h"
#include "inline.h"
#include "logger.h"
#include "render_utils.h"

graphics_context_t init_graphics_context(int display, int display_mode,
                                         window_mode_t window_mode,
                                         bool vsync) {
  graphics_context_t context =
      initialize_graphics_context(display, display_mode, window_mode, vsync);

  if (!context.window || !context.renderer) {
    LOG_ERROR("Failed to initialize graphics context");
    abort();
  }
  return context;
}
