#include <process.h>
#include <state.h>
#include <utils.h>
#include "debug.h"
#include "path.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int api_exit(lua_State* L) {
    debug_printf("ending session...\n");
    running = false;
    return 0;
}

// should not exists
// it should be just as easy as mod the variable
// Luall.vars.debug
static int api_set_debug(lua_State* L) {
    debug = true;
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "debug");

    lua_pop(L, 2);
    return 0;
}

static int api_unset_debug(lua_State* L) {
    debug = false;
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "debug");

    lua_pop(L, 2);
    return 0;
}

static int api_reload(lua_State* L) {
    reload = true;
    return 0;
}

static int api_cd(lua_State* L) {
    const char* path = lua_tostring(L, -1);
    if (path == NULL) {
        return 0;
    }
    debug_printf("cd to %s\n", path);

    error = chdir(path);
    if (error != 0) {
        return 0;
    }

    struct Path c_path = parse_path(path);

    if (c_path._inner.data[0].ty == ROOT_PATH) {
        chdir(path);
        destruct_path(&cwd);
        cwd = c_path;
    }
    else {
        char* cc_path = get_path_string(c_path);
        debug_printf("c_path: %s\n", cc_path);
        free(cc_path);
        for (size_t i = 0; i < c_path._inner.len; ++i) {
            if (c_path._inner.data[i].ty == PREV_PATH) {
                debug_printf("prev!\n");
                pop_segment(&cwd);
            }
            else if (c_path._inner.data[i].ty == NAMED_PATH) {
                push_segment(&cwd, c_path._inner.data[i]);
            }
        }
        free(c_path._inner.data);
    }

    return 0;
}

