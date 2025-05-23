#include <defs.h>
#include <alloc.h>
#include <command.h>
#include <tokenizer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pwd.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

typedef const char* const cchar;

char* read_line(){
	char* buffer = (char*)stdalloc.a(sizeof(char));
	size_t size = 0;
	size_t esize = 1;
	char c;
	while((c = getchar()) != '\n'){
		buffer[size++] = c;
		if(size == esize){
			esize *= 2;
			void* aux = realloc(buffer, esize);
			if(aux == NULL) exit(EXIT_FAILURE);
			buffer = aux;

		}
	}

	void* aux = realloc(buffer, size + 1);
	if(aux == NULL) exit(EXIT_FAILURE);
	buffer = aux;
	buffer[size] = 0;

	return buffer;
}

#define __DEBUG

int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
#endif
	load_bin_names();

	struct passwd* pwd = getpwuid(getuid());
	cchar user = pwd->pw_name;
	cchar home = pwd->pw_dir;
	size_t home_size = strlen(home);
	int ret = 0;
	int status = 0;

	while(true){
		char _cwd[255] = {};
		getcwd(_cwd, 255);

		bool is_home = strncmp(_cwd, home, home_size) == 0;
		if(is_home){
			_cwd[home_size - 1] = '~';
		}

		char priv = strncmp("root", user, 4) == 0 ? '#' : '$';

		cchar cwd = _cwd + (home_size - 1) * is_home;
		char _host[255] = {};
		gethostname(_host, 255);
		cchar host = _host;

		printf("[%s@%s %s]%c [%d] ", user, host, cwd, priv, ret);
		char* line = read_line();

		// this shit is wack
		if(strcmp(line, "exit") == 0){
            printf("exiting...\n");
			status = -1;
			stdalloc.d(line);
			goto END;
		}

		ret = run(line);
		stdalloc.d(line);
	}

END:
    printf("cleaning...\n");
	unload_bin_names();
#ifdef __DEBUG_MEM
    __mem_debug_end();
#endif
	return 0;
}
