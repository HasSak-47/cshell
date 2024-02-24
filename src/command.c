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
	v_insert(args, path, 0);

	pid_t pid = fork();
	int status = 0;
	if(pid == 0) // child
		execv(path, args);
	else{
		waitpid(pid, &status, 0);
		v_remove(args, 0);
	}

	return status;
}

static int __run(struct command* so_cmd, char vectored** args){
	int return_code = -1;
	switch(so_cmd->type){
		case CMD_T_LIB:
			return_code = __run_so(so_cmd->run.library, args);
		break;
		case CMD_T_EXE:
			return_code = __run_ex(so_cmd->path, args);
		break;
		default:
		break;
	}

	return return_code;
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
	} v_delete(buffer);
}

void unload_commands(){
	for(size_t i = 0; i < v_len(commands); ++i){
		if(commands[i].type == CMD_T_LIB)
			dlclose(commands[i].run.library);
	}

	v_delete(commands);
}

void reload(){
	unload_commands();
	load_commands();
}

int run_cmd(char* name, char vectored* params[]){
	if(strcmp(name,"reload") == 0){
		reload();
		return 0;
	}
	for(size_t i = 0; i < v_len(commands); ++i){
		if(strcmp(commands[i].name, name) == 0)
			return __run(&commands[i], params);
	}
	printf("func not fund\n");
	return -1;
}
