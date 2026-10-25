#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "command_line.h"
#include "events.h"
#include "geometry.h"
#include "physics.h"
#include "rng.h"
#include "stepper.h"
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
static void rng_is_deterministic_per_seed(void) {
  rng_t a, b, c;
  rng_seed(&a, 42);
  rng_seed(&b, 42);
  rng_seed(&c, 43);
  int differs = 0;
  for (int i = 0; i < 100; ++i) {
    uint32_t x = rng_next(&a);
    assert(x == rng_next(&b));
    if (x != rng_next(&c)) differs = 1;
  }
  assert(differs);
  rng_seed(&a, 0);
  assert(rng_next(&a) != 0);
}
static void rng_stays_in_range(void) {
  rng_t rng;
  rng_seed(&rng, 7);
  for (int i = 0; i < 10000; ++i) {
    float f = rng_float(&rng);
    assert(f >= 0.0f && f < 1.0f);
    int n = rng_range(&rng, -3, 4);
    assert(n >= -3 && n <= 4);
  }
  assert(rng_range(&rng, 5, 5) == 5);
  assert(!rng_chance(&rng, 0.0f));
  assert(rng_chance(&rng, 1.0f));
}
static void random_geometry_follows_the_generator_it_is_given(void) {
  rng_t a, b;
  rng_seed(&a, 9);
  rng_seed(&b, 9);
  point_t p = random_point(&a, 640, 480);
  point_t q = random_point(&b, 640, 480);
  assert(p.x == q.x && p.y == q.y);
  assert(p.x >= 0 && p.x < 640 && p.y >= 0 && p.y < 480);
  assert(random_angle(&a) == random_angle(&b));
  velocity_t v = random_velocity(&a);
  velocity_t w = random_velocity(&b);
  assert(v.speed == w.speed);
  point_t centre = point(100, 100);
  point_t near = random_point_around(&a, &centre, 10, 20);
  point_t same = random_point_around(&b, &centre, 10, 20);
  assert(near.x == same.x && near.y == same.y);
}
static void scalar_helpers_clamp_blend_and_approach(void) {
  assert(clampf(5.0f, 0.0f, 3.0f) == 3.0f);
  assert(clampf(-1.0f, 0.0f, 3.0f) == 0.0f);
  assert(clampf(2.0f, 0.0f, 3.0f) == 2.0f);
  assert(lerpf(10.0f, 20.0f, 0.5f) == 15.0f);
  assert(approachf(0.0f, 10.0f, 3.0f) == 3.0f);
  assert(approachf(9.0f, 10.0f, 3.0f) == 10.0f);
  assert(approachf(10.0f, 0.0f, 4.0f) == 6.0f);
}
static void float_rectangles_overlap_only_when_they_share_area(void) {
  frect_t a = {0, 0, 10, 10};
  frect_t b = {5, 5, 10, 10};
  frect_t c = {10, 0, 5, 5};
  assert(frect_overlaps(a, b));
  assert(!frect_overlaps(a, c));
  assert(frect_right(a) == 10.0f && frect_bottom(a) == 10.0f);
}
static void stepper_splits_a_long_frame(void) {
  double step;
  int n = stepper_plan(1.0 / 30.0, 1.0 / 120.0, 0.1, &step);
  assert(n == 4);
  assert(fabs(step * n - 1.0 / 30.0) < 1e-9);
  assert(step <= 1.0 / 120.0 + 1e-12);
}
static void stepper_caps_a_stalled_frame(void) {
  double step;
  int n = stepper_plan(5.0, 1.0 / 120.0, 0.1, &step);
  assert(fabs(step * n - 0.1) < 1e-9);
}
static void stepper_always_runs_one_step(void) {
  double step;
  assert(stepper_plan(0.0, 1.0 / 120.0, 0.1, &step) == 1);
  assert(step == 0.0);
  assert(stepper_plan(-1.0, 1.0 / 120.0, 0.1, &step) == 1);
  assert(step == 0.0);
}
static void the_clock_reports_seconds_that_do_not_go_backwards(void) {
  double first = get_clock_seconds();
  double second = get_clock_seconds();
  assert(second >= first);
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
  } else if (!strcmp(argv[1], "rng")) {
    rng_is_deterministic_per_seed();
    rng_stays_in_range();
  } else if (!strcmp(argv[1], "random-geometry")) {
    random_geometry_follows_the_generator_it_is_given();
  } else if (!strcmp(argv[1], "scalars")) {
    scalar_helpers_clamp_blend_and_approach();
  } else if (!strcmp(argv[1], "rectangles")) {
    float_rectangles_overlap_only_when_they_share_area();
  } else if (!strcmp(argv[1], "stepper")) {
    stepper_splits_a_long_frame();
    stepper_caps_a_stalled_frame();
    stepper_always_runs_one_step();
  } else if (!strcmp(argv[1], "clock")) {
    the_clock_reports_seconds_that_do_not_go_backwards();
  } else if (!strcmp(argv[1], "events")) {
    events_are_drained_even_when_quit_is_queued();
  } else {
    return 1;
  }
  return 0;
}
