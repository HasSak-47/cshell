#include <stdio.h>

#include <alloc.h>
#include <defs.h>
#include <string.h>
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

#define get_byte(v, i) (*(u8*)&v->ptr[i])
#define get_addr(v, i) (u8*)&v->ptr[i]

void __insert(struct __vector* v, void* data, size_t pos, size_t size_of, struct allocator a) {
	size_t new_len = (v->len + 1) * size_of;
	size_t beg = pos * size_of;
	size_t end = (pos + 1) * size_of;
	void* aux = a.r(v->ptr, new_len);
	if(aux == NULL)
		return;
	v->ptr = aux;
	v->len++;

	u8* buffer_a = (u8*) a.a(size_of);
	u8* buffer_b = (u8*) a.a(size_of);
	memcpy(buffer_a, data, size_of);
	for(size_t i = beg; i < new_len; i += size_of){
		memcpy(buffer_b, v->ptr + i, size_of);
		memcpy(v->ptr + i, buffer_a, size_of);
		memcpy(buffer_a, buffer_b, size_of);
	}

	a.d(buffer_a);
	a.d(buffer_b);
}

void __remove(struct __vector* v, size_t pos, size_t size_of, struct allocator a) {
	if(v->len == 1){
		a.d(v->ptr);
		v->ptr = NULL;
		v->len = 0;
		return;
	}

	size_t beg = pos * size_of;
	size_t new_len = (v->len - 1) * size_of;
	for(size_t i = beg; i < new_len; i += size_of){
		memcpy(v->ptr + i, v->ptr + i + size_of, size_of);
	}
	v->len--;
}

void __resize(struct __vector* v, size_t len, size_t size_of, struct allocator a) {
	void* aux = a.r(v->ptr, size_of * len);
	if(aux == NULL)
		return;

	v->ptr = aux;
	v->len = len;
}
