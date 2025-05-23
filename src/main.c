#include <errno.h>
#include <utils.h>
#include <testing.h>

#include <hot.h>
#include <state.h>

#include <dlfcn.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

HandleInput handle_input = NULL;
LuaSetup lua_setup = NULL;
LuaCleanup lua_cleanup = NULL;
#ifdef TEST
Tester tester = NULL;
#endif
void* handler = NULL;

struct termios orig_termios;
bool got_original = false;

void unset_raw_mode(){

    if(got_original)
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    else{
        if (debug) {
            printf("there is no original termios?\n");
        }
    }
}

void set_raw_mode(){
    if(!got_original){
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(unset_raw_mode);
        got_original = true;
    }

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#ifdef TEST
int test();
#endif

int main(){
#ifdef TEST
    test();
    return 0;
#endif
    set_raw_mode();

    init_shell_state();
    get_current_state();
    load();
    // interaction loop
    // this is so fucking ass
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
    if(debug)
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
    if(debug){
        printf("changing to dir: %s\n", hot_path);
    }
    chdir(hot_path);
    int exit = system("make hot");
    if( exit != 0) {
        chdir(cwd);
        return;
    }

    if(debug){
        printf("loading handler and bundle\n");
    }

    // RTLD_LAZY because for some reason in fedora atomic inside a toolbox
    // it doesn't load :)
    handler = dlopen("./units/bundle.so", RTLD_LAZY);

    if (handler == NULL) {
        unrecoverable_error("could not find bundle");
    }

    handle_input = dlsym(handler, "handle_input");
    lua_setup    = dlsym(handler, "lua_setup");
    lua_cleanup  = dlsym(handler, "lua_cleanup");

    if(debug){
        printf("symbols loaded:\n");
        printf("handler: %p\n", handler);
        printf("handle_input: %p\n", handle_input);
        printf("lua_setup: %p\n", lua_setup);
        printf("lua_input: %p\n", lua_cleanup);
    }

#ifdef TEST
    tester = dlsym(handler, "__test");
    if(tester == NULL){
        temporal_suicide_msg("could not load test function");
    }
#endif

    chdir(cwd);
    free(cwd);

    // init api
    if(debug){
        printf("initing api\n");
    }
    lua_setup(L);
}

void unload(){
    lua_cleanup(L);
    if(handler != NULL){
        dlclose(handler);
        handler          = NULL;
        handle_input     = NULL;
        lua_setup        = NULL;
        lua_cleanup      = NULL;
    }

    lua_close(L);
}
