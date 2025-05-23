#include <stdio.h>
#include <unistd.h>

#define SHELL_CMDS
#include <command.h>

const char* get_name(){
	return "cd";
}

int run(char vectored** params) {
	if(v_len(params) < 2){
		return -1;
	}

	int exit = chdir(params[0]);

	if(exit != 0)
		printf("could not change to %s", params[0]);

	return exit;
}

