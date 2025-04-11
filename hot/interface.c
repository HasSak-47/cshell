#include "utils.h"
#include <ctype.h>
#include <state.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <api.h>
#include <testing.h> 
#include <debug.h> 
#include <interface.h>
#include <input.h>

void update_lua_state(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

    lua_pushinteger(L, error);
    lua_setfield(L, -2, "error");

    lua_pushboolean(L, debug);
    lua_setfield(L, -2, "debug");

    lua_pushstring(L, host);
    lua_setfield(L, -2, "host");

    lua_pushstring(L, cwd);
    lua_setfield(L, -2, "cwd");

    lua_getfield(L, -1, "user");

    lua_pushstring(L, user.home);
    lua_setfield(L, -2, "home");

    lua_pushstring(L, user.name);
    lua_setfield(L, -2, "name");

    lua_pop(L, 2);
}

void prompt(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "prompts");
    lua_getfield(L, -1, "prompt");
    if(lua_pcall(L, 0, 1, 0) == 0){
        const char* prompt = lua_tostring(L, -1);
        printf("%s", prompt);
    }
    else{
        printf("%s\nfallback>", lua_tostring(L, -1));
    }
    lua_pop(L, lua_gettop(L));
}

// WARN: this is a bad name for the function
void handle_input(lua_State* L){
    update_lua_state(L);
    prompt(L);

    // since it is raw mode make sure to print the prompt
    fflush(stdout);

    char* input = interactive_input();

    // parse the function
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "inner");
    lua_getfield(L, -1, "parse");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, input);

        int error = lua_pcall(L, 1, 1, 0);
        if(error != LUA_OK){
            temporal_suicide_msg("Luall.inner.parse crashed\n");
            return;
        }
    }
    else{
        printf("error Luall.inner.parse is not a function");
    }
    lua_pop(L, 2);

    free(input);
}

// env metatable function
static int __env_index(lua_State* L){
    const char* okey = lua_tostring(L, -1);
    if(okey == NULL){
        lua_pushnil(L);
        return 1;
    }

    size_t len = strlen(okey);
    char* key = malloc(strlen(okey) + 1);
    for(size_t i = 0; i < len; ++i)
        key[i] = toupper(okey[i]);
    key[len] = 0;

    const char* value = getenv(key);
    if(value == NULL)
        lua_pushnil(L);
    else
        lua_pushstring(L, value);
    free(key);

    return 1;
}

// env metatable function
static int __env_newindex(lua_State* L){
    const char* okey = lua_tostring(L, -2);
    if(okey == NULL)
        return 0;

    size_t len = strlen(okey);
    char* key = malloc(strlen(okey) + 1);
    for(size_t i = 0; i < len; ++i)
        key[i] = toupper(okey[i]);
    key[len] = 0;

    if(lua_isnil(L, -1)){
        unsetenv(key);
        return 0;
    }
    const char* value = lua_tostring(L, -1);
    setenv(key, value, 1);

    return 0;
}

// create metatable env
void create_env(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

    lua_newtable(L);
    luaL_newmetatable(L, "__env");
    lua_pushcfunction(L, __env_index);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, __env_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);

    lua_setfield(L, -2, "env");
    lua_pop(L, 2);
}

void lua_setup(lua_State* L){
    debug_printf("loading lua stdlibs\n");
    // luaL_openlibs(L);
    luaL_requiref(L, "base", luaopen_base, true);
    luaL_requiref(L, "math", luaopen_math, true);
    luaL_requiref(L, "table", luaopen_table, true);
    luaL_requiref(L, "package", luaopen_package, true);
    luaL_requiref(L, "string", luaopen_string, true);

    // load blueprint
    debug_printf("loading blueprint\n");
    if(luaL_dofile(L, init_path) != LUA_OK){
        // if it doesn't load just nuke it
        running = false;
        temporal_suicide_msg("could not load init");
        return;
    }
    debug_printf("setting up state\n");
    update_lua_state(L);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_pushstring(L, hot_path);
    lua_setfield(L, -2, "hot_path");

    lua_pushstring(L, config_path);
    lua_setfield(L, -2, "config_path");

    lua_pushstring(L, init_path);
    lua_setfield(L, -2, "init_path");

    lua_pop(L, 2);

    create_env(L);

    // blueprint setup
    lua_createtable(L, 4, 4);
    luaL_setfuncs(L, api, 0);
    lua_setfield(L, -2, "api");
    lua_getfield(L, -1, "api");

    lua_createtable(L, 3, 3);
    luaL_setfuncs(L, api_process, 0);
    lua_setfield(L, -2, "process");

    lua_createtable(L, 3, 3);
    luaL_setfuncs(L, api_pipe, 0);
    lua_setfield(L, -2, "pipe");

    lua_pop(L, 1);

    // load user config
    if(luaL_dofile(L, config_path) != LUA_OK){
        // just like before if it doesn't load just nuke it
        running = false;
        return;
    }

    // setup makes sure that everything in the config
    // that was a relative path is now an absolute path
    lua_getfield(L, -1, "setup");
    if(lua_isfunction(L, -1)){
        lua_call(L, 0, 0);
    }

    // just clean the stack
    lua_pop(L, lua_gettop(L));

    // bring the expanded paths to the global state
    free(hot_path);
    free(config_path);
    free(init_path);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_getfield(L, -1, "hot_path");
    hot_path = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, "config_path");
    config_path = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, "init_path");
    init_path = strdup(lua_tostring(L, -1));
    lua_pop(L, lua_gettop(L));
}

void lua_cleanup(lua_State* L){
    // there is nothing to do yet...
}

char* get_history(lua_State* state, int index){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "inner");
    lua_getfield(L, -1, "history");

    lua_pushinteger(L, index);
    int error = lua_pcall(L, 1, 1, 0);

    if (error != 0) {
        lua_pop(L, lua_gettop(L));
        return NULL;
    }

    const char* s = lua_tostring(L, -1);
    if (s == NULL) 
        return NULL;

    char* copy = strdup(s);
    return copy;
}
