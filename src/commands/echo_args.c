#include <stdio.h>
#define COMMAND_DEFINITION
#include <cmd.h>

int echo_args(int argc, char** argv){
	for(size_t i = 0; i < argc; ++i){
		printf("%s\n", argv[i]);
	}
	return 0;
}
const struct String name = {
	9,
	"echo_args"
};

const struct Command cmd = {
	echo_args,
	name,
};


