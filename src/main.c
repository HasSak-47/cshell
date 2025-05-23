#include <hot.h>
#include <state.h>

#include<dlfcn.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

HandleInput handle_input = NULL;
// UpdateVariables update_variables = NULL;
LuaSetup lua_setup = NULL;
LuaCleanup lua_cleanup = NULL;
void* handler = NULL;

int main(){
    init_shell_state();
    get_current_state();
    load();
    // event loop
    // there is no fucking way I can hot reload this shit
    while (running) {
        handle_input(L);
        if (reload) {
            printf("reloading...\n");
            unload();
            load();
            reload = false;
        }
        get_current_state();
    }
    printf("graceful exit...\n");
    unload();
    end_shell_state();
    return 0;
}

void load(){
    // init blank lua state
    L = luaL_newstate();

    // load dynamic symbols
    char* cwd = getcwd(NULL, 0);
    printf("changing to dir: %s\n", hot_path);
    chdir(hot_path);
    int exit = system("make hot");
    if( exit != 0) {
        chdir(cwd);
        return;
    }

    handler = dlopen("units/bundle.so", RTLD_NOW);
    handle_input = dlsym(handler, "handle_input");
    // update_variables = dlsym(handler, "update_variables");
    lua_setup = dlsym(handler, "lua_setup");
    lua_cleanup = dlsym(handler, "lua_cleanup");

    chdir(cwd);
    free(cwd);

    // init api
    lua_setup(L);
}

void unload(){
    lua_cleanup(L);
    if(handler != NULL){
        dlclose(handler);
        handler          = NULL;
        handle_input     = NULL;
        // update_variables = NULL;
        lua_setup        = NULL;
        lua_cleanup      = NULL;
    }

    lua_close(L);
}
