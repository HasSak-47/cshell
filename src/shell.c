#include <lualib.h>
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>
#include <cmds.h>

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
        }
    }
}

void* handler = NULL;
struct luaL_Reg* api_cmds = NULL;

void load(){
    printf("inited state\n");
    L = luaL_newstate();
    luaL_openlibs(L);

    printf("initing Luall\n");
    if (luaL_dofile(L, "./init.lua") != LUA_OK){
        luaL_error(L, "Could not init the Luall: %s\n", lua_tostring(L, -1));
    }

    printf("updating values\n");
    update_values();

    printf("building api\n");
    int exit = system("make cmd");
    if(exit != 0){
        printf("error could not build api\n");
        return;
    }
    else{
        handler = dlopen("units/cmds.so", RTLD_NOW);
        api_cmds = dlsym(handler, "api_cmds");
    }

    printf("init api\n");
    init_api();
}

void unload(){
    if(handler != NULL){
        dlclose(handler);
        handler = NULL;
        api_cmds = NULL;
    }
    lua_close(L);
}

int last_return_code = 0;
bool run = true;
bool reload = false;