static int api_exists(lua_State* L) {
    const char* path = lua_tostring(L, -1);
    struct stat statm;
    if (stat(path, &statm) == 0)
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
static int api_exec(lua_State* L) {
    const size_t argc = lua_gettop(L);
    if (debug) {
        printf("params:\n");
        for (size_t i = 1; i <= argc; ++i) {
            const char* arg = lua_tostring(L, i);
            printf("param %lu %s\n", i, arg);
        }
    }
    const char* path = lua_tostring(L, 1);
    if (path == NULL) {
        error = -1;
        printf("not a valid path");
        return 0;
    }
    struct Command p = new_command(path);
    command_reserve_size(&p, argc);

    if (debug) {
        printf("argc: %lu\n", argc);
    }
    // the first command was already there!
    for (size_t i = 1; i < argc; ++i) {
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if (arg == NULL)
            continue;

        if (debug) {
            printf("adding arg: %s\n", arg);
        }
        add_arg(&p, arg);
    }
    // so it takes over
    p.foreground = true;
    // if the cmd does not exists just don't execute it
    struct stat statbuf;
    if (stat(p.cmd, &statbuf) == 0) {
        pid_t child_pid = run(&p);
        waitpid(child_pid, &error, 0);

        if (debug) {
            printf("process ended with: %d\n", error);
        }
        set_to_foreground();
        set_raw_mode();
    }
    else {
        error = -1;
        printf("file not found!\n");
    }

    return 0;
}

/*
 * Creates a process that will be executed in the future
 * Just like Luall.api.exec it takes a valid path and `n` arguments
 */
static int api_process_new(lua_State* L) {
    // create command
    const size_t argc = lua_gettop(L);
    const char* path  = lua_tostring(L, 1);
    if (path == NULL) {
        error = -1;
        printf("not a valid path");
        return 0;
    }
    struct Command* p = lua_newuserdata(L, sizeof(struct Command));
    *p                = new_command(path);
    command_reserve_size(p, argc);

    for (size_t i = 1; i < argc; ++i) {
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if (arg == NULL)
            continue;
        add_arg(p, arg);
    }

    return 1;
}

/**
 * runs a process and returns it's pid
 */
static int api_process_run(lua_State* L) {
    struct Command* p = lua_touserdata(L, 1);
    pid_t pid         = run(p);
    lua_pushinteger(L, pid);

    return 1;
}

static int api_process_wait(lua_State* L) {
    pid_t p = lua_tointeger(L, 1);
    waitpid(p, &error, 0);

    return 1;
}

/*
 * Creates a new pipe
 */
static int api_process_bind_pipe(lua_State* L) {
    // create command
    struct Command* cmd = lua_touserdata(L, 1);
    struct Pipe* pipe   = lua_touserdata(L, 2);
    const char* action  = lua_tostring(L, 3);

    enum BindType ty = NoneBind;
    if (strcmp(action, "out") == 0) {
        ty |= WriteBind;
    }
    else if (strcmp(action, "in") == 0) {
        ty |= ReadBind;
    }
    else if (strcmp(action, "err") == 0) {
        ty |= ErrorBind;
    }

    bind_pipe(cmd, pipe, ty);

    return 0;
}

/*
 * Creates a new pipe
 */
static int api_pipe_new(lua_State* L) {
    // create command
    struct Pipe* p = lua_newuserdata(L, sizeof(struct Pipe));
    *p             = new_pipe();

    return 1;
}

/*
 * Creates a new pipe
 */
static int api_pipe_close(lua_State* L) {
    // create command
    struct Pipe* p = lua_touserdata(L, -1);
    close_pipe(p);

    return 1;
}

static void __print_param(lua_State* L, size_t i) {
    if (lua_isstring(L, i + 1)) {
        printf("%s", lua_tostring(L, i));
    }
    else if (lua_isboolean(L, i)) {
        printf("%s", lua_toboolean(L, i) ? "true" : "false");
    }
    else if (lua_isinteger(L, i)) {
        printf("%lld", lua_tointeger(L, i));
    }
    else if (lua_isnumber(L, i)) {
        printf("%f", lua_tonumber(L, i));
    }
    else {
        printf("<%s %p>", luaL_typename(L, i + 1), lua_topointer(L, i + 1));
    }
}

static int api_io_print(lua_State* L) {
    size_t params = lua_gettop(L);

    for (size_t i = 0; i < params - 1; ++i) {
        __print_param(L, i + 1);
        printf("\t");
    }
    __print_param(L, params - 1);
    return 0;
}

static int api_io_debug_print(lua_State* L) {
    if (debug)
        return 0;

    api_io_debug_print(L);

    return 0;
}

const luaL_Reg api_process[] = {
    {      "new",       api_process_new},
    {      "run",       api_process_run},
    {     "wait",      api_process_wait},
    {"bind_pipe", api_process_bind_pipe},
    {       NULL,                  NULL},
};

#define REG(name) {#name, api_##name}

luaL_Reg api[] = {
    REG(cd),
    REG(set_debug),
    REG(unset_debug),
    REG(exit),
    REG(reload),
    REG(exec),
    REG(exists),
    {"debug", api_io_debug_print},
    {"print",       api_io_print},

    {   NULL,               NULL},
};

const luaL_Reg api_pipe[] = {
    {  "new",   api_pipe_new},
    {"close", api_pipe_close},
    {   NULL,           NULL},
};

static void init_process_metatable(lua_State* L) {
    luaL_newmetatable(L, "Process");
    lua_pushcfunction(L, api_process_run);
    lua_setfield(L, -2, "run");

    lua_pushcfunction(L, api_process_wait);
    lua_setfield(L, -2, "wait");

    lua_pushcfunction(L, api_process_bind_pipe);
    lua_setfield(L, -2, "bind_pipe");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pop(L, 1);
}

static void init_pipe_metatable(lua_State* L) {
    luaL_newmetatable(L, "Pipe");

    lua_pushcfunction(L, api_pipe_close);
    lua_setfield(L, -2, "close");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

void init_api() {
    init_pipe_metatable(L);
    init_process_metatable(L);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "api");
    luaL_setfuncs(L, api, 0);

    lua_createtable(L, 0, 0);
    lua_pushcfunction(L, api_process_new);
    lua_setfield(L, -2, "new");
    luaL_getmetatable(L, "process");
    lua_setmetatable(L, -2);

    lua_setfield(L, -2, "process");

    lua_createtable(L, 0, 0);
    lua_pushcfunction(L, api_pipe_new);
    lua_setfield(L, -2, "new");
    luaL_getmetatable(L, "pipe");
    lua_setmetatable(L, -2);

    lua_setfield(L, -2, "pipe");
    lua_pop(L, 2);
}
