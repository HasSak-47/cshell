#include <stdio.h>
#include <unistd.h>
#define SHELL_CMDS
#include <command.h>

const char* get_name(){
	return "cd";
}

int run(char vectored** params) {
	printf("chiange!\n");
	if(v_len(params) < 2){
		printf("no commands provided\n");
		return -1;
	}
	return chdir(params[0]);
}

