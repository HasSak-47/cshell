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

typedef const char* const cchar;


#define __DEBUG

int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
#endif
	char test_str[] = "  this     is a    \"test string\" yahooo    \" wacky sttttt222\" lmaoo o o o ";
	struct vector_token tokens = make_tokens(test_str);

	del_alloc(tokens.ptr);
	
END:
#ifdef __DEBUG_MEM
    __mem_debug_end();
#endif
	return 0;
}
