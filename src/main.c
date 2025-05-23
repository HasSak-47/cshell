#define __DEBUG

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


int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
#endif
	i32 vectored* vector = NULL;
	for(size_t i = 0; i < 16; ++i){
		v_push(vector, i);
	}
	for(size_t i = 16; i < 32; ++i){
		v_insert(vector, i, 0);
	}
	for(size_t i = 8; i < 16; ++i){
		v_remove(vector, 8);
	}
	for(size_t i = 8; i < v_len(vector); ++i)
		printf("%d\n", vector[i]);

	while(vector != NULL){
		v_pop(vector);
	}
END:
#ifdef __DEBUG_MEM
    __mem_debug_end();
#endif
	return 0;
}
