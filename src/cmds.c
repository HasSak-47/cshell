#include <shell.h>
#include <cmds.h>
#include <stdio.h>

int cmd_exit(lua_State* L) {
    run = false;
    return 0;
}

int cmd_cd(lua_State* L) {
    printf("not implemented lmao\n");
    return 0;
}

int cmd_reload(lua_State* L) {
    printf("not implemented lmao\n");
    return 0;
}
