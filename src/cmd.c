#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#include "cmd.h"

static int reload(int _argc, char **_argv){
	reload_cmd();
	return 0;
}

static struct Command _reload_cmd = {
	reload, {6, "reload"},
};

struct Hot* hot = NULL;
struct Command* api = {
};
struct Command* path = NULL;

size_t chot  = 0;
size_t capi  = 1;
size_t cpath = 0;
#define ALLOCATED

void load_cmds(){
	DIR* dir = opendir("./units");
	struct dirent* entry; 
	size_t count = 0;
	while( (entry = readdir(dir)) != NULL)
		count++;

	rewinddir(dir);
	hot = malloc(sizeof(struct Hot) * count);
	count = 0;
	while( (entry = readdir(dir)) != NULL){
		if(strcmp(entry->d_name, ".") == 0)
			continue;
		if(strcmp(entry->d_name, "..") == 0)
			continue;

		char* path = get_name(entry->d_name);
		void * handle = dlopen(path, RTLD_LAZY);
		printf("loading cmd: %s\n", path);
		free(path);

		if(handle == NULL){
			printf("could not get handle dlerror: %s\n", dlerror());
			continue;
		}
		hot[count].handle = handle;

		struct Command* c = (struct Command*)dlsym(handle, "cmd");
		if(c != NULL){
			hot[count].cmd = *c;
		}
		else{
			printf("could not find command in .so\n");
		}
		count++;
	}
	chot = count;
	closedir(dir);
}

void reload_cmd(){
	unload_cmds();
	load_cmds();
}

void unload_cmds(){
	for(size_t i = 0; i < chot; ++i){
		if(hot[i].handle != NULL)
			dlclose(hot[i].handle);
	}
	free(hot);
}
