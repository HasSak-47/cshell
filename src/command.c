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

struct dirent* bin_programs = NULL;
size_t bin_programs_len = 0;

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

void load_bin_names(){
	DIR* bin_dir = opendir("/bin");
	while(bin_dir){
		struct dirent* de = readdir(bin_dir);
		if(!de)
			break;
		if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".") == 0)
			continue;
		bin_programs_len++;
	}
	rewinddir(bin_dir);

	size_t index = 0;
	bin_programs = stdalloc.c(sizeof(struct dirent), bin_programs_len);
	struct dirent* de;
	while((de = readdir(bin_dir)) != NULL){
		if(strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".") == 0)
			continue;

		// gives an error otherwise :)
		size_t name_len = strlen(de->d_name);
		for(size_t i = 0; i < name_len; ++i){
			bin_programs[index].d_name[i] = de->d_name[i];
		}
		bin_programs[index].d_type = de->d_type;
		bin_programs[index].d_ino= de->d_ino;
		bin_programs[index].d_off= de->d_off;
		bin_programs[index].d_reclen= de->d_reclen;
		index++;
	}

	closedir(bin_dir);
	
}

void unload_bin_names(){
	stdalloc.d(bin_programs);
}

void print_line(char* line, size_t len){
	for(size_t i = 0; i < len; ++i){
		switch(line[i]){
		case -1:
		case 0:
			printf("[%d]", line[i]);
			break;
		default:
			printf("%c", line[i]);
			break;
		}
	}
	printf("\n");
}

static bool is_shell_command(char* argv){
	if(strcmp(argv, "exit") == 0)
		exit(-1);
	size_t i = 0;
	for(i = 0; i < shell_cmds_len; ++i)
		if(strcmp(argv, shell_cmds[i].name) == 0)
			return true;
	return false;
}

int run_shell_command(char** args){
	size_t i = 0;
	for(i = 0; i < shell_cmds_len; ++i)
		if(strcmp(args[0], shell_cmds[i].name) == 0)
			return chdir(args[1]);
	return -1;
}

int run_bin_program(char** args){
	bool found = false;
	for(size_t i = 0; i < bin_programs_len; ++i){
		if(strcmp(args[0], bin_programs[i].d_name) == 0){
			found = true;
			break;
		}
	}
	if(!found){
		printf("command not found!\n");
		return -1;
	}

	size_t name_len = strlen(args[0]) + 10;
	char* name = stdalloc.c(sizeof(char), name_len);
	strcat(name, "/bin/");
	strcat(name, args[0]);

	// char* aux = args[0];
	args[0] = name;

	int status = 0;
	pid_t pid = fork();
	if (pid == 0) { // child
		if(execv(name, args) -1){
			return -1;
		}
	}
	else{ // parent
		waitpid(pid, &status, 0);
	}

	stdalloc.d(name);

	return status;
}

int run(char* __line){
	char* line = __line;
	while(*line == ' ') line++;
	size_t line_len = strlen(line);
	bool inside_quotes = false;
	bool repeated = false;

	size_t words = 0;
	for(size_t i = 0; i < line_len; ++i){
		if(line[i] == '"') inside_quotes = !inside_quotes;
		if(inside_quotes) continue;

		if(line[i] == ' '){
			line[i] = 0;
			if(repeated) line[i] = -1;
			else words++;
			repeated = true;
			continue;
		}

		repeated = false;
	}
	for(size_t index = 0; index < line_len; ++index){
		if(line[index] == -1){
			size_t end;
			for(end = index; end < line_len && line[end] == -1; ++end);
			if(line[index] == line[end]) break;

			char aux = line[index];
			line[index] = line[end];
			line[end] = aux;
		}
	}

	char** args = stdalloc.c(sizeof(char*), words + 2);
	size_t new_len = 0;
	size_t args_index = 1;
	args[0] = line;
	for(size_t i = 0; i < line_len && args_index <= words; ++i){
		if(line[i] == 0){
			args[args_index++] = &line[i + 1];	
		}
		else if(line[i] == -1){
			new_len = i - 1;
			break;
		}
	}

	int st = 0;
	if(is_shell_command(args[0]))
		st = run_shell_command(args);
	else
		st = run_bin_program(args);

	stdalloc.d(args);
	return st;
}
