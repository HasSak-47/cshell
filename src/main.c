#include "vector.h"
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

MAKE_STRUCT_VECTOR(i32);

int main(){
#ifdef __DEBUG_MEM
    __mem_debug_init();
#endif

	struct vector_i32 testv = {};
	for(i32 i = 0; i < 512; ++i)
		push(testv, i);
	free(testv.ptr);

END:
#ifdef __DEBUG_MEM
    __mem_debug_end();
#endif
	return 0;
}
