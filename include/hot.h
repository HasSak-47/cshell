#ifndef __HOT_H__
#define __HOT_H__

#include <lua.h>

typedef void (*Prompt)(lua_State*);
typedef void (*HandleInput)(lua_State*);
typedef void (*UpdateVariables)(lua_State*);
typedef void (*LuaSetup)(lua_State*);
typedef void (*LuaCleanup)(lua_State*);

// stuff that will be hot loaded
extern Prompt prompt;
extern HandleInput handle_input;
// extern UpdateVariables update_variables;
extern LuaSetup lua_setup;
extern LuaCleanup lua_cleanup;

extern void* handler;
void unload();
void load();

#endif
