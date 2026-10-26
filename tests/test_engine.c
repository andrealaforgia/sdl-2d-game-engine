#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "command_line.h"
#include "events.h"
#include "geometry.h"
#include "graphics.h"
#include "pixel_font.h"
#include "texture.h"
#include "viewport.h"
#include "physics.h"
#include "rng.h"
#include "stepper.h"
#include "audio.h"
#include "controls.h"
#include "keyboard.h"
#include "object_pool.h"
#include "test_allocator.h"
extern int test_ticks;
enum { BTN_LEFT = 1u << 0, BTN_JUMP = 1u << 1, BTN_FIRE = 1u << 2 };

static void controls_report_edges_once(void) {
  controls_t controls = {0};
  controls_update(&controls, BTN_JUMP);
  assert(controls_held(&controls, BTN_JUMP));
  assert(controls_pressed(&controls, BTN_JUMP));
  controls_update(&controls, BTN_JUMP);
  assert(controls_held(&controls, BTN_JUMP));
  assert(!controls_pressed(&controls, BTN_JUMP));
  controls_update(&controls, 0);
  assert(!controls_held(&controls, BTN_JUMP));
  assert(!controls_pressed(&controls, BTN_JUMP));
  controls_update(&controls, BTN_JUMP | BTN_LEFT);
  assert(controls_pressed(&controls, BTN_JUMP));
  assert(controls_pressed(&controls, BTN_LEFT));
  controls_consume_presses(&controls);
  assert(!controls_pressed(&controls, BTN_JUMP));
  assert(controls_held(&controls, BTN_JUMP));
}
static void keys_map_to_buttons_and_may_share_a_key(void) {
  static const key_binding_t bindings[] = {
      {BTN_LEFT, {SDL_SCANCODE_LEFT, SDL_SCANCODE_A, 0}},
      {BTN_JUMP, {SDL_SCANCODE_SPACE, SDL_SCANCODE_Z, SDL_SCANCODE_UP}},
      {BTN_FIRE, {SDL_SCANCODE_J, 0, 0}},
  };
  Uint8 keys[SDL_NUM_SCANCODES] = {0};
  assert(keyboard_buttons(keys, bindings, 3) == 0);
  keys[SDL_SCANCODE_A] = 1;
  assert(keyboard_buttons(keys, bindings, 3) == BTN_LEFT);
  keys[SDL_SCANCODE_UP] = 1;
  assert(keyboard_buttons(keys, bindings, 3) == (BTN_LEFT | BTN_JUMP));
  keys[SDL_SCANCODE_J] = 1;
  assert(keyboard_buttons(keys, bindings, 3) ==
         (BTN_LEFT | BTN_JUMP | BTN_FIRE));
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
static void pools_can_be_walked_and_released_by_object(void) {
  object_pool_t pool = create_object_pool(sizeof(int), 4);
  int* values[4];
  for (int i = 0; i < 4; ++i) {
    values[i] = pool_acquire(&pool, NULL);
    assert(values[i]);
    *values[i] = i * 10;
  }
  pool_release_object(&pool, values[1]);
  pool_release_object(&pool, values[1]);  // twice is harmless
  assert(pool_get_active_count(&pool) == 3);
  assert(pool_index_of(&pool, values[2]) == 2);
  size_t cursor = 0;
  int seen = 0, sum = 0;
  for (int* value; (value = pool_next_active(&pool, &cursor));) {
    ++seen;
    sum += *value;
  }
  assert(seen == 3 && sum == 0 + 20 + 30);
  assert(pool_next_active(&pool, &cursor) == NULL);
  // Released slots come back to be acquired again, zeroed.
  int* again = pool_acquire(&pool, NULL);
  assert(again == values[1] && *again == 0);
  pool_destroy(&pool);
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
static void games_can_register_their_own_options(int argc, char** argv) {
  bool title = false, flag_seen = false;
  int seed = 1, width = 0;
  float seconds = 4.0f;
  const char* snapshot = NULL;
  const custom_option_t custom[] = {
      {"snapshot", OPTION_STRING, &snapshot, "save a frame"},
      {"seed", OPTION_INT, &seed, "random seed"},
      {"width", OPTION_INT, &width, "window width"},
      {"seconds", OPTION_FLOAT, &seconds, "how long to play"},
      {"title", OPTION_FLAG, &title, "show the title"},
      {"other", OPTION_FLAG, &flag_seen, "another flag"},
  };
  command_line_options_t options = default_command_line_options();
  options.window_mode = WINDOWED;
  parse_command_line_options(argc, argv, &options, custom, 6);
  assert(options.window_mode == WINDOWED);
  assert(options.fps == 30);
  assert(seed == 7 && width == 800 && title && !flag_seen);
  assert(seconds > 2.4f && seconds < 2.6f);
  assert(snapshot && !strcmp(snapshot, "out.bmp"));
}
static void the_mixer_plays_a_sample_once_then_stops(void) {
  int16_t data[100];
  for (int i = 0; i < 100; ++i) data[i] = 1000;
  sample_t sample = {data, 100};
  mixer_t mixer;
  mixer_init(&mixer);
  mixer_play(&mixer, &sample, 1.0f);
  assert(mixer_active_voices(&mixer) == 1);
  int16_t out[60] = {0};
  mixer_render(&mixer, out, 60);
  assert(out[0] == 1000 && out[59] == 1000);
  mixer_render(&mixer, out, 60);
  assert(mixer_active_voices(&mixer) == 0);
}
static void the_mixer_saturates_instead_of_wrapping_and_steals_voices(void) {
  int16_t loud[4] = {30000, 30000, 30000, 30000};
  sample_t sample = {loud, 4};
  mixer_t mixer;
  mixer_init(&mixer);
  mixer_play(&mixer, &sample, 1.0f);
  mixer_play(&mixer, &sample, 1.0f);
  int16_t out[4] = {0};
  mixer_render(&mixer, out, 4);
  assert(out[0] == 32767);
  mixer_init(&mixer);
  for (int i = 0; i < MIXER_VOICES + 3; ++i) mixer_play(&mixer, &sample, 1.0f);
  assert(mixer_active_voices(&mixer) == MIXER_VOICES);
}
static void a_game_without_audio_can_still_call_every_audio_function(void) {
  int16_t data[4] = {1, 2, 3, 4};
  assert(!set_sound(NULL, 0, data, 4));
  assert(!load_sound(NULL, 0, "", "missing.wav"));
  play_sound(NULL, 0);
  play_sound_at_volume(NULL, 0, 1.0f);
  set_audio_volume(NULL, 64);
  set_sound_enabled(NULL, false);
  set_music_source(NULL, NULL, NULL);
  set_music_gain(NULL, 0.5f);
  set_music_enabled(NULL, false);
  terminate_audio_context(NULL);
}
static void a_sound_context_copies_mixes_and_mutes_samples(void) {
  setenv("SDL_AUDIODRIVER", "dummy", 1);
  audio_context_ptr audio = init_audio_context(2, 128);
  assert(audio);
  int16_t data[8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
  assert(set_sound(audio, 0, data, 8));
  data[0] = 0;  // the context holds its own copy
  assert(!set_sound(audio, 5, data, 8));
  int16_t out[8] = {0};
  play_sound(audio, 0);
  audio_render(audio, out, 8);
  assert(out[0] == 1000);
  set_sound_enabled(audio, false);
  play_sound(audio, 0);
  int16_t silent[8] = {0};
  audio_render(audio, silent, 8);
  assert(silent[0] == 0);
  set_sound_enabled(audio, true);
  play_sound_at_volume(audio, 0, 0.5f);
  int16_t half[8] = {0};
  audio_render(audio, half, 8);
  assert(half[0] == 500);
  set_audio_volume(audio, 64);
  play_sound(audio, 0);
  int16_t quiet[8] = {0};
  audio_render(audio, quiet, 8);
  assert(quiet[0] == 500);
  terminate_audio_context(audio);
}
static void music_fill(void* user, int16_t* out, int frames, float gain) {
  int* calls = user;
  ++*calls;
  for (int i = 0; i < frames; ++i) out[i] = (int16_t)(1000 * gain);
}
static void a_music_source_is_mixed_with_a_gain_and_can_be_switched_off(void) {
  setenv("SDL_AUDIODRIVER", "dummy", 1);
  audio_context_ptr audio = init_audio_context(1, 128);
  assert(audio);
  int calls = 0;
  set_music_source(audio, music_fill, &calls);
  set_music_gain(audio, 0.5f);
  int16_t out[4] = {0};
  audio_render(audio, out, 4);
  assert(calls == 1 && out[0] == 500);
  set_music_enabled(audio, false);
  int16_t silent[4] = {0};
  audio_render(audio, silent, 4);
  assert(silent[0] == 0);
  terminate_audio_context(audio);
}
static void write_wav(const char* path, int channels, int rate, int frames) {
  FILE* file = fopen(path, "wb");
  assert(file);
  uint32_t data_bytes = (uint32_t)(frames * channels * 2);
  uint32_t riff = 36 + data_bytes, fmt = 16, byte_rate = rate * channels * 2;
  uint16_t pcm = 1, ch = (uint16_t)channels, align = (uint16_t)(channels * 2),
           bits = 16;
  uint32_t sample_rate = (uint32_t)rate;
  fwrite("RIFF", 1, 4, file);
  fwrite(&riff, 4, 1, file);
  fwrite("WAVEfmt ", 1, 8, file);
  fwrite(&fmt, 4, 1, file);
  fwrite(&pcm, 2, 1, file);
  fwrite(&ch, 2, 1, file);
  fwrite(&sample_rate, 4, 1, file);
  fwrite(&byte_rate, 4, 1, file);
  fwrite(&align, 2, 1, file);
  fwrite(&bits, 2, 1, file);
  fwrite("data", 1, 4, file);
  fwrite(&data_bytes, 4, 1, file);
  for (int i = 0; i < frames * channels; ++i) {
    int16_t value = 2000;
    fwrite(&value, 2, 1, file);
  }
  fclose(file);
}
static void wav_files_are_converted_to_the_mixer_format(void) {
  setenv("SDL_AUDIODRIVER", "dummy", 1);
  audio_context_ptr audio = init_audio_context(2, 128);
  assert(audio);
  const char* directory = getenv("TMPDIR") ? getenv("TMPDIR") : "/tmp";
  char mono[512], stereo[512];
  snprintf(mono, sizeof(mono), "%s/engine-test-mono.wav", directory);
  snprintf(stereo, sizeof(stereo), "%s/engine-test-stereo.wav", directory);
  write_wav(mono, 1, MIXER_RATE, 100);
  write_wav(stereo, 2, MIXER_RATE * 2, 400);
  assert(load_sound(audio, 0, directory, "engine-test-mono.wav"));
  assert(load_sound(audio, 1, directory, "engine-test-stereo.wav"));
  assert(!load_sound(audio, 0, directory, "does-not-exist.wav"));
  assert(sound_length(audio, 0) == 100);
  assert(sound_length(audio, 1) == 200);
  remove(mono);
  remove(stereo);
  terminate_audio_context(audio);
}
static void the_viewport_keeps_the_height_and_lets_the_width_follow(void) {
  float scale, width;
  viewport_fit(1280, 720, 360.0f, &scale, &width);
  assert(scale == 2.0f && width == 640.0f);
  viewport_fit(1000, 720, 360.0f, &scale, &width);
  assert(scale == 2.0f && width == 500.0f);
  viewport_fit(0, 0, 360.0f, &scale, &width);
  assert(scale > 0.0f && width > 0.0f);
  viewport_fit(800, 600, 0.0f, &scale, &width);
  assert(scale == 1.0f && width == 800.0f);
}
static void pixel_text_width_counts_cells_and_drops_the_trailing_gap(void) {
  assert(pixel_text_width("", 1.0f) == -1.0f + 0.0f);
  assert(pixel_text_width("AB", 1.0f) == 11.0f);
  assert(pixel_text_width("AB", 3.0f) == 33.0f);
}
static void a_headless_window_scales_draws_and_saves_frames(void) {
  setenv("SDL_VIDEODRIVER", "dummy", 1);
  graphics_config_t config = default_graphics_config("Test");
  config.width = 640;
  config.height = 480;
  config.window_mode = WINDOWED;
  config.view_height = 240.0f;
  graphics_context_t graphics = initialize_graphics_context(&config);
  assert(graphics.window && graphics.renderer);
  begin_frame(&graphics, 10, 20, 30);
  assert(graphics.view_scale > 0.0f);
  assert(graphics.view_width * graphics.view_scale > 639.0f);
  draw_filled_rect_alpha(&graphics, 1.5f, 2.5f, 10.0f, 10.0f, COLOR_RED, 128);
  uint32_t pixels[4] = {0xff0000ffu, 0xff00ff00u, 0xffff0000u, 0xffffffffu};
  texture_t tex = texture_from_pixels(&graphics, pixels, 2, 2, false, false);
  assert(tex.texture && tex.width == 2 && tex.height == 2);
  draw_options_t options = {true, 0.5f, 90.0f, true, 255, 0, 0};
  frect_t where = {20.0f, 20.0f, 8.0f, 8.0f};
  render_sprite(&graphics, &tex, NULL, where, &options);
  render_sprite(&graphics, &tex, NULL, where, NULL);
  pixel_font_t font = build_pixel_font(&graphics);
  assert(font.atlas.texture);
  render_pixel_text(&font, &graphics, 4.0f, 4.0f, "Hi 12", 2.0f, 255, 255, 0);
  render_pixel_text_alpha(&font, &graphics, 4.0f, 24.0f, "+200", 2.0f, 255, 255, 0, 0.5f);
  render_pixel_text_alpha(&font, &graphics, 4.0f, 44.0f, "", 2.0f, 255, 255, 0, 0.0f);
  const char* directory = getenv("TMPDIR") ? getenv("TMPDIR") : "/tmp";
  char path[512];
  snprintf(path, sizeof(path), "%s/engine-test-frame.bmp", directory);
  assert(save_frame_bmp(&graphics, path));
  FILE* file = fopen(path, "rb");
  assert(file);
  fclose(file);
  remove(path);
  free_pixel_font(&font);
  free_texture(&tex);
  terminate_graphics_context(&graphics);
}
int main(int argc, char** argv) {
  assert(argc >= 2);
  if (!strcmp(argv[1], "parse")) {
    command_line_options_t options = default_command_line_options();
    parse_command_line_options(argc - 1, argv + 1, &options, NULL, 0);
    printf("fps=%d volume=%d mode=%d\n", options.fps, options.volume,
           options.window_mode);
  } else if (!strcmp(argv[1], "viewport")) {
    the_viewport_keeps_the_height_and_lets_the_width_follow();
  } else if (!strcmp(argv[1], "pixel-text")) {
    pixel_text_width_counts_cells_and_drops_the_trailing_gap();
  } else if (!strcmp(argv[1], "headless-graphics")) {
    a_headless_window_scales_draws_and_saves_frames();
  } else if (!strcmp(argv[1], "mixer")) {
    the_mixer_plays_a_sample_once_then_stops();
    the_mixer_saturates_instead_of_wrapping_and_steals_voices();
  } else if (!strcmp(argv[1], "sounds")) {
    a_sound_context_copies_mixes_and_mutes_samples();
  } else if (!strcmp(argv[1], "no-audio")) {
    a_game_without_audio_can_still_call_every_audio_function();
  } else if (!strcmp(argv[1], "music")) {
    a_music_source_is_mixed_with_a_gain_and_can_be_switched_off();
  } else if (!strcmp(argv[1], "wav")) {
    wav_files_are_converted_to_the_mixer_format();
  } else if (!strcmp(argv[1], "custom")) {
    games_can_register_their_own_options(argc - 1, argv + 1);
  } else if (!strcmp(argv[1], "controls")) {
    controls_report_edges_once();
  } else if (!strcmp(argv[1], "keys")) {
    keys_map_to_buttons_and_may_share_a_key();
  } else if (!strcmp(argv[1], "pool")) {
    pools_release_storage_and_reuse_slots();
  } else if (!strcmp(argv[1], "pool-walk")) {
    pools_can_be_walked_and_released_by_object();
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
