#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "command_line.h"
#include "events.h"
#include "keyboard.h"
#include "object_pool.h"
#include "test_allocator.h"
extern int test_ticks;
static void toggle_keys_fire_once_per_press(void) {
  Uint8 keys[SDL_NUM_SCANCODES] = {0};
  keyboard_state_t keyboard = init_keyboard_state();
  keyboard.keys = keys;
  keys[SDL_SCANCODE_S] = 1;
  test_ticks += 1000;
  assert(is_s_key_pressed(&keyboard));
  test_ticks += 1000;
  assert(!is_s_key_pressed(&keyboard));
  keys[SDL_SCANCODE_S] = 0;
  assert(!is_s_key_pressed(&keyboard));
  keys[SDL_SCANCODE_S] = 1;
  assert(is_s_key_pressed(&keyboard));
  keys[SDL_SCANCODE_F11] = 1;
  assert(is_f11_key_pressed(&keyboard));
  test_ticks += 1000;
  assert(!is_f11_key_pressed(&keyboard));
}
static void pools_release_storage_and_reuse_slots(void) {
  object_pool_t pool = create_object_pool(sizeof(int), 2);
  size_t first, second;
  assert(pool_acquire(&pool, &first));
  assert(pool_acquire(&pool, &second));
  assert(!pool_acquire(&pool, NULL));
  pool_release(&pool, first);
  assert(pool_acquire(&pool, NULL));
  pool_reset(&pool);
  assert(pool_get_active_count(&pool) == 0);
  pool_destroy(&pool);
  pool_destroy(&pool);
  assert(outstanding_allocations() == 0);
}
static void failed_pool_allocations_release_partial_storage(void) {
  for (int allocation = 0; allocation < 3; ++allocation) {
    fail_allocation_after(allocation);
    object_pool_t pool = create_object_pool(sizeof(int), 4);
    assert(!pool.objects);
    assert(pool.capacity == 0);
    pool_destroy(&pool);
    assert(outstanding_allocations() == 0);
  }
  fail_allocation_after(-1);
  object_pool_t pool = create_object_pool(SIZE_MAX, 2);
  assert(!pool.objects);
  assert(outstanding_allocations() == 0);
}
static void events_are_drained_even_when_quit_is_queued(void) {
  assert(SDL_Init(SDL_INIT_EVENTS) == 0);
  SDL_Event event = {0};
  event.type = SDL_KEYDOWN;
  for (int i = 0; i < 20; ++i) assert(SDL_PushEvent(&event) == 1);
  event.type = SDL_QUIT;
  assert(SDL_PushEvent(&event) == 1);
  assert(drain_events() == QUIT_EVENT);
  assert(poll_event() == NO_EVENT);
  SDL_Quit();
}
int main(int argc, char** argv) {
  assert(argc >= 2);
  if (!strcmp(argv[1], "parse")) {
    command_line_options_t options =
        parse_command_line_options(argc - 1, argv + 1);
    printf("fps=%d volume=%d mode=%d\n", options.fps, options.volume,
           options.window_mode);
  } else if (!strcmp(argv[1], "keys")) {
    toggle_keys_fire_once_per_press();
  } else if (!strcmp(argv[1], "pool")) {
    pools_release_storage_and_reuse_slots();
  } else if (!strcmp(argv[1], "allocation")) {
    failed_pool_allocations_release_partial_storage();
  } else if (!strcmp(argv[1], "events")) {
    events_are_drained_even_when_quit_is_queued();
  } else {
    return 1;
  }
  return 0;
}
