#include <defs.h>
#include <command.h>
#include <utils.h>
#include <alloc.h>
#include <vector.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

struct command{
	char name[256];
	char path[256];
	enum command_type{
		CMD_T_EXE,
		CMD_T_LIB,
	} type;
	union runable{
		void* library;
	} run;
};

struct command vectored* commands = NULL;

static int __run_so(void* lib, char* args[]){
	lib_command cmd = dlsym(lib, "run");
	return cmd(args);
}

static int __run_ex(char* path, char* args[]){
	printf("running %s\n", path);
	return -1;
}

static int __run(struct command* so_cmd, char** args){
	switch(so_cmd->type){
		CMD_T_EXE:
			return __run_so(so_cmd->run.library, args);
		break;
		CMD_T_LIB:
			return __run_ex(so_cmd->path, args);
		break;
		default:
		break;
	}
	return -1;
}

static struct command vectored* __get_commands(const char* path){
	DIR* dir = opendir(path);
	struct dirent* buffer = NULL;
	struct command vectored* cmds = NULL;

	while( (buffer = readdir(dir)) != NULL ){
		if(
			strcmp(buffer->d_name, ".") == 0 || 
			strcmp(buffer->d_name, "..") == 0
		)
			continue;

		struct command new_cmd = {};
		strcpy(new_cmd.path, path);
		strcat(new_cmd.path, buffer->d_name);
		strcpy(new_cmd.name, buffer->d_name);

		v_push(cmds, new_cmd);
	}
	closedir(dir);
	return cmds;
}

#define UTILS_DIR "utils/"

void load_commands(){
	struct command vectored* buffer = __get_commands("/bin/");
	size_t len = v_len(buffer);
	for(size_t i = 0; i < len; ++i){
		buffer[i].type = CMD_T_EXE;
		v_push(commands, buffer[i]);
	}
	v_delete(buffer);

	buffer = __get_commands(UTILS_DIR);
	len = v_len(buffer);
	for(size_t i = 0; i < len; ++i){
		buffer[i].type = CMD_T_LIB;
		buffer[i].run.library = dlopen(buffer[i].path, RTLD_LAZY);
		lib_get_name name = dlsym(buffer[i].run.library, "get_name");
		strcpy(buffer[i].name, name());
		v_push(commands, buffer[i]);
	}
	v_delete(buffer);
}

void unload_commands(){
	for(size_t i = 0; i < v_len(commands); ++i){
		if(commands[i].type == CMD_T_LIB)
			dlclose(commands[i].run.library);
	}

	v_delete(commands);
}

int run(char* name, char *params[]){
	for(size_t i = 0; i < v_len(commands); ++i){
		if(strcmp(commands[i].name, name) == 0)
			return __run(&commands[i], params);
	}
	return -1;
}
