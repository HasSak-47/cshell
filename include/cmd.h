#ifndef __LY_CMD_H__
#define __LY_CMD_H__


#include "./str.h"

typedef int (*Run)(int, char**);

struct Command{
	Run run;
	struct String name;
};

struct Hot{
	struct Command cmd;
	void* handle;
};


#ifdef COMMAND_DEFINITION
const struct Command cmd;
#else

extern struct Hot* hot;
extern struct Command* api;
extern struct Command* path;

extern size_t chot;
extern size_t capi;
extern size_t cpath;

void load_cmds();
void reload_cmd();
void unload_cmds();

#endif


#endif
