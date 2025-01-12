#ifndef __CMDS_H__
#define __CMDS_H__

#include <lauxlib.h>
#include <lua.h>

#ifdef HOT
extern struct luaL_Reg api_cmds[];
#else
extern struct luaL_Reg* api_cmds;
#endif

#endif // !__CMDS_H__
