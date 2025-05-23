#ifndef __SHELL_H__
#define __SHELL_H__

#include <lua.h>
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

extern lua_State* L;

void shell();

void load();
void setup();
/**
Update Luall.vars.* except env
env variables should are set to the values in Luall.vars.env
 */
void update_variables();
void set_env_variables();
void set_conf_variables();
void init_api();
void unload();

void prompt();
void parse(const char* prompt);

extern int last_return_code;
extern bool run;
extern bool reload;

extern char* init_path;
extern char* config_path;
extern char* hot_path;

#endif 
