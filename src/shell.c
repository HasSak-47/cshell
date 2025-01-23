#include <lualib.h>
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <dlfcn.h>

lua_State* L = NULL;

void prompt(){
    lua_getglobal(L, "Luall");
        lua_getfield(L, -1, "prompts");
            lua_getfield(L, -1, "prompt");
    if (lua_isfunction(L, -1)) {
        int error = lua_pcall(L, 0, 1, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            printf("fallback >");
            return;
        }

        const char* va = lua_tostring(L, -1);
        printf("%s", va);
    }
    lua_pop(L, 2);
}

void parse(const char* prompt){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "inner");
	lua_getfield(L, -1, "parse");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, prompt);

        int error = lua_pcall(L, 1, 1, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            return;
        }
    }
    else{
        printf("error Luall.inner.parse is not a function");
    }
    lua_pop(L, 2);
}

void load_env(){
    char** end = __environ;
    while(*end++ != NULL);

    size_t size = (end - 1) - __environ;

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "env");
    for(size_t i = 0; i < size; ++i){
        // value separator
        char* val = __environ[i];
        while(*val != '=' && val != NULL)
            val++;

        char* end = val;
        while(*end++ != '\0');

        size_t len = val - __environ[i];
        char* name = calloc(len + 1, 1);
        strncpy(name, __environ[i], len);

        printf("loading [%lu] (%lu): %*.s\n", i, len, (int)len, __environ[i]);
        if(val == end)
            lua_pushnil(L);
        else
            lua_pushstring(L, val + 1);
        lua_setfield(L, -2, name);

        free(name);
    }
    lua_pop(L, 3);
}

void* handler = NULL;
struct luaL_Reg* (* get_api)() = NULL;
void (* take_api)(struct luaL_Reg*) = NULL;

void load(){
    printf("inited state\n");
    L = luaL_newstate();
    luaL_openlibs(L);

    printf("initing Luall\n");
    if (luaL_dofile(L, "./init.lua") != LUA_OK){
        luaL_error(L, "Could not init the Luall: %s\n", lua_tostring(L, -1));
    }

    printf("updating variables...\n");
    update_variables();

    printf("building api\n");
    int exit = system("make hot");
    if(exit != 0){
        printf("could not build api\n");
        return;
    }
    else{
        handler = dlopen("units/cmds.so", RTLD_NOW);
        get_api = dlsym(handler, "get_api");
        take_api = dlsym(handler, "take_api");
    }

    printf("init api\n");
    init_api();
    printf("loading env\n");
    load_env();
}

void init_api(){
    lua_getglobal(L, "Luall");

    // WARN: magic numbres bad!
    lua_createtable(L, 4, 4);
    luaL_setfuncs(L, get_api(), 0);

    lua_setfield(L, -2, "api");

    lua_pop(L, 1);
}

void unload(){
    if(handler != NULL){
        dlclose(handler);
        handler = NULL;
        get_api = NULL;
        take_api = NULL;
    }
    lua_close(L);
}

int last_return_code = 0;
bool run = true;
bool reload = false;
