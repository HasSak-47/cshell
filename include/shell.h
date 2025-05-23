#ifndef __SHELL_H__
#define __SHELL_H__

#include <lua.h>
#include <stdbool.h>

extern lua_State* L;

void shell();

void load();
void unload();

extern int last_return_code;
extern bool run;

#endif 
