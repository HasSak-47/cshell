
#include <command.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

#include <utils.h>
#include <alloc.h>

struct dirent* path_programs = NULL;
size_t path_programs_len = 0;

typedef int (*command)(char**);

struct shell_cmd{
	const char* name;
	command c;
};

int shell_cd(char** args){
	if(args[1] == NULL)
		return -1;
	return chdir(args[1]);
}

const struct shell_cmd shell_cmds[] = {{"cd", shell_cd}};
const size_t shell_cmds_len = sizeof(shell_cmds) / sizeof(shell_cmds[0]);

void load_path_names(){}
void unload_path_names(){
}

