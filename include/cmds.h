#ifndef __CMDS_H__
#define __CMDS_H__

#include <lua.h>

int cmd_exit(lua_State* L);
int cmd_cd(lua_State* L);
int cmd_reload(lua_State* L);

#endif // !__CMDS_H__
