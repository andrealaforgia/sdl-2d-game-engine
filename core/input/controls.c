#include "controls.h"

void controls_update(controls_t* controls, uint32_t now) {
  controls->pressed = now & ~controls->held;
  controls->held = now;
}

void controls_consume_presses(controls_t* controls) { controls->pressed = 0; }
