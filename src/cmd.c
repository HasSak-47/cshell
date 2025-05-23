#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#include "cmd.h"

struct Hot* hot = NULL;
struct Command* api = NULL;
struct Command* path = NULL;

size_t chot  = 0;
size_t capi  = 0;
size_t cpath = 0;
#define ALLOCATED

char* ALLOCATED get_name(const char* name){
	char* target = malloc(1024);
	strcpy(target, "utils/"); 
	char* r  = strcat(target, name);
	if(r == NULL){
		free(target);
		return NULL;
	}
	return target;
}

void load_cmds(){
	DIR* dir = opendir("./utils");
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

		struct Command* c = (struct Command*)dlsym("cmd", RTLD_DEFAULT);
		if(c != NULL){
			printf("cmd position %p\n", c);
			hot[count].cmd = *c;
		}
		else{
			printf("could not find command in .so\n");
		}
		count++;
	}
	chot = count;
}

void unload_cmds(){
	for(size_t i = 0; i < chot; ++i){
		if(hot[i].handle != NULL)
			dlclose(hot[i].handle);
	}
	free(hot);
}
