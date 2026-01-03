#include "audio.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inline.h"
#include "logger.h"

// Construct full path to a sound file
static char* get_sound_path(const char* base_path, const char* sound_file) {
  // +2 for "/" and null terminator
  size_t path_len = strlen(base_path) + strlen(sound_file) + 2;
  char* full_path = malloc(path_len);
  if (full_path) {
    snprintf(full_path, path_len, "%s/%s", base_path, sound_file);
  }
  return full_path;
}

audio_context_t init_audio_context(int max_sounds, int volume) {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
    LOG_MIX_ERROR("Mix_OpenAudio");
  }

  audio_context_t audio_context;
  audio_context.max_sounds = max_sounds;
  audio_context.chunks = calloc(max_sounds, sizeof(Mix_Chunk*));

  if (!audio_context.chunks) {
    LOG_WARN("Failed to allocate memory for audio chunks");
    audio_context.max_sounds = 0;
    return audio_context;
  }

  // Amount of channels (Max amount of sounds playing at the same time)
  int channels = Mix_AllocateChannels(256);
  if (channels < 256) {
    LOG_WARN("Could not allocate 256 audio channels");
    LOG_INFO_FMT("Allocated %d channels instead", channels);
  }

  // Volume is in the range 0-128, where 128 is full volume
  Mix_Volume(-1, volume);  // -1 affects all channels

  return audio_context;
}

bool load_sound(audio_context_ptr audio_context, int index,
                const char* base_path, const char* sound_file) {
  if (!audio_context || !audio_context->chunks) {
    LOG_WARN("Invalid audio context");
    return false;
  }

  if (index < 0 || index >= audio_context->max_sounds) {
    LOG_WARN("Sound index out of bounds");
    return false;
  }

  char* full_path = get_sound_path(base_path, sound_file);
  if (!full_path) {
    LOG_WARN("Failed to allocate memory for sound path");
    return false;
  }

  LOG_INFO_FMT("Loading sound: %s", full_path);

  Mix_Chunk* chunk = Mix_LoadWAV(full_path);
  if (!chunk) {
    LOG_ERROR_FMT("Failed to load sound: %s", full_path);
    LOG_MIX_ERROR(full_path);
    LOG_WARN("Game will continue without this sound effect");
    free(full_path);
    return false;
  }

  audio_context->chunks[index] = chunk;
  LOG_INFO_FMT("Successfully loaded sound at index %d", index);
  free(full_path);
  return true;
}

void play_sound(const audio_context_ptr audio_context, int index) {
  if (!audio_context || !audio_context->chunks) {
    LOG_WARN("Invalid audio context in play_sound");
    return;
  }

  if (index < 0 || index >= audio_context->max_sounds) {
    LOG_WARN_FMT("Sound index %d out of bounds", index);
    return;
  }

  if (audio_context->chunks[index]) {
    int channel = Mix_PlayChannel(-1, audio_context->chunks[index], 0);
    if (channel == -1) {
      LOG_WARN_FMT("Failed to play sound %d: %s", index, Mix_GetError());
    }
  } else {
    LOG_WARN_FMT("Sound at index %d not loaded", index);
  }
}

void set_audio_volume(int volume) {
  // Volume is in the range 0-128, where 128 is full volume
  Mix_Volume(-1, volume);  // -1 affects all channels
}

void terminate_audio_context(const audio_context_ptr audio_context) {
  if (!audio_context || !audio_context->chunks) {
    return;
  }

  for (int i = 0; i < audio_context->max_sounds; i++) {
    if (audio_context->chunks[i]) {
      Mix_FreeChunk(audio_context->chunks[i]);
    }
  }

  free(audio_context->chunks);
  Mix_CloseAudio();
}
