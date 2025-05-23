#include<utils.h>
#include<alloc.h>

void cat_realloc(void** c, size_t s){
	void* aux = stdalloc.r(*c, s);
	if(aux == NULL){
		perror("ran out of memory!\n");
		exit(EXIT_FAILURE);
	}

	c = aux;
}
