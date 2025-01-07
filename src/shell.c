#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void prompt(lua_State* L){
    lua_getglobal(L, "luall");
    lua_getfield(L, -1, "functions");
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

void shell(lua_State* L){
    while(run){
        prompt(L);
        char* input = malloc(1);
        size_t len = 0;
        int c = getc(stdin);
        input[len] = c;
        while(c != '\n'){
            int c = getc(stdin);
            void* aux = realloc(input, len + 1);
            if(aux == NULL){
                exit(-1);
            }
            input = aux;
            input[len++] = c;
        }
    }
}
