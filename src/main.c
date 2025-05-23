#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __DEBUG

#include <vector.h>
#include <defs.h>
#include <alloc.h>
#include <command.h>
#include <tokenizer.h>

#include <stdbool.h>

#include <pwd.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

struct token vectored* get_args(){
	char c = 0;
	char vectored* v = NULL;
	while((c = getchar()) != '\n')
		v_push(v, c);
	v_push(v, 0);

	struct token vectored* tokens = make_tokens(v);
	return tokens;
}

char cwd_buf[1024] = {};
char usr_buf[1024] = {};
char hos_buf[1024] = {};

char* cwd_cleaned(){
	int user_id = getuid();
	struct passwd* usr = getpwuid(user_id);
	char buf[1024] = {};
	getcwd(cwd_buf, 1024);
	size_t len_pwdir = strlen(usr->pw_dir);
	char* r = cwd_buf;
	if(strncmp(cwd_buf, usr->pw_dir, len_pwdir - 1) == 0){
		cwd_buf[len_pwdir - 1] = '~';
		r = r + len_pwdir - 1;
	}
	gethostname(hos_buf, 1024);
	strcpy(usr_buf, usr->pw_name);

	return r;
}

void prompt();
void handle_input();

/**
 * this contains the minimal necessary functions
 */
int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
	atexit(__mem_debug_end);
#endif
	load_commands();


	atexit(unload_commands);
	return 0;
}
