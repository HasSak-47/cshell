#include <state.h>

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

// NOTE: this should not be run with valgrind!!
static int api_exec(lua_State* L){
    const size_t argc = lua_gettop(L);
    char** args = malloc((argc + 1) * sizeof(char*));
    // args must be null terminated
    args[argc] = NULL;

    for(size_t i = 0; i < argc; ++i){
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if(arg == NULL)
            continue;
        args[i] = strdup(arg);
    }
    const char* cmd = args[0];

    // if the cmd does not exists just don't execute it
    struct stat statbuf;
    if(stat(cmd, &statbuf) == 0){
        pid_t pid = fork();
        if(pid == 0) { // child
            int error = execv(cmd, args);
            if(error == 1)
                exit(-1);
        }
        else if(pid > 0){ // parent
            waitpid(pid, &error, 0);
        }
        else { // not found
            printf("could not exec\n");
            error = -1;
        }
    }
    else{
        error = -1;
        printf("binary not found!\n");
    }

    for(size_t i = 0; i < argc; ++i)
        free(args[i]);
    free(args);

    return 0;
}

#define REG(name) { #name, api_##name }

luaL_Reg api[] = {
    REG(cd),
    REG(exit),
    REG(reload),
    REG(exec),
    REG(exists),
    {NULL, NULL},
};

