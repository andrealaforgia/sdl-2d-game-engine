#include "graphics.h"
#include "graphics_context.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "inline.h"
#include "logger.h"

// Pre-calculated sin/cos lookup table for circle drawing optimization
#define CIRCLE_POINTS 360
static double circle_cos[CIRCLE_POINTS];
static double circle_sin[CIRCLE_POINTS];
static bool circle_lookup_initialized = false;

void init_circle_lookup(void) {
  if (!circle_lookup_initialized) {
    for (int i = 0; i < CIRCLE_POINTS; i++) {
      double angle = i * M_PI / 180.0;
      circle_cos[i] = cos(angle);
      circle_sin[i] = sin(angle);
    }
    circle_lookup_initialized = true;
  }
}

static int get_display_count(void) { return SDL_GetNumVideoDisplays(); }

static SDL_DisplayMode* get_display_modes(int display_index,
                                          int* p_display_mode_count) {
  *p_display_mode_count = SDL_GetNumDisplayModes(display_index);
  if (*p_display_mode_count < 1) {
    LOG_SDL_ERROR("SDL_GetNumDisplayModes");
    return NULL;
  }

  SDL_DisplayMode* display_modes =
      calloc(*p_display_mode_count, sizeof(SDL_DisplayMode));

  for (int i = 0; i < *p_display_mode_count; i++) {
    if (SDL_GetDisplayMode(display_index, i, &display_modes[i]) != 0) {
      LOG_SDL_ERROR("SDL_GetDisplayMode");
      free(display_modes);
      return NULL;
    }
  }
  return display_modes;
}

void print_graphics_info(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    LOG_SDL_ERROR("SDL_Init");
    return;
  }
  int display_count = get_display_count();
  LOG_INFO_FMT("Number of available displays: %d", display_count);
  for (int display_index = 0; display_index < display_count; display_index++) {
    LOG_INFO_FMT("Display Index: %d", display_index);
    int display_mode_count;
    SDL_DisplayMode* display_modes =
        get_display_modes(display_index, &display_mode_count);
    if (display_modes) {
      for (int dm = 0; dm < display_mode_count; dm++) {
        LOG_INFO_FMT("Display Mode %d\tbpp %d\t%s\t%d x %d", dm,
                     SDL_BITSPERPIXEL(display_modes[dm].format),
                     SDL_GetPixelFormatName(display_modes[dm].format),
                     display_modes[dm].w, display_modes[dm].h);
      }
      free(display_modes);
    }
  }
  SDL_Quit();
}


graphics_context_t init_graphics_context(int display, int display_mode,
                                         window_mode_t window_mode,
                                         bool vsync) {
  graphics_context_t context = initialize_graphics_context(display, display_mode, window_mode, vsync);
  
  if (!context.window || !context.renderer) {
    LOG_ERROR("Failed to initialize graphics context");
    abort();
  }
  
  return context;
}


