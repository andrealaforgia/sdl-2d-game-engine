#include "command_line.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HELP "--help"
#define GRAPHICS_INFO "--graphics-info"
#define SHOW_FPS "--show-fps"
#define VSYNC "--vsync"
#define DISPLAY "--display="
#define DISPLAY_MODE "--display-mode="
#define WINDOW_MODE "--window-mode="
#define FPS "--fps="
#define VOLUME "--volume="

void print_help(const custom_option_t* custom, size_t custom_count) {
  puts("\noptions:");
  puts("\t" HELP ": print this help");
  puts("\t" GRAPHICS_INFO ": print info about the graphics system");
  puts("\t" VSYNC ": enable VSync for smoother rendering (default: off)");
  puts("\t" VOLUME
       "X: set audio volume 0-128 (default: 32, 0=silent, 128=max)");
  puts("\t" SHOW_FPS ": show frames-per-second stats during game");
  puts("\t" FPS "X: set target frame rate 1-300 (default: 60)");
  puts("\t" DISPLAY "X : use display X (default is 0: use " GRAPHICS_INFO
       " to show the number of available displays");
  puts("\t" DISPLAY_MODE
       "X: use display mode X (default is 0: use " GRAPHICS_INFO
       " to show all available display modes)");
  puts("\t" WINDOW_MODE
       "N: window mode (0=windowed, 1=fullscreen, 2=borderless, 3=maximized)");
  for (size_t i = 0; i < custom_count; ++i) {
    const char* value = custom[i].kind == OPTION_FLAG ? "" : "=X";
    printf("\t--%s%s: %s\n", custom[i].name, value, custom[i].help);
  }
  printf("\n");
}

static int extract_numeric_argument(const char* prefix, const char* argument,
                                    int* p_number) {
  size_t prefix_length = strlen(prefix);
  size_t argument_length = strlen(argument);
  if (!strncmp(prefix, argument, prefix_length) &&
      argument_length > prefix_length) {
    char* end;
    errno = 0;
    int64_t value = strtol(&argument[prefix_length], &end, 10);
    if (errno == ERANGE || end == &argument[prefix_length] || *end != '\0' ||
        value < 0 || value > INT_MAX) {
      fprintf(stderr, "Error: Invalid numeric option '%s'\n", argument);
      exit(EXIT_FAILURE);
    }
    *p_number = (int)value;
    return 1;
  }
  return 0;
}

static void fail_on(const char* what, const char* argument) {
  fprintf(stderr, "Error: %s '%s'\n", what, argument);
  fprintf(stderr, "Use --help to see all available options\n");
  exit(EXIT_FAILURE);
}

// Returns true if the argument is one of the game's own options
static bool parse_custom_argument(const char* argument,
                                  const custom_option_t* custom,
                                  size_t custom_count) {
  if (strncmp(argument, "--", 2) != 0) return false;
  const char* name = argument + 2;
  const char* equals = strchr(name, '=');
  size_t name_length = equals ? (size_t)(equals - name) : strlen(name);
  for (size_t i = 0; i < custom_count; ++i) {
    const custom_option_t* option = &custom[i];
    if (strlen(option->name) != name_length ||
        strncmp(option->name, name, name_length) != 0)
      continue;
    if (option->kind == OPTION_FLAG) {
      if (equals) fail_on("Option takes no value:", argument);
      *(bool*)option->target = true;
      return true;
    }
    if (!equals || !equals[1]) fail_on("Option needs a value:", argument);
    const char* value = equals + 1;
    char* end;
    errno = 0;
    if (option->kind == OPTION_STRING) {
      *(const char**)option->target = value;
    } else if (option->kind == OPTION_INT) {
      long number = strtol(value, &end, 10);
      if (errno == ERANGE || *end != '\0' || number < 0 || number > INT_MAX)
        fail_on("Invalid numeric option", argument);
      *(int*)option->target = (int)number;
    } else {
      double number = strtod(value, &end);
      if (errno == ERANGE || *end != '\0' || !(number >= 0.0))
        fail_on("Invalid numeric option", argument);
      *(float*)option->target = (float)number;
    }
    return true;
  }
  return false;
}

// Returns true if the argument was recognized, false otherwise
static bool parse_argument(const char* argument,
                           const command_line_options_ptr options) {
  int number;
  if (!strcmp(HELP, argument)) {
    options->help = true;
  } else if (!strcmp(SHOW_FPS, argument)) {
    options->show_fps = true;
  } else if (!strcmp(VSYNC, argument)) {
    options->vsync = true;
  } else if (!strcmp(GRAPHICS_INFO, argument)) {
    options->graphics_info = true;
  } else if (extract_numeric_argument(DISPLAY, argument, &number)) {
    options->display = number;
  } else if (extract_numeric_argument(DISPLAY_MODE, argument, &number)) {
    options->display_mode = number;
  } else if (extract_numeric_argument(WINDOW_MODE, argument, &number)) {
    int valid_window_mode = number >= 0 && number <= 3;
    if (!valid_window_mode) {
      fprintf(stderr, "Error: Invalid window mode %s (valid: 0-3)\n", argument);
      fprintf(stderr, "Use --help to see all available options\n");
      exit(EXIT_FAILURE);
    }
    options->window_mode = number;
  } else if (extract_numeric_argument(FPS, argument, &number)) {
    if (number < 1 || number > 300) {
      fprintf(stderr, "Error: Invalid FPS %s (valid: 1-300)\n", argument);
      fprintf(stderr, "Use --help to see all available options\n");
      exit(EXIT_FAILURE);
    }
    options->fps = number;
  } else if (extract_numeric_argument(VOLUME, argument, &number)) {
    if (number < 0 || number > 128) {
      fprintf(stderr, "Error: Invalid volume %s (valid: 0-128)\n", argument);
      fprintf(stderr, "Use --help to see all available options\n");
      exit(EXIT_FAILURE);
    }
    options->volume = number;
  } else {
    // Argument not recognized
    return false;
  }

  return true;
}

command_line_options_t default_command_line_options(void) {
  command_line_options_t defaults;
  command_line_options_ptr options = &defaults;
  options->help = false;
  options->graphics_info = false;
  options->show_fps = false;
  options->vsync = false;
  options->display = 0;
  options->display_mode = 0;
  options->window_mode = FULLSCREEN;  // Default to fullscreen
  options->fps = 60;
  options->volume = 32;  // Default volume (0-128 range)
  return defaults;
}

void parse_command_line_options(int argc, char* argv[],
                                command_line_options_ptr options,
                                const custom_option_t* custom,
                                size_t custom_count) {
  // Skip argv[0] which is the program name
  for (int i = 1; i < argc; i++) {
    if (parse_argument(argv[i], options)) continue;
    if (parse_custom_argument(argv[i], custom, custom_count)) continue;
    // Unrecognized argument
    if (strncmp(argv[i], "-", 1) == 0) {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      fprintf(stderr, "Use --help to see all available options\n");
      exit(EXIT_FAILURE);
    }
    // Not an option (doesn't start with a dash), ignore it
  }
}
