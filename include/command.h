#ifndef __RUN_H__
#define __RUN_H__
#include <stdbool.h>

typedef int (*lib_command)(char**);
typedef const char* (*lib_get_name)();

#ifndef SHELL_CMDS

int run(char* cmd, char** params);
void load_path_names();
void unload_path_names();

void load_so_names();
void unload_so_names();

#else

// this is for the so files to define stuff
int run(char** params);
const char* get_name();

#endif


#endif
