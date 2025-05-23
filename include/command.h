#ifndef __RUN_H__
#define __RUN_H__
#include <stdbool.h>

int run(char* line);
void load_bin_names();
void unload_bin_names();

typedef int (*__command)(char**);

void init_shell_cmd(char* name, __command c);

#endif
