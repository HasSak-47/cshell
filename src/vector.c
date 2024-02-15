#include <stdio.h>

#include <alloc.h>
#include <defs.h>
#include <vector.h>

void __push(struct __vector * v, void* data, usize size_of, struct allocator a){
	void* aux = a.r(v->ptr, (v->len + 1) * size_of);
	if(aux == NULL)
		return;
	v->ptr = aux;

	size_t beg = v->len * size_of;
	for(size_t i = 0; i < size_of; ++i)
		((uint8_t*)v->ptr)[beg + i] = ((uint8_t*)data)[i];

	v->len++;
}

void __pop(struct __vector * v, size_t len, struct allocator a){
	if(v->len == 0){
		a.d(v->ptr);
		return;
	}
	v->len--;
}
