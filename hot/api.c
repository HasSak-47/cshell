#include <process.h>
#include <state.h>
#include <utils.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

static int api_exit(lua_State* L){
    running = false;
    return 0;
}

// should not exists
// it should be just as easy as mod the variable
// Luall.vars.debug
static int api_set_debug(lua_State* L){
    debug = true;
    return 0;
}

static int api_unset_debug(lua_State* L){
    debug = false;
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

static int api_exists(lua_State* L){
    const char* path = lua_tostring(L, -1);
    struct stat statm;
    if(stat(path, &statm) == 0)
        lua_pushboolean(L, true);
    else
        lua_pushboolean(L, false);
    return 1;
}

/*
 * Executes a regular binary
 * In it's lua form it takes a valid path and `n` arguments
 * set's it to the foreground and awaits it's termination
 */
static int api_exec(lua_State* L){
    const size_t argc = lua_gettop(L);
    const char* path = lua_tostring(L, 1);
    if (path == NULL) {
        error = -1;
        printf("not a valid path");
        return 0;
    }
    struct Command p = new_command(path);
    command_reserve_size(&p, argc);

    // last arg must be null
    p.args[argc] = NULL;
    
    for(size_t i = 0; i < argc; ++i){
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if(arg == NULL)
            continue;
        p.args[i] = strdup(arg);
    }

    // so it takes over
    p.foreground = true;
    // if the cmd does not exists just don't execute it
    struct stat statbuf;
    if(stat(p.cmd, &statbuf) == 0){
        pid_t child_pid = run(&p);
        waitpid(child_pid, &error, 0);

        if (debug) {
            printf("process ended with: %d\n", error);
        }
        set_to_foreground();
        set_raw_mode();
    }
    else{
        error = -1;
        printf("file not found!\n");
    }

    return 0;
}

/*
 * Creates a process that will be executed in the future
 * Just like Luall.api.exec it takes a valid path and `n` arguments
 */
static int api_process_new(lua_State* L){
    // create command
    const size_t argc = lua_gettop(L);
    const char* path = lua_tostring(L, 1);
    if (path == NULL) {
        error = -1;
        printf("not a valid path");
        return 0;
    }
    struct Command* p = lua_newuserdata(L, sizeof(struct Command));
    *p = new_command(path);
    command_reserve_size(p, argc);

    // last arg must be null
    p->args[argc] = NULL;
    
    for(size_t i = 0; i < argc; ++i){
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if(arg == NULL)
            continue;
        p->args[i] = strdup(arg);
    }

    return 1;
}

/*
 * Creates a new pipe
 */
static int api_process_bind_pipe(lua_State* L){
    // create command
    struct Command* cmd = lua_touserdata(L, 1);
    struct Pipe* pipe = lua_touserdata(L, 2);
    const char* action = lua_tostring(L, 3);

    enum BindType ty = NoneBind;
    if (strcmp(action, "out")) {
        ty |= WriteBind;
    }
    if (strcmp(action, "in")) {
        ty |= ReadBind;
    }
    if (strcmp(action, "err")) {
        ty |= ErrorBind;
    }

    bind_pipe(cmd, pipe, ty);

    return 0;
}

/*
 * Creates a new pipe
 */
static int api_pipe_new(lua_State* L){
    // create command
    struct Pipe* p = lua_newuserdata(L, sizeof(struct Pipe));
    *p = new_pipe();

    return 1;
}


#define REG(name) { #name, api_##name }

luaL_Reg api[] = {
    REG(cd),
    REG(set_debug),
    REG(unset_debug),
    REG(exit),
    REG(reload),
    REG(exec),
    REG(exists),
    {NULL, NULL},
};

