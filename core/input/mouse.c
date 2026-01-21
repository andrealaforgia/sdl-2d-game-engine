/**
 * @file mouse.c
 * @brief Mouse input handling implementation
 *
 * Implements mouse position tracking, movement delta calculation,
 * and button state detection using SDL2 mouse functions.
 */

#include "mouse.h"

#include <SDL.h>

#include "inline.h"

/**
 * @brief Initialize mouse state with current position
 *
 * Queries SDL for current mouse position and button state.
 * Initializes delta values to zero and sets previous position
 * to current position.
 *
 * @return Initialized mouse state structure
 */
mouse_state_t init_mouse_state(void) {
    mouse_state_t mouse_state;

    // Get current mouse position and button state
    mouse_state.buttons = SDL_GetMouseState(&mouse_state.x, &mouse_state.y);

    // Initialize previous position to current (no initial delta)
    mouse_state.prev_x = mouse_state.x;
    mouse_state.prev_y = mouse_state.y;

    // Initialize deltas to zero
    mouse_state.delta_x = 0;
    mouse_state.delta_y = 0;

    return mouse_state;
}

/**
 * @brief Update mouse state with current position and calculate deltas
 *
 * Stores previous position, queries current position from SDL,
 * and calculates movement deltas.
 *
 * @param mouse_state Pointer to mouse state to update
 */
void update_mouse_state(mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return;
    }

    // Store previous position
    mouse_state->prev_x = mouse_state->x;
    mouse_state->prev_y = mouse_state->y;

    // Get current mouse position and button state
    mouse_state->buttons = SDL_GetMouseState(&mouse_state->x, &mouse_state->y);

    // Calculate movement deltas
    mouse_state->delta_x = mouse_state->x - mouse_state->prev_x;
    mouse_state->delta_y = mouse_state->y - mouse_state->prev_y;
}

/**
 * @brief Get current mouse X position
 */
ALWAYS_INLINE int get_mouse_x(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return 0;
    }
    return mouse_state->x;
}

/**
 * @brief Get current mouse Y position
 */
ALWAYS_INLINE int get_mouse_y(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return 0;
    }
    return mouse_state->y;
}

/**
 * @brief Get mouse movement delta X since last update
 */
ALWAYS_INLINE int get_mouse_delta_x(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return 0;
    }
    return mouse_state->delta_x;
}

/**
 * @brief Get mouse movement delta Y since last update
 */
ALWAYS_INLINE int get_mouse_delta_y(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return 0;
    }
    return mouse_state->delta_y;
}

/**
 * @brief Check if left mouse button is currently pressed
 */
ALWAYS_INLINE bool is_mouse_left_button_pressed(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return false;
    }
    return (mouse_state->buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
}

/**
 * @brief Check if right mouse button is currently pressed
 */
ALWAYS_INLINE bool is_mouse_right_button_pressed(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return false;
    }
    return (mouse_state->buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

/**
 * @brief Check if middle mouse button is currently pressed
 */
ALWAYS_INLINE bool is_mouse_middle_button_pressed(const mouse_state_ptr mouse_state) {
    if (mouse_state == NULL) {
        return false;
    }
    return (mouse_state->buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
}
