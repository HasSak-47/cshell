#include <state.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int api_exit(lua_State* L){
    running = false;
    return 0;
}

static int api_reload(lua_State* L){
    reload = true;
    return 0;
}

static int api_cd(lua_State* L){
    const char* path = lua_tostring(L, -1);
    if( path == NULL ){
        return 0;
    }

    error = chdir(path);

    return 0;
}


#define REG(name) { #name, api_##name }

luaL_Reg api[] = {
    REG(cd),
    REG(exit),
    REG(reload),
    {NULL, NULL},
};

void handle_input(lua_State* L){
    // NOTE: dumb way to do it
    char* input = NULL;
    size_t len = 0; 
    int c = getc(stdin);
    while (c != '\n') {
        void* aux = realloc(input, len + 1);
        if (aux == NULL) {
            exit(-1);
        }
        input = aux;
        input[len++] = c;
        c = getc(stdin);
    }

    // make null terminated
    void* aux = realloc(input, len + 1);
    if (aux == NULL) 
        exit(-1);
    input = aux;
    input[len++] = 0;

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "inner");
    lua_getfield(L, -1, "parse");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, input);

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

    free(input);
}

void lua_setup(lua_State* L){
    luaL_openlibs(L);

    // load blueprint
    if(luaL_dofile(L, init_path) != LUA_OK){
        // if it doesn't load just nuke it
        running = false;
        return;
    }

    // init api
    lua_getglobal(L, "Luall");
    lua_createtable(L, 4, 4);
    luaL_setfuncs(L, api, 0);
    lua_setfield(L, -2, "api");

    lua_pop(L, 1);
}

void lua_cleanup(lua_State* L){
    // ...
}
