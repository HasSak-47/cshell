# Luall
This is an interactive Linux shell implemented in C with embedded Lua support, designed to provide a flexible and extensible command-line interface.
It uses Lua as its scripting language, and provides a minimal API to interact with the OS and the interaction loop, which also serves as a built in commands.

The shell allows modifications to its behavior at runtime including prompts, command parsing, and more.

## Features
- Built-in shell commands.
- Lua integration for scripting and automation.
- Environment variable management.
- Configurable initialization scripts.
- Configurable at runtime

## Demonstrations

## How to use
### dependencies
To build and run this project, you will need:
- **Lua** 5.4
- **gcc**
- **make**

to build all: `make all`
to run: `make run` or `./luall` after building

## Technical Details
### Runtime modifications

### Foot guns
Luall is the bridge between the interaction loop and the configuration, and there are not many guards to guaranty it's integrity and structure and the user has full control over it changes to it may crash the whole shell :D
