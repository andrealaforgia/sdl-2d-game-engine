# SDL 2D Game Engine

A lightweight, modular 2D game engine built with C and SDL2, designed for creating retro-style games and platformers.

## Features

### Graphics
- **Texture loading and rendering** (PNG, JPG support via SDL2_image)
- **Primitive rendering** (lines, circles, polygons, pixels)
- **Scrolling background system** for platformers
- **Frame rate management** and VSync support
- **FPS tracking and display**
- **Multiple window modes** (windowed, fullscreen, borderless)

### Input
- **Keyboard input handling** with debouncing
- **Event system** for game events
- **Configurable key bindings**

### Math & Physics
- **Vector and point operations**
- **Geometry utilities** (circles, polygons, rotation)
- **Physics simulation** (velocity, acceleration, friction)
- **Animation interpolation** (easing functions)

### Audio
- **Sound effect playback** (WAV, OGG via SDL2_mixer)
- **Volume control**
- **Multi-channel audio mixing**

### Utilities
- **Timing and clock management**
- **Command-line argument parsing**
- **Logging system** with different severity levels
- **Memory management** (object pooling)
- **Type definitions** for consistency

## Project Structure

```
core/
├── graphics/       # Rendering, textures, display
│   ├── graphics.{c,h}
│   ├── texture.{c,h}
│   ├── frame.{c,h}
│   ├── fps_tracker.{c,h}
│   └── ...
├── input/          # Keyboard, events
│   ├── keyboard.{c,h}
│   └── events.{c,h}
├── math/           # Physics, geometry
│   ├── geometry.{c,h}
│   ├── physics.{c,h}
│   └── animate.{c,h}
├── audio/          # Sound system
│   └── audio.{c,h}
├── time/           # Timing utilities
│   └── clock.{c,h}
├── memory/         # Memory management
│   └── object_pool.{c,h}
├── events/         # Event system
│   └── event_system.{c,h}
└── utils/          # Common utilities
    ├── logger.h
    ├── types.h
    └── command_line.{c,h}
```

## Requirements

### Dependencies
- **SDL2** - Core graphics and window management
- **SDL2_image** - Image loading (PNG, JPG)
- **SDL2_mixer** - Audio playback

### Platforms
- macOS (tested)
- Linux (Ubuntu/Debian)
- Potentially Windows (with MinGW)

## Installation

### macOS
```bash
brew install sdl2 sdl2_image sdl2_mixer
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

## Usage

This engine is designed to be used as a **Git submodule** in your game projects.

### Adding to a New Project

```bash
# Create your game project
mkdir my-game
cd my-game
git init

# Add the engine as a submodule
git submodule add git@github.com:andrealaforgia/sdl-2d-game-engine.git engine

# Create your game structure
mkdir -p game/src/main
mkdir -p assets
```

### Makefile Integration

Add the engine directories to your Makefile:

```makefile
CC = gcc
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LFLAGS := $(shell sdl2-config --libs) -lSDL2_image -lSDL2_mixer

ENGINE_GRAPHICS_DIR = engine/core/graphics
ENGINE_MATH_DIR = engine/core/math
ENGINE_INPUT_DIR = engine/core/input
ENGINE_AUDIO_DIR = engine/core/audio
ENGINE_TIME_DIR = engine/core/time
ENGINE_UTILS_DIR = engine/core/utils
ENGINE_MEMORY_DIR = engine/core/memory
ENGINE_EVENTS_DIR = engine/core/events

GAME_MAIN_DIR = game/src/main

SRC = $(wildcard $(ENGINE_GRAPHICS_DIR)/*.c) \
      $(wildcard $(ENGINE_MATH_DIR)/*.c) \
      $(wildcard $(ENGINE_INPUT_DIR)/*.c) \
      $(wildcard $(ENGINE_AUDIO_DIR)/*.c) \
      $(wildcard $(ENGINE_TIME_DIR)/*.c) \
      $(wildcard $(ENGINE_UTILS_DIR)/*.c) \
      $(wildcard $(ENGINE_MEMORY_DIR)/*.c) \
      $(wildcard $(ENGINE_EVENTS_DIR)/*.c) \
      $(wildcard $(GAME_MAIN_DIR)/*.c)

INCLUDES = -I$(ENGINE_GRAPHICS_DIR) -I$(ENGINE_MATH_DIR) \
           -I$(ENGINE_INPUT_DIR) -I$(ENGINE_AUDIO_DIR) \
           -I$(ENGINE_TIME_DIR) -I$(ENGINE_UTILS_DIR) \
           -I$(ENGINE_MEMORY_DIR) -I$(ENGINE_EVENTS_DIR) \
           -I$(GAME_MAIN_DIR)

CFLAGS := -std=c99 -Wall -Wextra $(INCLUDES) $(SDL2_CFLAGS)
LFLAGS := $(SDL2_LFLAGS) -lm

OBJ = $(SRC:.c=.o)
TARGET = my-game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)
```

### Example Projects

- **[Asteroids](https://github.com/andrealaforgia/asteroids)** - A classic arcade-style asteroids game implementation showcasing the engine's capabilities for vector graphics, physics, and input handling.

## Cloning a Project with This Engine

When cloning a game that uses this engine:

```bash
# Clone with submodules
git clone --recurse-submodules <game-repo-url>

# OR if already cloned:
git clone <game-repo-url>
cd <game-dir>
git submodule init
git submodule update
```

## Updating the Engine

### In the Engine Repository

```bash
cd sdl-2d-game-engine
# Make your changes
git add .
git commit -m "Add new feature"
git push origin main
```

### In Your Game Project

```bash
cd my-game/engine
git pull origin main
cd ..
git add engine
git commit -m "Update engine to latest version"
git push
```

Or update all submodules at once:

```bash
git submodule update --remote --merge
```

## Use Cases

This engine is suitable for:
- 2D platformers with jumping and scrolling mechanics
- Retro-style arcade games
- Vector graphics games
- Casual 2D games with basic physics

## API Overview

### Graphics

```c
// Initialize graphics context
graphics_context_t ctx = init_graphics_context(display, mode, window_mode, vsync);

// Load and render textures
texture_t bg = load_texture(renderer, "background.png");
render_texture(renderer, &bg, x, y);

// Draw primitives
draw_line(ctx, x1, y1, x2, y2, color);
draw_circle(ctx, cx, cy, radius, color);
draw_filled_polygon(ctx, points, num_points, color);
```

### Player/Physics

```c
// Create player with physics
player_t player = create_player(screen_width, screen_height);

// Update with gravity and collision
update_player(&player, delta_time, jump_pressed);

// Render
render_player(renderer, &player, screen_center_x);
```

### Input

```c
// Initialize keyboard
keyboard_state_t kb = init_keyboard_state();

// Check for input
const Uint8* keys = SDL_GetKeyboardState(NULL);
if (keys[SDL_SCANCODE_SPACE]) {
    player_jump(&player);
}
```

### Audio

```c
// Initialize audio
audio_context_t audio = init_audio_context(volume);

// Load and play sounds
int jump_sound = load_sound(&audio, "jump.wav");
play_sound(&audio, jump_sound);
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## License

MIT License - See LICENSE file for details

## Credits

Created by Andrea Laforgia

Built with SDL2 by Sam Lantinga and contributors
