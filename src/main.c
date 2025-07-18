#include <bindgen.h>
#include <debug.h>
#include <hot.h>
#include <path.h>
#include <state.h>
#include <string.h>
#include <testing.h>
#include <utils.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

HandleInput handle_input = NULL;
LuaSetup lua_setup       = NULL;
LuaCleanup lua_cleanup   = NULL;
#ifdef TEST
Tester tester = NULL;
#endif
void* handler = NULL;

struct termios orig_termios;
bool got_original = false;

void unset_raw_mode() {
    if (got_original)
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    else
        debug_printf("there is no original termios?\n");
}

void set_raw_mode() {
    if (!got_original) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(unset_raw_mode);
        got_original = true;
    }

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#ifdef TEST
int test();
#endif

int main(const int argc, const char* argv[]) {
    struct Args* args  = NULL;
    const char* script = NULL;

    args = parse_args(argc, argv);

    if (is_debug(args)) {
        debug = true;
    }
    script = get_script(args);
    if (script != NULL) {
        debug_printf("running scripting %s\n", script);
    }
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
    debug_printf("running: %i\n", running);
    if (script != NULL) {
        luaL_dofile(L, script);
    }
    else {
        while (running) {
            error = 0;
            debug_printf("handling input\n");
            handle_input(L);
            if (reload) {
                printf("reloading...\n");
                unload();
                load();
                reload = false;
            }
            debug_printf("reloading state\n");
            get_current_state();
        }
    }
    if (debug)
        printf("graceful exit...\n");
    free_args(args);
    unload();
    end_shell_state();
    return 0;
}

void chdir_path(struct Path* path) {
    char* _path = get_path_string(*path);
    printf("changing path to: %s\n", _path);
    chdir(_path);
    free(_path);
}

void load() {
    // init blank lua state
    L = luaL_newstate();

    // load dynamic symbols
    chdir_path(&hot_path);
    int exit = system("make hot");
    if (exit != 0) {
        chdir_path(&cwd);
        return;
    }

    debug_printf("loading handler and bundle\n");

    // RTLD_LAZY because for some reason in fedora atomic
    // inside a toolbox it doesn't load :)
    handler = dlopen("./units/bundle.so", RTLD_LAZY);

    if (handler == NULL) {
        unrecoverable_error("could not find bundle");
    }

    handle_input = dlsym(handler, "handle_input");
    lua_setup    = dlsym(handler, "lua_setup");
    lua_cleanup  = dlsym(handler, "lua_cleanup");

    debug_printf("symbols loaded:\n");
    debug_printf("handler: %p\n", handler);
    debug_printf("handle_input: %p\n", handle_input);
    debug_printf("lua_setup: %p\n", lua_setup);
    debug_printf("lua_input: %p\n", lua_cleanup);

#ifdef TEST
    tester = dlsym(handler, "__test");
    if (tester == NULL) {
        temporal_suicide_msg("could not load test function");
    }
#endif

    // init api
    debug_printf("initing api\n");
    lua_setup(L);

    chdir_path(&cwd);
}

void unload() {
    lua_cleanup(L);
    if (handler != NULL) {
        dlclose(handler);
        handler      = NULL;
        handle_input = NULL;
        lua_setup    = NULL;
        lua_cleanup  = NULL;
    }

    lua_close(L);
}
