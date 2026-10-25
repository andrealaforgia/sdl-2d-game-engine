/**
 * @file mixer.h
 * @brief A small software mixer: a fixed set of voices that each play a
 * mono 16-bit sample once, summed with saturation. Free of SDL so it is
 * tested without a sound device.
 */

#ifndef CORE_AUDIO_MIXER_H_
#define CORE_AUDIO_MIXER_H_

#include <stdint.h>

#define MIXER_RATE 22050
#define MIXER_VOICES 16

typedef struct {
  const int16_t* data;
  int length;  // in frames
} sample_t;

typedef struct {
  const sample_t* sample;
  int position;
  float volume;
} voice_t;

typedef struct {
  voice_t voices[MIXER_VOICES];
} mixer_t;

void mixer_init(mixer_t* mixer);

/** Starts a sample; with every voice busy, steals the furthest along. */
void mixer_play(mixer_t* mixer, const sample_t* sample, float volume);

/** Adds the voices into `out` (which the caller has cleared or filled). */
void mixer_render(mixer_t* mixer, int16_t* out, int frames);

int mixer_active_voices(const mixer_t* mixer);

#endif  // CORE_AUDIO_MIXER_H_
