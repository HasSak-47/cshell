// here all the Luall.api functions are loaded

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>


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

static int api_in_dir(lua_State* L){
    const char* path = lua_tostring(L, -2);
    const char* name = lua_tostring(L, -1);
    if(path == NULL || name == NULL){
        lua_pop(L, 2);
        lua_pushboolean(L, false);
        return 1;
    }

    struct dirent* entry = NULL;
    DIR* dir = opendir(path);
    while((entry = readdir(dir)) != NULL){
        bool eq = strcmp(entry->d_name, name) == 0;
        if(eq){
            lua_pop(L, 2);
            lua_pushboolean(L, true);
            return 1;
        }
    }

    lua_pop(L, 2);
    lua_pushboolean(L, false);
    return 1;
}

static int api_execp(lua_State *L){
    size_t argc = lua_gettop(L);
    printf("number of arguments: %lu\n", argc);
    for(size_t i = 1; i <= argc; ++i){
        const char* arg = lua_tostring(L, i);
        printf("arg: %s\n", arg);
    }



    return 0;
}

struct luaL_Reg api[] = {
    {"exit", api_exit},
    {"cd", api_cd},
    {"reload", api_reload},
    {"exec", api_execp},
    {"in_dir", api_in_dir},

    {NULL, NULL}
};

// "constructor"
struct luaL_Reg* get_api(){
    return api;
}

// "destructor"
void take_api(struct luaL_Reg* api){ }
