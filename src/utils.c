#include "defs.h"
#include <stdio.h>
#include <utils.h>
#include <alloc.h>

void cat_realloc(void** c, size_t s){
	void* aux = cur_alloc.r(*c, s);
	if(aux == NULL){
		perror("ran out of memory!\n");
		exit(EXIT_FAILURE);
	}

	c = aux;
}

void hex_dump(void* data, size_t len){
	for(size_t i = 0; i < len; ++i){
		u8 chr = *(u8*)(data + i);
		printf("%02x", chr);
		if(i % 16 == 15)
			printf("\n");
	}
	if(len % 16 != 0)
		printf("\n");
}
