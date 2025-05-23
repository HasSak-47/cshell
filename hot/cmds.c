// here all the Luall.api functions are loaded
//
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>


static int api_exit(lua_State* L) {
    run = false;
    return 0;
}

static int api_cd(lua_State* L) {
    size_t argc = lua_gettop(L);
    if(argc != 1){
        printf("invalid number of arguments\n");
        last_return_code = -1;
        return -1;
    }
    const char* path = lua_tostring(L, -1);
    if(path == NULL){
        printf("not a valid path\n");
        last_return_code = -1;
        return 0;
    }

    last_return_code = chdir(path);
    if(last_return_code == -1){
        printf("could not change dir to %s", path);
        perror(NULL);
        printf("\n");
    }
    return 0;
}

static int api_reload(lua_State *L){
    reload = true;
    return 0;
}

static int api_execp(lua_State *L){
    size_t argc = lua_gettop(L);
    printf("number of arguments: %lu\n", argc);
    for(size_t i = 1; i <= argc; ++i){
        const char* arg = lua_tostring(L, i);
        printf("arg: %s\n", arg);
    }
    const char* path = getenv("path");
    if(path == NULL){
        last_return_code = -1;
        return 0;
    }
    return 0;
}

static int api_set_env(lua_State* L){
    const char* name = lua_tostring(L, 1);
    const char* val = lua_tostring(L, 2);
    setenv(name, val, 1);
    return 0;
}

struct luaL_Reg api[] = {
    {"exit", api_exit},
    {"cd", api_cd},
    {"reload", api_reload},
    {"exec", api_execp},
    {"add_env", api_set_env},

    {NULL, NULL}
};

// "constructor"
struct luaL_Reg* get_api(){
    return api;
}

// "destructor"
void take_api(struct luaL_Reg* api){ }
