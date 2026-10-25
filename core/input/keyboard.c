#include "keyboard.h"

uint32_t keyboard_buttons(const Uint8* keys, const key_binding_t* bindings,
                          size_t count) {
  uint32_t buttons = 0;
  for (size_t i = 0; i < count; ++i) {
    for (int k = 0; k < MAX_KEYS_PER_BUTTON; ++k) {
      int scancode = bindings[i].scancodes[k];
      if (scancode == 0) break;
      if (keys[scancode]) {
        buttons |= bindings[i].button;
        break;
      }
    }
  }
  return buttons;
}

uint32_t keyboard_read(const key_binding_t* bindings, size_t count) {
  return keyboard_buttons(SDL_GetKeyboardState(NULL), bindings, count);
}
