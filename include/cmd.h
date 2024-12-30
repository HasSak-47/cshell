#ifndef __LY_CMD_H__
#define __LY_CMD_H__


#include "./str.h"

typedef int (*Run)(const size_t, const struct String*);

struct Command{
	Run run;
	struct String name;
};

struct Hot{
	struct Command cmd;
	void* handle;
};


#ifdef COMMAND_DEFINITION

int on_load();
int run(const size_t argc, const struct String* args);
int on_unload();

#else

extern struct Hot* hot;
extern struct Command* api;
extern struct Command* path;

extern size_t chot;
extern size_t capi;
extern size_t cpath;

void load_cmds();
int run_cmd(const struct String name, const size_t argc, const struct String* args);
void unload_cmds();

#endif


#endif
