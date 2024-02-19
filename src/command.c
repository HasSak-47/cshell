/*
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


struct so_cmd{
	char name[512];
	char path[512]; void* lib;
};

MAKE_STRUCT_VECTOR_NAMED(dirent, struct dirent);
MAKE_STRUCT_VECTOR_NAMED(so_cmd, struct so_cmd);
struct vector_dirent path_execs = {};
struct vector_so_cmd so_execs = {};


void load_path_names(){}
void unload_path_names(){}

void load_so_names(){
	DIR* so_dir = opendir("utils");

	struct dirent* entry = NULL;
	while((entry = readdir(so_dir)) != NULL){
		// clean obv dirs
		if(strcmp(entry->d_name, ".") == 0)
			continue;
		if(strcmp(entry->d_name, "..") == 0)
			continue;
		
		struct so_cmd cmd = {};
		strcpy(cmd.path, "utils/");
		strcat(cmd.path, entry->d_name);
		cmd.lib = dlopen(cmd.path, RTLD_LAZY);
		lib_get_name name = dlsym(cmd.lib, "get_name");
		strcpy(cmd.name, name());
		v_push(so_execs, cmd);
	}

	closedir(so_dir);
}

void unload_so_names(){
	for(size_t i = 0; i < so_execs.len; ++i)
		dlclose(so_execs.ptr[i].lib);

	cur_alloc.d(so_execs.ptr);
}

int __run_so(struct so_cmd* so_cmd, char** params){
	lib_command run_cmd = dlsym(so_cmd->lib, "run");
	printf("executing: %s\n", so_cmd->name);
	return run_cmd(params);
}

int run(char* name, char **params){
	for(size_t i = 0; i < so_execs.len; ++i)
		if(strcmp(so_execs.ptr[i].name, name) == 0)
			return __run_so(&so_execs.ptr[i], params);
	return -1;
}
*/
