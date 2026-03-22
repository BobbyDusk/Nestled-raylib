# Nestled

A C++ game foundation built on [raylib](https://github.com/raysan5/raylib) with an [RmlUi](https://github.com/mikke89/RmlUi) HTML/CSS UI layer.

## Features

- raylib window with OpenGL rendering
- RmlUi for HTML/CSS-driven UI (menus, HUD, etc.)
- Keyboard/mouse input bridged from raylib to RmlUi
- Single self-contained binary (all dependencies statically linked)

## Dependencies

| Library | Version |
|---|---|
| [raylib](https://github.com/raysan5/raylib) | 5.5 |
| [RmlUi](https://github.com/mikke89/RmlUi) | 6.2 |
| [flecs](https://github.com/SanderMertens/flecs) | 4.0.4 |

Dependencies are fetched and built automatically via CMake `FetchContent`.

## Requirements

- CMake 3.x
- clang / clang++
- [just](https://github.com/casey/just)

## Building

```sh
just configure   # CMake configure (Debug)
just build       # Compile
```

Or in one step:

```sh
just rebuild     # Clean + configure + build from scratch
```

For a release build:

```sh
just release     # Configure as MinSizeRel + build
```

## Running

```sh
just run
```

The binary is at `build/Nestled`. Run it from the project root so it can find the `assets/` directory.

## Keybindings

| Key | Action |
|---|---|
| `4` | Toggle FPS display |
| `5` | Toggle VSync |
| `6` | Toggle RmlUi debugger |
| `ESC` | Quit |

## Project Structure

```
src/
  main.cpp                  # Window init and main loop
  RmlUiLayer.h/.cpp         # RmlUi lifecycle and input handling
  RaylibSystemInterface.h   # Rml::SystemInterface backed by raylib
assets/
  fonts/                    # Font files
  ui/                       # RmlUi documents (.rml) and stylesheets (.rcss)
```
