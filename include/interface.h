#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <lua.h>

void handle_input(lua_State*);
void lua_setup(lua_State*);
void lua_cleanup(lua_State*);

void unload();
void load();

#endif
