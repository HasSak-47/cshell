#include <stdio.h>

#include <alloc.h>
#include <defs.h>
#include <string.h>
#include <vector.h>


/**
 * @param {void**} vector: It contains a pointer to the vector info 
 */
void __vector_push(void** vector, usize size_of, struct allocator a){
	// setup header if it is a null vector
	if(*vector == NULL){
		*vector = a.c(__SIZE_H, 1);
		*vector += __SIZE_H;
	}

	//get header
	struct vector_header* header = (*vector) - __SIZE_H;
	void* aux = a.r(header, __SIZE_H + size_of * (header->len + 1));
	if(aux == NULL)
		return;

	*vector = aux + __SIZE_H;
	header = aux;
	header->len++;
}

void __vector_pop(void** vector, size_t size_of, struct allocator a){
	//get header
	struct vector_header* header = (*vector) - __SIZE_H;
	if(header->len == 1){
		a.d(header);
		*vector = NULL;
		return;
	}

	size_t new_size = (header->len - 1) * size_of + __SIZE_H;
	void* aux = a.r(header, new_size);
	if(aux == NULL)
		return;


	*vector = aux + __SIZE_H;
	header = aux;
	header->len--;

}
