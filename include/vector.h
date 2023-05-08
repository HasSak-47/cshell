#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h>

typedef struct void_vector{
	void* ptr;
	size_t size;
}void_vector;

#define MAKE_VECTOR(type)\
typedef struct vector_##type{\
	type* ptr;\
	size_t size;\
} vector_##type;

void __push(void_vector* vector, void* data, size_t len);
void  __pop(void_vector* vector, void* buf,  size_t len);

#define push(vec, d) __push((void_vector*)&vec, &d, sizeof(vec.ptr[0]))
#define pop(vec, b)   __pop((void_vector*)&vec, &b, sizeof(vec.ptr[0]))

#define MAKE_PUSH(type)\
static inline void push_##type (vector_##type* vec, type data) { __push((void_vector*)vec, &data, sizeof(type)); }

#define DEF_VEC {NULL, 0}

#define MAKE_POP(type)\
static inline type pop_##type (vector_##type* vec) {  type buf; __pop((void_vector*)vec, &buf, sizeof(type)); return buf;}

#endif
