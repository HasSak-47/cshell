#define _GNU_SOURCE
#include <unistd.h>

#include <pwd.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define MAX 256

#define FORE "\e[38;2;%d;%d;%d;m"
#define BACK "\e[42;2;%d;%d;%d;m"

struct ColorString{
	const char* const str;
	uint8_t r, g, b;
};



int main(){
	// setup
	struct passwd* user = getpwuid(getuid());
	char* username = user->pw_name;
	char* cwd = get_current_dir_name();
	char  host[512] = {};

	gethostname(host, 512);

	unsigned hd_size = strlen(user->pw_dir);

	//loop time
	bool in_home = strncmp(user->pw_dir, cwd, hd_size) == 0;

	if(in_home){
		cwd[hd_size - 1] = '~';
		printf("%s@%s %s", user->pw_name, host, cwd + hd_size - 1);
	}
	else
		printf("%s@%s %s", user->pw_name, host, cwd);

	char* buffer = NULL;
	size_t bsize = 0;
	char c = 0;
	while(c != EOF && c != '\n'){
		c = getchar();
		char* aux = realloc(buffer, bsize + 1);
		if(aux == NULL)
			return -1;

		buffer = aux;
		buffer[bsize++] = c;
	}

	printf("%s", buffer);

	free(cwd);
	return 0;
}
