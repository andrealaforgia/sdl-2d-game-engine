/**
 * @file frame.h
 * @brief Frame buffer management and presentation
 *
 * Provides functions for clearing the frame buffer and presenting
 * rendered frames to the display. Handles double-buffering operations
 * and screen refresh.
 */

#ifndef CORE_GRAPHICS_FRAME_H_
#define CORE_GRAPHICS_FRAME_H_

#include "graphics.h"

void clear_frame(const graphics_context_ptr graphics_context);
void render_frame(const graphics_context_ptr graphics_context);

#endif  // CORE_GRAPHICS_FRAME_H_
