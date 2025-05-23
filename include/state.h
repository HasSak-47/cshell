#ifndef __STATE_H__
#define __STATE_H__

#ifndef INIT_PATH
#define INIT_PATH "./init.lua"
#endif

#ifndef HOT_PATH
#define HOT_PATH "./"
#endif

#ifndef CONFIG_PATH
#define CONFIG_PATH "./config.lua"
#endif

#include <stdbool.h>
#include <lua.h>

#include "./path.h"

extern bool running;
extern bool reload;
extern lua_State* L;

// Luall.vars.config
extern char* config_path;
extern char* init_path;
extern char* hot_path;

// Luall.vars
struct User{
    char* name;
    // TODO: change to path struct
    char* home;
};

extern struct Path path;
extern struct User user;
extern char* cwd;
extern char* host;
extern int error;
extern bool debug;

// TODO:
// Luall.vars.env

void init_shell_state();
void end_shell_state();

void get_current_state();
void update_current_state();

void set_raw_mode();
void unset_raw_mode();

#endif
