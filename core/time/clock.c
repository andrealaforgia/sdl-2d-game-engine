#include "clock.h"

#include <SDL.h>

#include "inline.h"

ALWAYS_INLINE int get_clock_ticks_ms(void) { return SDL_GetTicks(); }
ALWAYS_INLINE int elapsed_from(int ticks) {
  return get_clock_ticks_ms() - ticks;
}

double get_clock_seconds(void) {
  return (double)SDL_GetPerformanceCounter() /
         (double)SDL_GetPerformanceFrequency();
}
