#ifndef __HOT_H__
#define __HOT_H__

#include "testing.h"
#include <lua.h>

typedef void (*HandleInput)(lua_State*);
typedef void (*UpdateVariables)(lua_State*);
typedef void (*LuaSetup)(lua_State*);
typedef void (*LuaCleanup)(lua_State*);

#ifdef TEST
typedef int (*Tester)(lua_State*);
extern Tester tester;
int test();
#endif

// stuff that will be hot loaded
extern HandleInput handle_input;
// extern UpdateVariables update_variables;
extern LuaSetup lua_setup;
extern LuaCleanup lua_cleanup;

extern void* handler;
void unload();
void load();

#endif
