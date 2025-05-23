#include <stdio.h>

#include <alloc.h>
#include <defs.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

/**
 * @param {void**} vector: It contains a pointer to the vector info 
 */
void __vector_push(void** vector, usize size_of, struct allocator a){
	// get header or
	// setup header if it is a null vector
	struct vector_header* header;
	if(*vector == NULL)
		header = a.c(1, __SIZE_H);
	else
		header = *vector - __SIZE_H;


	// there is no need to resize the vector if the capacity is bigger than the current size
	if(header->len < header->cap){
		header->len++;
		return;
	}

	// set new cap to double (prev cap + 1)
	size_t new_cap= (header->cap + 1) << 1;
	void* aux = a.r(header, __SIZE_H + new_cap * size_of);

	// something went wrong
	if(aux == NULL)
		return;

	// move the *vector pointer to the pos of header addrs + header size
	*vector = aux + __SIZE_H;
	header = aux;
	header->cap = new_cap;
	header->len++;
}

void __vector_pop(void** vector, size_t size_of, struct allocator a){
	//get header
	struct vector_header* header = (*vector) - __SIZE_H;

	// delete vector if empty
	if(header->len == 1){
		a.d(header);
		*vector = NULL;
		return;
	}

	// do not shrink cap just remove 1
	header->len--;

}

void __vector_delete(void **vector, usize size_of, struct allocator a){
	//get header
	struct vector_header* header = (*vector) - __SIZE_H;
	a.d(header);
	*vector = NULL;
}
