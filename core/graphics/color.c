/**
 * @file color.c
 * @brief Implementation of color utility functions
 */

#include "color.h"
#include "inline.h"

#include <stdlib.h>

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
  size_t color_count = sizeof(colors) / sizeof(color_t);
  return colors[rand() % color_count];
}
