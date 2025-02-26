# Luall
This is an interactive shell written in C and Lua, designed to provide a flexible and extensible command-line interface.
It allows user to execute built-in commands, use embedded scripting language for automation, customization, and extensibility.

\[pipes not yet implemented\]
A single line interpreted as a normal "bash-like" command, e.g. `ls --lA >> out.txt` would have the same behavior as bash.
But if there is more than a single line it would be interpreted as a Lua script.

## Features
- Built-in shell commands for file and process management.
- Lua integration for scripting and automation.
- Customizable prompts.
- Environment variable management.
- Configurable initialization scripts.

## Lua scripting
## Technical Details
### Architecture
#### Interaction loop

#### Hot reloading
### Extensibility
### Partial hot reloading
### Foot guns
Luall is the bridge between the interaction loop and the configuration, and there are not many guards to guaranty it's integrety and structure and the user has full control over it changes to it may crash the whole shell :D

## !!! MISSING PARTS !!!
