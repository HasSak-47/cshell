#ifndef __SHELL_H__
#define __SHELL_H__

#include <lua.h>
#include <stdbool.h>

void shell(lua_State* L);
extern int last_return_code;
extern bool run;

#endif 
