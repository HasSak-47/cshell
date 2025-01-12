#ifndef __SHELL_H__
#define __SHELL_H__

#include <lua.h>
#include <stdbool.h>

extern lua_State* L;

void shell();

void load();
void update_values();
void init_api();
void unload();

void prompt();
void parse(const char* prompt);

extern int last_return_code;
extern bool run;
extern bool reload;

#endif 
