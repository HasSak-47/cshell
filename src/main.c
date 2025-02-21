#include <hot.h>
#include <state.h>

#include<dlfcn.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Prompt prompt = NULL;
HandleInput handle_input = NULL;
// UpdateVariables update_variables = NULL;
LuaSetup lua_setup = NULL;
LuaCleanup lua_cleanup = NULL;
void* handler = NULL;

int main(){
    init_shell_state();
    load();
    // event loop
    // there is no fucking way I can hot reload this shit
    while (running) {
        if(prompt == NULL){
            printf("fallback >");
        }
        else{
            prompt(L); 
        }
        get_current_state();
        handle_input(L);
        // update_variables(L);
        if (reload) {
            unload();
            load();
            reload = false;
        }
    }
    unload();
    end_shell_state();
    return 0;
}

void load(){
    // init blank lua state
    L = luaL_newstate();

    // load dynamic symbols
    char* cwd = getcwd(NULL, 0);
    printf("changing to dir: %s", hot_path);
    chdir(hot_path);
    int exit = system("make hot");
    if( exit != 0) {
        chdir(cwd);
        return;
    }

    handler = dlopen("units/bundle.so", RTLD_NOW);
    prompt = dlsym(handler, "prompt");
    handle_input = dlsym(handler, "handle_input");
    // update_variables = dlsym(handler, "update_variables");
    lua_setup = dlsym(handler, "lua_setup");
    lua_cleanup = dlsym(handler, "lua_cleanup");

    chdir(cwd);

    // init api
    lua_setup(L);
}

void unload(){
    lua_cleanup(L);
    if(handler != NULL){
        dlclose(handler);
        handler          = NULL;
        prompt           = NULL;
        handle_input     = NULL;
        // update_variables = NULL;
        lua_setup        = NULL;
        lua_cleanup      = NULL;
    }

    lua_close(L);
}
