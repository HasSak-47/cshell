#include <stdio.h>
#include <unistd.h>
#define SHELL_CMDS
#include <command.h>

const char* get_name(){
	return "cd";
}

int run(char **params) {
	if(params == NULL){
		printf("no commands provided");
		return -1;

	}
	return chdir(params[0]);
}

