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

graphics_context_t init_graphics_context(const graphics_config_t* config) {
  graphics_context_t context = initialize_graphics_context(config);

  if (!context.window || !context.renderer) {
    LOG_ERROR("Failed to initialize graphics context");
    abort();
  }
  return context;
}
