/**
 * @file command_line.h
 * @brief Command-line argument parsing for game configuration
 *
 * Parses command-line arguments to configure display settings, window mode,
 * FPS limits, audio volume, and other runtime options. Provides help text
 * and validation for all supported command-line flags.
 */

#ifndef CORE_UTILS_COMMAND_LINE_H_
#define CORE_UTILS_COMMAND_LINE_H_

#include <stdbool.h>
#include <stddef.h>

#include "window_mode.h"

typedef struct {
  bool help;
  bool graphics_info;
  bool show_fps;
  bool vsync;
  int display;
  int display_mode;
  window_mode_t window_mode;
  int fps;
  int volume;  // Audio volume (0-128, default 32)
} command_line_options_t, *command_line_options_ptr;

/** What a game option holds: a flag (--name), or --name=VALUE. */
typedef enum {
  OPTION_FLAG,    /**< target is a bool* */
  OPTION_INT,     /**< target is an int*; a non-negative integer */
  OPTION_FLOAT,   /**< target is a float*; a non-negative number */
  OPTION_STRING,  /**< target is a const char**; the value must not be empty */
} option_kind_t;

/** An option a game adds to the engine's own. */
typedef struct {
  const char* name;  /**< without the leading dashes */
  option_kind_t kind;
  void* target;      /**< written when the option is given */
  const char* help;  /**< one line for --help */
} custom_option_t;

/** The engine's defaults; a game may change them before parsing. */
command_line_options_t default_command_line_options(void);

/**
 * @brief Parse the arguments into `options`, leaving untouched what is not
 * given
 *
 * Understands the engine's options and the game's `custom` ones. An unknown
 * or malformed option prints a message and exits with failure.
 */
void parse_command_line_options(int argc, char* argv[],
                                command_line_options_ptr options,
                                const custom_option_t* custom,
                                size_t custom_count);

void print_help(const custom_option_t* custom, size_t custom_count);

#endif  // CORE_UTILS_COMMAND_LINE_H_
