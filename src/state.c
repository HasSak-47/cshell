#include <state.h>

#include <pwd.h>

#include <stdlib.h>
#include <string.h>

#include <unistd.h>


bool reload  = false;
bool running = true;
lua_State* L = NULL;

// Luall.vars.config
char* config_path = NULL;
char* init_path = NULL;
char* hot_path = NULL;

// Luall.vars
struct User user = { NULL, NULL };
char* cwd = NULL;
char* host = NULL;
int error = 0;
bool debug = true;

// TODO:
// Luall.vars.env


/**
 * sets the state of the shell
 */
void init_shell_state(){
    config_path = strdup(CONFIG_PATH);
    init_path   = strdup(INIT_PATH);
    hot_path    = strdup(HOT_PATH);
}

/**
 * gets the current state of the shell
 * like the cwd, host, and user
 */
void get_current_state(){
    if(cwd != NULL)
        free(cwd);
    cwd = getcwd(NULL, 0);

    if(host != NULL)
        free(host);
    host = malloc(256);
    gethostname(host, 256);

    uid_t uid = getuid();
    if(user.home != NULL)
        free(user.home);
    if(user.name != NULL)
        free(user.name);

    // no getpwuid_r cuz it is ez and I (hopefully) just need a single thread
    struct passwd* p = getpwuid(uid);
    user.home = strdup(p->pw_dir);
    user.name = strdup(p->pw_name);
}

/**
 * cleaning :)
 */
void end_shell_state(){
    free(config_path);
    free(init_path);
    free(hot_path);
}
