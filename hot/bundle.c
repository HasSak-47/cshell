#include <state.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>

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


static int api_exec(lua_State* L){
}

#define REG(name) { #name, api_##name }

luaL_Reg api[] = {
    REG(cd),
    REG(exit),
    REG(reload),
    {NULL, NULL},
};

void update_lua_state(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

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
    lua_call(L, 0, 1);
    const char* prompt = lua_tostring(L, -1);
    printf("%s", prompt);
}

// WARN: this is a bad name for the function
void handle_input(lua_State* L){
    update_lua_state(L);
    prompt(L);

    // NOTE: dumb way to do increase an array len
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

    // make the input null terminated
    void* aux = realloc(input, len + 1);
    if (aux == NULL) 
        exit(-1);
    input = aux;
    input[len++] = 0;

    // parse the function
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

    // blueprint setup
    lua_createtable(L, 4, 4);
    luaL_setfuncs(L, api, 0);
    lua_setfield(L, -2, "api");

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
    // ...
}
