#include <stdio.h>
#include <unistd.h>
#define SHELL_CMDS
#include <command.h>

const char* get_name(){
	return "tests2";
}

int run(char vectored**params) {
	printf("test again again!!\n");
	size_t total = v_len(params);
	for(size_t i = 0; i < total; ++i){
		printf("%lu) %s\n", i, params[i] ? params[i] : "NULL");
	}

	return 0;
}

