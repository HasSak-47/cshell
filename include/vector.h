#ifndef __VECTOR__
#define __VECTOR__

#include "defs.h"

struct __vector {
    void_ptr ptr;
    usize len;
    usize cap;
};


#define MAKE_STRUCT_VECTOR(type)\
struct vector_##type{\
    type* ptr;\
    usize len;\
    usize cap;\
};

void __push(struct __vector*,void*,size_t);
void __pop(struct __vector*,size_t);

#define push(vector, var) __push((struct __vector*)&vector, (void*)&var, sizeof(ver));
#define pop(vector) __pop((struct __vector*)&vector, sizeof(*(vector.ptr)));

#define DECLARE_POP(type) void pop_##type(struct vector_##type*);
#define DECLARE_PUSH(type) void push_##type(struct vector_##type*, type);

#define DEFINE_POP(type) \
void pop_##type(struct vector_##type* vector){\
    __pop((struct __vector*)vector, sizeof(*(vector->ptr)));\
}
#define DEFINE_PUSH(type) \
void push_##type(struct vector_##type* vector, type data){\
    __push((struct __vector*)vector, &data, sizeof(*(vector->ptr)));\
}

MAKE_STRUCT_VECTOR(i32)
DECLARE_PUSH(i32)
DECLARE_POP(i32)


#endif 
