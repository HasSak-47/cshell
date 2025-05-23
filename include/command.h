#ifndef __RUN_H__
#define __RUN_H__
#include <stdbool.h>
#include "vector.h"

typedef int (*lib_command)(char**);
typedef const char* (*lib_get_name)();

#ifndef SHELL_CMDS

int run_cmd(char* cmd, char vectored** params);

void load_commands();
void unload_commands();

#else

// this is for the so files to define stuff
int run(char vectored** params);
const char* get_name();

#endif


#endif
