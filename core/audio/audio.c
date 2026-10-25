#include "audio.h"

#include <stdlib.h>
#include <string.h>

#include "logger.h"

static void lock(audio_context_ptr audio) {
  if (audio->device) SDL_LockAudioDevice(audio->device);
}

static void unlock(audio_context_ptr audio) {
  if (audio->device) SDL_UnlockAudioDevice(audio->device);
}

static int16_t clip(int value) {
  return (int16_t)(value > 32767 ? 32767 : (value < -32768 ? -32768 : value));
}

void audio_render(audio_context_ptr audio, int16_t* out, int frames) {
  memset(out, 0, (size_t)frames * sizeof(*out));
  if (audio->music && audio->music_enabled)
    audio->music(audio->music_user, out, frames, audio->music_gain);
  if (audio->sound_enabled) {
    // The mixer adds to what is in `out`; the master volume applies to the
    // sounds alone, so mix them apart and scale on the way in.
    int16_t sounds[4096];
    for (int done = 0; done < frames;) {
      int count = frames - done < 4096 ? frames - done : 4096;
      memset(sounds, 0, (size_t)count * sizeof(*sounds));
      mixer_render(&audio->mixer, sounds, count);
      for (int i = 0; i < count; ++i)
        out[done + i] = clip(out[done + i] +
                             (int)((float)sounds[i] * audio->volume));
      done += count;
    }
  }
}

static void device_callback(void* user, Uint8* stream, int length) {
  audio_render((audio_context_ptr)user, (int16_t*)stream,
               length / (int)sizeof(int16_t));
}

static void open_device(audio_context_ptr audio) {
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
    LOG_WARN("No audio: the game will run silently");
    return;
  }
  SDL_AudioSpec want, have;
  SDL_zero(want);
  want.freq = MIXER_RATE;
  want.format = AUDIO_S16SYS;
  want.channels = 1;
  want.samples = 1024;
  want.callback = device_callback;
  want.userdata = audio;
  audio->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (!audio->device) {
    LOG_WARN("No audio device: the game will run silently");
    return;
  }
  SDL_PauseAudioDevice(audio->device, 0);
}

audio_context_ptr init_audio_context(int max_sounds, int volume) {
  audio_context_ptr audio = calloc(1, sizeof(*audio));
  if (!audio) return NULL;
  audio->sounds = calloc((size_t)max_sounds, sizeof(*audio->sounds));
  audio->sound_data = calloc((size_t)max_sounds, sizeof(*audio->sound_data));
  if (!audio->sounds || !audio->sound_data) {
    free(audio->sounds);
    free(audio->sound_data);
    free(audio);
    return NULL;
  }
  audio->max_sounds = max_sounds;
  audio->music_gain = 1.0f;
  audio->music_enabled = true;
  audio->sound_enabled = true;
  set_audio_volume(audio, volume);
  mixer_init(&audio->mixer);
  open_device(audio);
  return audio;
}

static bool store(audio_context_ptr audio, int index, int16_t* data,
                  int frames) {
  lock(audio);
  // A playing voice may point at the old data, so stop the mixer using it.
  mixer_init(&audio->mixer);
  free(audio->sound_data[index]);
  audio->sound_data[index] = data;
  audio->sounds[index] = (sample_t){data, frames};
  unlock(audio);
  return true;
}

bool set_sound(audio_context_ptr audio, int index, const int16_t* data,
               int frames) {
  if (!audio || index < 0 || index >= audio->max_sounds || !data ||
      frames <= 0)
    return false;
  int16_t* copy = malloc((size_t)frames * sizeof(*copy));
  if (!copy) return false;
  memcpy(copy, data, (size_t)frames * sizeof(*copy));
  return store(audio, index, copy, frames);
}

bool load_sound(audio_context_ptr audio, int index, const char* base_path,
                const char* sound_file) {
  if (!audio || index < 0 || index >= audio->max_sounds) {
    LOG_WARN("Sound index out of bounds");
    return false;
  }
  size_t length = strlen(base_path) + strlen(sound_file) + 2;
  char* path = malloc(length);
  if (!path) return false;
  snprintf(path, length, "%s/%s", base_path, sound_file);

  SDL_AudioSpec spec;
  Uint8* wav = NULL;
  Uint32 wav_bytes = 0;
  if (!SDL_LoadWAV(path, &spec, &wav, &wav_bytes)) {
    LOG_ERROR_FMT("Failed to load sound: %s", path);
    LOG_WARN("Game will continue without this sound effect");
    free(path);
    return false;
  }
  free(path);

  SDL_AudioCVT cvt;
  if (SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
                        AUDIO_S16SYS, 1, MIXER_RATE) < 0) {
    SDL_FreeWAV(wav);
    return false;
  }
  cvt.len = (int)wav_bytes;
  Uint8* buffer = malloc((size_t)cvt.len * (size_t)cvt.len_mult);
  if (!buffer) {
    SDL_FreeWAV(wav);
    return false;
  }
  memcpy(buffer, wav, wav_bytes);
  SDL_FreeWAV(wav);
  cvt.buf = buffer;
  if (SDL_ConvertAudio(&cvt) != 0 || cvt.len_cvt < 2) {
    free(buffer);
    return false;
  }
  int frames = cvt.len_cvt / (int)sizeof(int16_t);
  return store(audio, index, (int16_t*)buffer, frames);
}

int sound_length(const audio_context_ptr audio, int index) {
  if (!audio || index < 0 || index >= audio->max_sounds) return 0;
  return audio->sounds[index].length;
}

void play_sound_at_volume(audio_context_ptr audio, int index, float volume) {
  if (!audio || !audio->sound_enabled || index < 0 ||
      index >= audio->max_sounds || !audio->sounds[index].data)
    return;
  lock(audio);
  mixer_play(&audio->mixer, &audio->sounds[index], volume);
  unlock(audio);
}

void play_sound(audio_context_ptr audio, int index) {
  play_sound_at_volume(audio, index, 1.0f);
}

void set_audio_volume(audio_context_ptr audio, int volume) {
  if (volume < 0) volume = 0;
  if (volume > 128) volume = 128;
  audio->volume = (float)volume / 128.0f;
}

void set_sound_enabled(audio_context_ptr audio, bool enabled) {
  lock(audio);
  audio->sound_enabled = enabled;
  unlock(audio);
}

void set_music_source(audio_context_ptr audio, music_source_fn source,
                      void* user) {
  lock(audio);
  audio->music = source;
  audio->music_user = user;
  unlock(audio);
}

void set_music_gain(audio_context_ptr audio, float gain) {
  lock(audio);
  audio->music_gain = gain;
  unlock(audio);
}

void set_music_enabled(audio_context_ptr audio, bool enabled) {
  lock(audio);
  audio->music_enabled = enabled;
  unlock(audio);
}

void terminate_audio_context(audio_context_ptr audio) {
  if (!audio) return;
  if (audio->device) SDL_CloseAudioDevice(audio->device);
  for (int i = 0; i < audio->max_sounds; ++i) free(audio->sound_data[i]);
  free(audio->sound_data);
  free(audio->sounds);
  free(audio);
}
