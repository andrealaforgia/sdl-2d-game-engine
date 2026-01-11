/**
 * @file drawing_primitives.c
 * @brief Implementation of drawing primitive functions
 */

#include "drawing_primitives.h"

#include <SDL.h>
#include <math.h>
#include <stdbool.h>

#include "graphics.h"
#include "inline.h"

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

void draw_filled_rect(const graphics_context_ptr graphics_context,
                      int x, int y, int width, int height, color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  SDL_Rect rect = {x, y, width, height};
  SDL_RenderFillRect(graphics_context->renderer, &rect);
}

void draw_filled_rect_alpha(const graphics_context_ptr graphics_context,
                            int x, int y, int width, int height,
                            color_t color, uint8_t alpha) {
  SDL_SetRenderDrawBlendMode(graphics_context->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), alpha);
  SDL_Rect rect = {x, y, width, height};
  SDL_RenderFillRect(graphics_context->renderer, &rect);
  SDL_SetRenderDrawBlendMode(graphics_context->renderer, SDL_BLENDMODE_NONE);
}

void set_render_draw_color_alpha(const graphics_context_ptr graphics_context,
                                 color_t color, uint8_t alpha) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), alpha);
}

void clear_screen(const graphics_context_ptr graphics_context, color_t color) {
  SDL_SetRenderDrawColor(graphics_context->renderer, R(color), G(color),
                         B(color), 255);
  SDL_RenderClear(graphics_context->renderer);
}

void present_frame(const graphics_context_ptr graphics_context) {
  SDL_RenderPresent(graphics_context->renderer);
}
