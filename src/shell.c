#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void prompt(lua_State* L){
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
}

void parse(lua_State* L, const char* prompt){
    lua_getglobal(L, "Luall");
        lua_getfield(L, -1, "api");
            lua_getfield(L, -1, "inner");
	            lua_getfield(L, -1, "parse");
    lua_pushstring(L, prompt);
    if (lua_isfunction(L, -1)) {
        int error = lua_pcall(L, 1, 0, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            printf("fallback >");
            return;
        }

        // const char* va = lua_tostring(L, -1);
        // printf("%s", va);
    }
}

void shell(lua_State* L){
    while(run){
        prompt(L);

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
        parse(L, input);
        free(input);
    }
}

int last_return_code = 0;
bool run = true;
