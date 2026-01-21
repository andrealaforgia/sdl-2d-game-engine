#include "keyboard.h"

#include <SDL.h>
#include <stdlib.h>

#include "clock.h"
#include "inline.h"

#define SPACE_KEY_TICKS 150
#define UP_KEY_TICKS 100
#define LEFT_RIGHT_KEY_TICKS 100  // Increased from 15 to prevent double key detection
#define DOWN_KEY_TICKS 10
#define S_KEY_TICKS 150
#define P_KEY_TICKS 150
#define F11_KEY_TICKS 500     // Longer debounce for toggle actions
#define RETURN_KEY_TICKS 300  // Debounce for stage transitions

keyboard_state_t init_keyboard_state(void) {
  keyboard_state_t keyboard_state;
  keyboard_state.keys = SDL_GetKeyboardState(NULL);
  keyboard_state.space_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.up_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.left_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.right_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.down_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.s_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.p_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.f11_key_last_ticks = get_clock_ticks_ms();
  keyboard_state.return_key_last_ticks = get_clock_ticks_ms();
  return keyboard_state;
}

void update_keyboard_state(keyboard_state_ptr keyboard_state) {
  keyboard_state->keys = SDL_GetKeyboardState(NULL);
}

ALWAYS_INLINE bool is_space_key_pressed(
    const keyboard_state_ptr keyboard_state) {
  if (keyboard_state->keys[SDL_SCANCODE_SPACE] &&
      elapsed_from(keyboard_state->space_key_last_ticks) > SPACE_KEY_TICKS) {
    keyboard_state->space_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_up_key_pressed(const keyboard_state_ptr keyboard_state) {
  if ((keyboard_state->keys[SDL_SCANCODE_UP] ||
       keyboard_state->keys[SDL_SCANCODE_K]) &&
      elapsed_from(keyboard_state->up_key_last_ticks) > UP_KEY_TICKS) {
    keyboard_state->up_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_left_key_pressed(
    const keyboard_state_ptr keyboard_state) {
  if ((keyboard_state->keys[SDL_SCANCODE_LEFT] ||
       keyboard_state->keys[SDL_SCANCODE_H]) &&
      elapsed_from(keyboard_state->left_key_last_ticks) >
          LEFT_RIGHT_KEY_TICKS) {
    keyboard_state->left_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_right_key_pressed(
    const keyboard_state_ptr keyboard_state) {
  if ((keyboard_state->keys[SDL_SCANCODE_RIGHT] ||
       keyboard_state->keys[SDL_SCANCODE_L]) &&
      elapsed_from(keyboard_state->right_key_last_ticks) >
          LEFT_RIGHT_KEY_TICKS) {
    keyboard_state->right_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

/**
 * @brief Check if left key is currently held down (no debouncing)
 *
 * Use this for continuous movement where you need to check every frame.
 * Unlike is_left_key_pressed(), this has no debounce timer.
 *
 * @param keyboard_state Pointer to keyboard state
 * @return true if left arrow or H key is currently held down
 */
ALWAYS_INLINE bool is_left_key_held(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_LEFT] ||
         keyboard_state->keys[SDL_SCANCODE_H];
}

/**
 * @brief Check if right key is currently held down (no debouncing)
 *
 * Use this for continuous movement where you need to check every frame.
 * Unlike is_right_key_pressed(), this has no debounce timer.
 *
 * @param keyboard_state Pointer to keyboard state
 * @return true if right arrow or L key is currently held down
 */
ALWAYS_INLINE bool is_right_key_held(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_RIGHT] ||
         keyboard_state->keys[SDL_SCANCODE_L];
}

/**
 * @brief Check if up key is currently held down (no debouncing)
 *
 * Use this for continuous movement where you need to check every frame.
 * Unlike is_up_key_pressed(), this has no debounce timer.
 *
 * @param keyboard_state Pointer to keyboard state
 * @return true if up arrow or K key is currently held down
 */
ALWAYS_INLINE bool is_up_key_held(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_UP] ||
         keyboard_state->keys[SDL_SCANCODE_K];
}

/**
 * @brief Check if down key is currently held down (no debouncing)
 *
 * Use this for continuous movement where you need to check every frame.
 * Unlike is_down_key_pressed(), this has no debounce timer.
 *
 * @param keyboard_state Pointer to keyboard state
 * @return true if down arrow or J key is currently held down
 */
ALWAYS_INLINE bool is_down_key_held(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_DOWN] ||
         keyboard_state->keys[SDL_SCANCODE_J];
}

ALWAYS_INLINE bool is_down_key_pressed(
    const keyboard_state_ptr keyboard_state) {
  if ((keyboard_state->keys[SDL_SCANCODE_DOWN] ||
       keyboard_state->keys[SDL_SCANCODE_J]) &&
      elapsed_from(keyboard_state->down_key_last_ticks) >
          DOWN_KEY_TICKS) {
    keyboard_state->down_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_esc_key_pressed(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_ESCAPE];
}

ALWAYS_INLINE bool is_y_key_pressed(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_Y];
}

ALWAYS_INLINE bool is_n_key_pressed(const keyboard_state_ptr keyboard_state) {
  return keyboard_state->keys[SDL_SCANCODE_N];
}

ALWAYS_INLINE bool is_return_key_pressed(
    const keyboard_state_ptr keyboard_state) {
  if (keyboard_state->keys[SDL_SCANCODE_RETURN] &&
      elapsed_from(keyboard_state->return_key_last_ticks) > RETURN_KEY_TICKS) {
    keyboard_state->return_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_s_key_pressed(const keyboard_state_ptr keyboard_state) {
  if (keyboard_state->keys[SDL_SCANCODE_S] &&
      elapsed_from(keyboard_state->s_key_last_ticks) > S_KEY_TICKS) {
    keyboard_state->s_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_p_key_pressed(const keyboard_state_ptr keyboard_state) {
  if (keyboard_state->keys[SDL_SCANCODE_P] &&
      elapsed_from(keyboard_state->p_key_last_ticks) > P_KEY_TICKS) {
    keyboard_state->p_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}

ALWAYS_INLINE bool is_f11_key_pressed(const keyboard_state_ptr keyboard_state) {
  if (keyboard_state->keys[SDL_SCANCODE_F11] &&
      elapsed_from(keyboard_state->f11_key_last_ticks) > F11_KEY_TICKS) {
    keyboard_state->f11_key_last_ticks = get_clock_ticks_ms();
    return true;
  }
  return false;
}
