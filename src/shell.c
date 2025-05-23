#include <lualib.h>
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
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

void shell(){
    while(run){
        prompt();

        char* input = NULL;
        size_t len = 0;
        int c = getc(stdin);
        while(c != '\n'){
            void* aux = realloc(input, len + 1);
            if(aux == NULL){
                exit(-1);
            }
            input = aux;
            input[len++] = c;
            c = getc(stdin);
        }

        // make it null terminated
        void* aux = realloc(input, len + 1);
        if(aux == NULL){
            exit(-1);
        }

        input = aux;
        input[len++] = 0;
        parse(input);
        free(input);

        if(reload){
            unload();
            load();
            reload = false;
        }
        update_variables();
    }
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
