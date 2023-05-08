#include <vector.h>
#include <stdint.h>
#include <stdlib.h>

void __push(void_vector *vector, void *data, size_t len){
	void* aux = realloc(vector->ptr, (vector->size + 1) * len);
	if(aux == NULL)
		exit(EXIT_FAILURE);

	vector->ptr = aux;
	vector->size++;
}

void __pop(void_vector *vector, void *buf, size_t len){
	if(vector->size == 0)
		return;

	for(size_t i = 0; i < len; ++i)
		((uint8_t*)buf)[i] = ((uint8_t*)vector->ptr)[(vector->size - 1) * len];

	void* aux = realloc(vector->ptr, (vector->size - 1) * len);
	if(aux == NULL)
		exit(EXIT_FAILURE);

	vector->ptr = aux;
	vector->size--;

}
