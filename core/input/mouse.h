/**
 * @file mouse.h
 * @brief Mouse input handling for SDL2 applications
 *
 * Provides functions for tracking mouse position, movement delta,
 * and button states. Designed to work alongside keyboard input
 * for flexible player control options.
 */

#ifndef CORE_INPUT_MOUSE_H_
#define CORE_INPUT_MOUSE_H_

#include <SDL.h>
#include <stdbool.h>

/**
 * @brief Mouse state structure
 *
 * Tracks current mouse position, movement delta since last update,
 * and button states. Position values are in screen coordinates.
 */
typedef struct {
    int x;              // Current mouse X position (screen coordinates)
    int y;              // Current mouse Y position (screen coordinates)
    int delta_x;        // Movement delta X since last update
    int delta_y;        // Movement delta Y since last update
    int prev_x;         // Previous frame X position
    int prev_y;         // Previous frame Y position
    Uint32 buttons;     // Current button state bitmask
} mouse_state_t, *mouse_state_ptr;

/**
 * @brief Initialize mouse state with current position
 *
 * Queries SDL for current mouse position and initializes
 * all tracking values.
 *
 * @return Initialized mouse state structure
 */
mouse_state_t init_mouse_state(void);

/**
 * @brief Update mouse state with current position and calculate deltas
 *
 * Should be called once per frame to update position tracking
 * and calculate movement deltas.
 *
 * @param mouse_state Pointer to mouse state to update
 */
void update_mouse_state(mouse_state_ptr mouse_state);

/**
 * @brief Get current mouse X position
 *
 * @param mouse_state Pointer to mouse state
 * @return Current X position in screen coordinates
 */
int get_mouse_x(const mouse_state_ptr mouse_state);

/**
 * @brief Get current mouse Y position
 *
 * @param mouse_state Pointer to mouse state
 * @return Current Y position in screen coordinates
 */
int get_mouse_y(const mouse_state_ptr mouse_state);

/**
 * @brief Get mouse movement delta X since last update
 *
 * @param mouse_state Pointer to mouse state
 * @return Movement delta in X direction (positive = right)
 */
int get_mouse_delta_x(const mouse_state_ptr mouse_state);

/**
 * @brief Get mouse movement delta Y since last update
 *
 * @param mouse_state Pointer to mouse state
 * @return Movement delta in Y direction (positive = down)
 */
int get_mouse_delta_y(const mouse_state_ptr mouse_state);

/**
 * @brief Check if left mouse button is currently pressed
 *
 * @param mouse_state Pointer to mouse state
 * @return true if left button is pressed
 */
bool is_mouse_left_button_pressed(const mouse_state_ptr mouse_state);

/**
 * @brief Check if right mouse button is currently pressed
 *
 * @param mouse_state Pointer to mouse state
 * @return true if right button is pressed
 */
bool is_mouse_right_button_pressed(const mouse_state_ptr mouse_state);

/**
 * @brief Check if middle mouse button is currently pressed
 *
 * @param mouse_state Pointer to mouse state
 * @return true if middle button is pressed
 */
bool is_mouse_middle_button_pressed(const mouse_state_ptr mouse_state);

#endif  // CORE_INPUT_MOUSE_H_