void toggle_fullscreen(const graphics_context_ptr graphics_context) {
  // Get current fullscreen state
  Uint32 flags = SDL_GetWindowFlags(graphics_context->window);
  bool is_fullscreen = (flags & SDL_WINDOW_FULLSCREEN) ||
                       (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);

  if (is_fullscreen) {
    // Switch to windowed mode
    if (SDL_SetWindowFullscreen(graphics_context->window, 0) == 0) {
      LOG_INFO("Switched to windowed mode (press F11 to toggle)");
    } else {
      LOG_SDL_ERROR("SDL_SetWindowFullscreen");
    }
  } else {
    // Switch to borderless fullscreen (faster toggle than true fullscreen)
    if (SDL_SetWindowFullscreen(graphics_context->window,
                                SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
      LOG_INFO("Switched to fullscreen mode (press F11 to toggle)");
    } else {
      LOG_SDL_ERROR("SDL_SetWindowFullscreen");
    }
  }
}

ALWAYS_INLINE void draw_line(const graphics_context_ptr graphics_context,
                             int x1, int y1, int x2, int y2, color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  SDL_RenderDrawLine(graphics_context->renderer, x1, y1, x2, y2);
}

ALWAYS_INLINE void draw_thick_line(const graphics_context_ptr graphics_context,
                                   int x1, int y1, int x2, int y2,
                                   color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  // Draw main line
  SDL_RenderDrawLine(graphics_context->renderer, x1, y1, x2, y2);
  // Draw parallel lines to create thickness
  SDL_RenderDrawLine(graphics_context->renderer, x1 + 1, y1, x2 + 1, y2);
  SDL_RenderDrawLine(graphics_context->renderer, x1, y1 + 1, x2, y2 + 1);
  SDL_RenderDrawLine(graphics_context->renderer, x1 - 1, y1, x2 - 1, y2);
  SDL_RenderDrawLine(graphics_context->renderer, x1, y1 - 1, x2, y2 - 1);
}

ALWAYS_INLINE void draw_line_between_points(
    const graphics_context_ptr graphics_context, const point_ptr p1,
    const point_ptr p2, color_t color) {
  draw_line(graphics_context, p1->x, p1->y, p2->x, p2->y, color);
}

ALWAYS_INLINE void draw_pixel(const graphics_context_ptr graphics_context,
                              int x, int y, color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  SDL_RenderDrawPoint(graphics_context->renderer, x, y);
}

ALWAYS_INLINE void draw_point(const graphics_context_ptr graphics_context,
                              const point_ptr p, color_t color) {
  draw_pixel(graphics_context, p->x, p->y, color);
}

ALWAYS_INLINE void draw_fat_pixel(const graphics_context_ptr graphics_context,
                                  const point_ptr p, color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  // Draw a 5x5 square for thicker bullets
  for (int dy = -2; dy <= 2; dy++) {
    SDL_RenderDrawLine(graphics_context->renderer, p->x - 2, p->y + dy,
                       p->x + 2, p->y + dy);
  }
}

void draw_circle(const graphics_context_ptr graphics_context, int32_t centreX,
                 int32_t centreY, int32_t radius, color_t color) {
  // Pre-allocate points array for batched rendering
  SDL_Point points[CIRCLE_POINTS];

  // Calculate all circle points using pre-calculated lookup table
  for (int i = 0; i < CIRCLE_POINTS; i++) {
    points[i].x = centreX + (int)(radius * circle_cos[i]);
    points[i].y = centreY + (int)(radius * circle_sin[i]);
  }

  // Render all points in a single batched call (720 calls -> 2 calls)
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  SDL_RenderDrawPoints(graphics_context->renderer, points, CIRCLE_POINTS);
}

void draw_filled_polygon(const graphics_context_ptr graphics_context,
                         const SDL_Point* points, int num_points,
                         color_t fill_color) {
  if (num_points < 3) return;  // Need at least 3 points for a polygon

  // Calculate center point for triangle fan
  int center_x = 0, center_y = 0;
  for (int i = 0; i < num_points; i++) {
    center_x += points[i].x;
    center_y += points[i].y;
  }
  center_x /= num_points;
  center_y /= num_points;

  // Draw triangle fan from center to each edge with solid fill color
  for (int i = 0; i < num_points; i++) {
    int next = (i + 1) % num_points;

    // Create triangle vertices with solid fill color
    SDL_Vertex vertices[3] = {
        {{center_x, center_y},
         {R(fill_color), G(fill_color), B(fill_color), 255},
         {0, 0}},
        {{points[i].x, points[i].y},
         {R(fill_color), G(fill_color), B(fill_color), 255},
         {0, 0}},
        {{points[next].x, points[next].y},
         {R(fill_color), G(fill_color), B(fill_color), 255},
         {0, 0}}};

    SDL_RenderGeometry(graphics_context->renderer, NULL, vertices, 3, NULL, 0);
  }
}

ALWAYS_INLINE double wrap(double value, double upper_bound) {
  if (value < 0) {
    return upper_bound - 1;
  }
  if (value > upper_bound - 1) {
    return 0;
  }
  return value;
}

ALWAYS_INLINE double wrap_x(const graphics_context_ptr graphics_context,
                            double x) {
  return wrap(x, graphics_context->screen_width);
}

ALWAYS_INLINE double wrap_y(const graphics_context_ptr graphics_context,
                            double y) {
  return wrap(y, graphics_context->screen_height);
}

ALWAYS_INLINE point_t
random_point(const graphics_context_ptr graphics_context) {
  return point(rand() % graphics_context->screen_width,
               rand() % graphics_context->screen_height);
}

ALWAYS_INLINE color_t random_color(void) {
  // Generate vibrant colors by using predefined bright color palette
  color_t colors[] = {
      0xFF0000,  // Red
      0x00FF00,  // Green
      0x0000FF,  // Blue
      0xFFFF00,  // Yellow
      0xFF00FF,  // Magenta
      0x00FFFF,  // Cyan
      0xFF8000,  // Orange
      0xFF0080,  // Pink
      0x80FF00,  // Lime
      0x0080FF,  // Sky blue
  };
  return colors[rand() % (sizeof(colors) / sizeof(color_t))];
}

ALWAYS_INLINE bool out_of_bounds(const graphics_context_ptr graphics_context,
                                 const point_ptr point) {
  return point->x < 0 || point->x >= graphics_context->screen_width ||
         point->y < 0 || point->y >= graphics_context->screen_height;
}
