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

char* init_path = NULL;
char* config_path = NULL;
char* hot_path = NULL;

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
        while(*end != '\0')
            end++;

        size_t len = val - __environ[i];
        char* name = calloc(len + 1, 1);
        strncpy(name, __environ[i], len);

        if(val == end)
            lua_pushnil(L);
        else
            lua_pushstring(L, val + 1);
        lua_setfield(L, 2, name);
        free(name);
    }

    lua_pop(L, 2);
}

void* handler = NULL;
struct luaL_Reg* (* get_api)() = NULL;
void (* take_api)(struct luaL_Reg*) = NULL;

void load(){
    printf("inited state\n");
    L = luaL_newstate();
    luaL_openlibs(L);

    printf("initing Luall at: %s\n", init_path);
    if (luaL_dofile(L, init_path) != LUA_OK){
        luaL_error(L, "Could not init the Luall: %s\n", lua_tostring(L, -1));
    }

    printf("updating variables...\n");
    update_variables();

    printf("building api\n");

    // this is so wack
    char* cwd = getcwd(NULL, 0);
    if(cwd == NULL){
        printf("error: could not move to hot dir\n");
        last_return_code = -1;
        return;
    }

    chdir(hot_path);
    int exit = system("make hot");
    if(exit != 0){
        printf("error: could not build api\n");
        last_return_code = -1;
        chdir(cwd);
        free(cwd);
        return;
    }
    else{
        handler = dlopen("units/cmds.so", RTLD_NOW);
        get_api = dlsym(handler, "get_api");
        take_api = dlsym(handler, "take_api");
    }
    chdir(cwd);
    free(cwd);

    printf("init api\n");
    init_api();
    printf("loading env\n");
    load_env();

    printf("running user config\n");
    if (luaL_dofile(L, config_path) != LUA_OK){
        luaL_error(L, "Could not init the Luall: %s\n", lua_tostring(L, -1));
    }
}

void setup(){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "setup");
    lua_call(L, 0, 0);
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
    lua_close(L);
    if(handler != NULL){
        dlclose(handler);
        handler = NULL;
        get_api = NULL;
        take_api = NULL;
    }
}

int last_return_code = 0;
bool run = true;
bool reload = false;
