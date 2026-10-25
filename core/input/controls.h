/**
 * @file controls.h
 * @brief What the player is asking for this frame
 *
 * Buttons are bits in a mask that each game defines for itself (BTN_JUMP =
 * 1u << 1, and so on). A controls_t remembers which are held and which went
 * down since the last update, so jumping and shooting can tell "pressed" from
 * "held" without timers. Free of SDL, so games and their tests share it;
 * keyboard.h fills it from the keys.
 */

#ifndef CORE_INPUT_CONTROLS_H_
#define CORE_INPUT_CONTROLS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t held;     /**< buttons down now */
  uint32_t pressed;  /**< buttons that were not down at the last update */
} controls_t;

/** Stores `now` as the held buttons; `pressed` is what was not held before. */
void controls_update(controls_t* controls, uint32_t now);

/** Forgets the presses, keeping what is held (for the later simulation steps
 * of a frame, so a press counts once). */
void controls_consume_presses(controls_t* controls);

static inline bool controls_held(const controls_t* controls, uint32_t button) {
  return (controls->held & button) != 0;
}

static inline bool controls_pressed(const controls_t* controls,
                                    uint32_t button) {
  return (controls->pressed & button) != 0;
}

#endif  // CORE_INPUT_CONTROLS_H_
