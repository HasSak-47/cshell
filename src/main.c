#include <stdlib.h>
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


int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
	atexit(__mem_debug_end);
#endif
	load_commands();
	char* ls_args[] = {"--color"};
	char* cd_args[] = {".."};

	run("ls", ls_args);
	run("cd", cd_args);
	run("ls", ls_args);
	unload_commands();
	return 0;
}
