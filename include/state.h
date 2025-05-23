#ifndef __STATE_H__
#define __STATE_H__

#include <stdbool.h>

#ifndef INIT_PATH
#define INIT_PATH "./init.lua"
#endif

#ifndef HOT_PATH
#define HOT_PATH "./hot.lua"
#endif

#ifndef CONFIG_PATH
#define CONFIG_PATH "./config.lua"
#endif


// Luall.vars.config
extern char* config_path;
extern char* init_path;
extern char* hot_path;
extern bool running;

// Luall.vars
struct User{
    char* name;
    char* home;
};
struct User user;
extern char* cwd;
extern char* host;
extern int error;
extern bool debug;

// TODO:
// Luall.vars.env

void init_shell_state();
void end_shell_state();

#endif
