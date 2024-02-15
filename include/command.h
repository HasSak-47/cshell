#ifndef __RUN_H__
#define __RUN_H__
#include <stdbool.h>

typedef int command_result;

#ifndef SHELL_CMDS

int run(char* cmd, char** params);
void load_path_names();
void unload_path_names();

void load_so_names();
void unload_so_names();

#else

typedef command_result (*__command)(char**);
void init_shell_cmd(char* name, __command c);

#endif


#endif
