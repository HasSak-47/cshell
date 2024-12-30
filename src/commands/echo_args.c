#include <stdio.h>
#define COMMAND_DEFINITION
#include <cmd.h>

int echo_args(const size_t argc, const struct String* argv){
	for(size_t i = 0; i < argc; ++i){
		printf("%*s\n", (int)argv[i].len, argv[i].cstr);
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


