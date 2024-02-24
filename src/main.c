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

int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
	atexit(__mem_debug_end);
#endif
	load_commands();
	int r = 0;
	while(true){
		printf("[%d]:", r);
		char** args = split_into_args(get_args());
		if(strcmp(args[0], "exit") == 0){
			for(size_t i = 0; i < v_len(args) - 1; ++i)
				v_delete(args[i]);
			v_delete(args);
			break;
		}

		char* name = args[0];
		v_remove(args, 0);
		int r = run_cmd(name, args);
		v_delete(args);
		for(size_t i = 0; i < v_len(args); ++i)
			v_delete(args[i]);
		v_delete(args);

	}
	unload_commands();
	return 0;
}
