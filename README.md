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
![luall](https://github.com/user-attachments/assets/14467c20-61e2-4202-a509-b09e3e581fdb)

## How to use
### dependencies
To build and run this project, you will need:
- **Lua** 5.4
- **gcc**
- **make**

### building
- Clone the project `git clone -b stable https://github.com/HasSak-47/cshell.git`
- to build all: `make all`
- to run: `make run` or `./luall` after building


## Technical Details
### Runtime modifications

### Foot guns
Luall is the bridge between the interaction loop and the configuration, and there are not many guards to guaranty it's integrity and structure and the user has full control over it changes to it may crash the whole shell :D
