#include "clock.h"
int test_ticks = 10000;
int get_clock_ticks_ms(void) { return test_ticks; }
int elapsed_from(int ticks) { return test_ticks - ticks; }
