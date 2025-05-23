#include <stdio.h>
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

MAKE_STRUCT_VECTOR(i32);

#define __DEBUG

int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
#endif
	// struct vector_token tokens = make_tokens("this is a \"test string\" yahooo");

	struct vector_i32 v = {};
	for(size_t i = 0; i < 16; ++i)
		v_push(v, i);
	for(size_t i = 16; i < 32; ++i)
		v_insert(v, i, 0);

	for(size_t i = 0; i < v.len; ++i)
		printf("%2d ", v.ptr[i]);
	printf("\n");

	for(size_t i = 8; i < 16; ++i)
		v_remove(v, i);

	for(size_t i = 0; i < v.len; ++i)
		printf("%2d ", v.ptr[i]);
	printf("\n");

	del_alloc(v.ptr);
	
END:
#ifdef __DEBUG_MEM
    __mem_debug_end();
#endif
	return 0;
}
