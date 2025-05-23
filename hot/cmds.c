#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <shell.h>
#include <cmds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>


static int cmd_exit(lua_State* L) {
    run = false;
    return 0;
}

static int cmd_cd(lua_State* L) {
    printf("not implemented lmao\n");
    return 0;
}

static int cmd_reload(lua_State *L){
    reload = true;
    return 0;
}

static int cmd_lua_info(lua_State *L){
    size_t c = lua_gettop(L);
    printf("there are %lu values in the stack\n", c);
    return 0;
}

static int cmd_execv(lua_State *L){
    size_t argc = lua_gettop(L);
    const char** args = malloc(argc - 1);
    /*
    char* buf = malloc(strlen(str));
    strcpy(buf, str);
    pid_t pid = fork();
    if(pid == 0){ // child
        execv(buf, NULL);
        free(buf);
    }
    else{ // parent
        waitpid(pid, &last_return_code, 0);
    }
    */
    
    return 1;
}

struct luaL_Reg api_cmds[] = {
    {"exit", cmd_exit},
    {"cd", cmd_cd},
    {"reload", cmd_reload},
    {"execv", cmd_execv},
    {"lua_info", cmd_lua_info},

    {NULL, NULL}
};
