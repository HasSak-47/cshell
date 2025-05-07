
#include <lauxlib.h>
#include <lua.h>

#include <debug.h>
#include <path.h>
#include <state.h>
#include <testing.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <unistd.h>

// event loop control

bool reload  = false;
bool running = true;
lua_State* L = NULL;

// Luall.vars.config

struct Path config_path = {};
struct Path init_path   = {};
struct Path hot_path    = {};

char** history = NULL;

// Luall.vars

struct User user = {NULL, NULL};
struct Path cwd  = {};
char* host       = NULL;
// last error code
int error = 0;
#ifdef TEST
bool debug = true;
#else
bool debug = false;
#endif /* ifdef TEST */

/**
 * sets the state of the shell
 */
void init_shell_state() {
    host = malloc(256);
    gethostname(host, 256);

    uid_t uid = getuid();
    // no getpwuid_r cuz it is ez and I (hopefully) just need a single thread
    struct passwd* p = getpwuid(uid);
    user.name        = strdup(p->pw_name);
    user.home        = parse_path(p->pw_dir);

    const char* _cwd = getenv("PWD");
    cwd              = parse_path(_cwd);

    init_path   = parse_path(INIT_PATH);
    hot_path    = parse_path(HOT_PATH);
    config_path = parse_path(INIT_PATH);

    expand_path(&init_path, &cwd);
    expand_path(&hot_path, &cwd);
    expand_path(&config_path, &cwd);
}

/**
 * updates state where it is needed
 */
void get_current_state() {}

/**
 * cleanins the shell state
 */
void end_shell_state() {
    free(host);

    free(user.name);

    debug_printf("deleting cwd\n");
    destruct_path(&cwd);
    debug_printf("deleting home\n");
    destruct_path(&user.home);
    debug_printf("deleting init\n");
    destruct_path(&init_path);
    debug_printf("deleting hot\n");
    destruct_path(&hot_path);
    debug_printf("deleting config\n");
    destruct_path(&config_path);
}
