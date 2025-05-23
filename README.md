# Luall
This is an interactive *nix shell written in C and Lua, designed to provide a flexible and extensible command-line interface.
It allows user to execute built-in commands, use embedded scripting language for automation, customization, and extensibility.

## Features
- Built-in shell commands for file and process management.
- Lua integration for scripting and automation.
- Environment variable management.
- Configurable initialization scripts.
- Lua Configurable at runtime

## How to use
### dependencies
To build and run this project, you will need:
- **Lua*** 5.4
- **gcc***
- **make***

to build all: `make all`
to run: `make run` or `./luall` after building

## Lua scripting
## Technical Details
### Architecture
#### Interaction loop
#### Hot reloading

### Extensibility
### Partial hot reloading
### Foot guns
Luall is the bridge between the interaction loop and the configuration, and there are not many guards to guaranty it's integrity and structure and the user has full control over it changes to it may crash the whole shell :D

## TODO:
- Expand `Luall.api`
- Pipes
- Autocomplete
- Cursor
- History
- Modules
