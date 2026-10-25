/**
 * @file keyboard.h
 * @brief Maps keys to the buttons of a controls_t
 *
 * A game lists its bindings once: which scancodes drive which button. A key
 * may appear under several buttons (up can be both "up" and "jump").
 */

#ifndef CORE_INPUT_KEYBOARD_H_
#define CORE_INPUT_KEYBOARD_H_

#include <SDL.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_KEYS_PER_BUTTON 4

typedef struct {
  uint32_t button;                       /**< the button bit this drives */
  int scancodes[MAX_KEYS_PER_BUTTON];    /**< SDL scancodes; 0 ends the list */
} key_binding_t;

/** The buttons down according to `keys` (an SDL keyboard state array). */
uint32_t keyboard_buttons(const Uint8* keys, const key_binding_t* bindings,
                          size_t count);

/** The buttons down right now, read from SDL's keyboard state. */
uint32_t keyboard_read(const key_binding_t* bindings, size_t count);

#endif  // CORE_INPUT_KEYBOARD_H_
