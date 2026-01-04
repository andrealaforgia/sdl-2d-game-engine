CC = gcc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    PLATFORM = LINUX
    INSTALL_CMD = sudo apt-get update && sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
    DEV_INSTALL_CMD = sudo apt-get update && sudo apt-get install -y cpplint clang-format libsdl2-ttf-dev
    AR = ar
else ifeq ($(UNAME_S), Darwin)
    PLATFORM = OSX
    INSTALL_CMD = brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf
    DEV_INSTALL_CMD = brew install cpplint clang-format sdl2_ttf
    AR = ar
else
    $(error Unsupported platform)
endif

SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LFLAGS := $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer -lSDL2_ttf

SRCDIR = .
CORE_GRAPHICS_DIR = core/graphics
CORE_MATH_DIR = core/math
CORE_INPUT_DIR = core/input
CORE_AUDIO_DIR = core/audio
CORE_TIME_DIR = core/time
CORE_UTILS_DIR = core/utils
CORE_MEMORY_DIR = core/memory
CORE_EVENTS_DIR = core/events

# Find all C source files in core directories
SRC = $(wildcard $(CORE_GRAPHICS_DIR)/*.c) $(wildcard $(CORE_MATH_DIR)/*.c) $(wildcard $(CORE_INPUT_DIR)/*.c) $(wildcard $(CORE_AUDIO_DIR)/*.c) $(wildcard $(CORE_TIME_DIR)/*.c) $(wildcard $(CORE_UTILS_DIR)/*.c) $(wildcard $(CORE_MEMORY_DIR)/*.c) $(wildcard $(CORE_EVENTS_DIR)/*.c)

HEADERS = $(wildcard $(SRCDIR)/*.h) \
          $(wildcard $(CORE_GRAPHICS_DIR)/*.h) $(wildcard $(CORE_MATH_DIR)/*.h) $(wildcard $(CORE_INPUT_DIR)/*.h) $(wildcard $(CORE_AUDIO_DIR)/*.h) $(wildcard $(CORE_TIME_DIR)/*.h) $(wildcard $(CORE_UTILS_DIR)/*.h) $(wildcard $(CORE_MEMORY_DIR)/*.h) $(wildcard $(CORE_EVENTS_DIR)/*.h)

OBJ = $(SRC:.c=.o)

# Add include paths
INCLUDES = -I. \
           -I$(CORE_GRAPHICS_DIR) -I$(CORE_MATH_DIR) -I$(CORE_INPUT_DIR) -I$(CORE_AUDIO_DIR) -I$(CORE_TIME_DIR) -I$(CORE_UTILS_DIR) -I$(CORE_MEMORY_DIR) -I$(CORE_EVENTS_DIR)

CFLAGS := -ggdb3 -Ofast --std=c99 -Wall -Wextra -pedantic-errors $(INCLUDES) $(SDL2_CFLAGS)
LFLAGS := $(SDL2_LFLAGS) -lm

# Library target for the engine
LIB_TARGET = libsdl2d.a

.PHONY: all install dev_install clean lint format

all: $(LIB_TARGET)

$(LIB_TARGET): $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install:
	git submodule update --init --recursive
	$(INSTALL_CMD)

dev_install:
	git submodule update --init --recursive
	$(DEV_INSTALL_CMD)

lint:
	cpplint --filter=-build/include_subdir,-legal/copyright,-runtime/threadsafe_fn,-readability/casting $(SRC) $(HEADERS)

clean:
	rm -f $(OBJ) $(LIB_TARGET)

format:
	clang-format -i -style=Google $(SRC) $(HEADERS)

show_sdl_config:
	@echo "Checking SDL2 Configuration..."
	@which sdl2-config || echo "sdl2-config not found"
	@sdl2-config --version || echo "Failed to get SDL2 version"
	@sdl2-config --cflags || echo "Failed to get SDL2 cflags"
	@sdl2-config --libs || echo "Failed to get SDL2 libs"
	@echo "Library Paths:"
	@ldconfig -p | grep SDL || echo "No SDL libraries found in ldconfig"

