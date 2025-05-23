#include "debug.h"
#include "path.h"
#include <lauxlib.h>
#include <lua.h>

#include <state.h>
#include <stdio.h>
#include <testing.h>

#include <pwd.h>

#include <stdlib.h>
#include <string.h>

#include <unistd.h>

// event loop control

bool reload  = false;
bool running = true;
lua_State* L = NULL;

// Luall.vars.config

char* config_path = NULL;
char* init_path   = NULL;
char* hot_path    = NULL;

char** history = NULL;

// Luall.vars

struct User user = { NULL, NULL };
struct Path cwd  = {};
char* host = NULL;
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
void init_shell_state(){
    config_path = strdup(CONFIG_PATH);
    init_path   = strdup(INIT_PATH);
    hot_path    = strdup(HOT_PATH);

    host = malloc(256);
    gethostname(host, 256);

    uid_t uid = getuid();
    // no getpwuid_r cuz it is ez and I (hopefully) just need a single thread
    struct passwd* p = getpwuid(uid);
    user.name = strdup(p->pw_name);
    user.home = parse_path(p->pw_dir);

    const char* _cwd = getenv("PWD");
    cwd = parse_path(_cwd);
}

/**
 * updates state where it is needed
 */
void get_current_state(){ }

/**
 * cleanins the shell state
 */
void end_shell_state(){
    destruct_path(&cwd);
    free(host);

    free(user.name);
    destruct_path(&user.home);

    free(config_path);
    free(init_path);
    free(hot_path);
}
