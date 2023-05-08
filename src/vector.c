#include <vector.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void __push(void_vector *vector, void *data, size_t len){
	size_t index = vector->size;
	void* aux = realloc(vector->ptr, (vector->size + 1) * len);
	if(aux == NULL)
		exit(EXIT_FAILURE);

	vector->ptr = aux;
	vector->size++;

	for(size_t i = 0; i < len; ++i){
		uint8_t* t = (vector->ptr);
		t[index * len + i] = ((uint8_t*)data)[i];

	}

}

void __pop(void_vector *vector, void *buf, size_t len){
	if(vector->size == 0)
		return;

	size_t index = vector->size - 1;
	for(size_t i = 0; i < len; ++i)
		((uint8_t*)buf)[i] = ((uint8_t*)vector->ptr)[index * len + i];

	void* aux = realloc(vector->ptr, (vector->size - 1) * len);
	if(aux == NULL && vector->size > 1){
		printf("ran out of memory!");
		exit(EXIT_FAILURE);
	}

	vector->ptr = aux;
	vector->size--;

}
