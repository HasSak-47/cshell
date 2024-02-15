#ifndef __VECTOR__
#define __VECTOR__

#include "defs.h"
#include "alloc.h"

/**
 * Default vector that has a void* as it's pointer
**/
struct __vector {
    void_ptr ptr;
    usize len;
    usize cap;
};


#define MAKE_STRUCT_VECTOR(typename)\
struct vector_##typename{\
    type* ptr;\
    usize len;\
    usize cap;\
};

#define MAKE_STRUCT_VECTOR_NAMED(name, type)\
struct vector_##name{\
    type* ptr;\
    usize len;\
    usize cap;\
};

void __push(struct __vector*, void*, size_t, struct allocator a);
void __pop(struct __vector*, size_t, struct allocator a);

#define push(vector, var) \
{\
	typeof(*vector.ptr) __aux = var; \
	__push((struct __vector*)&vector, (void*)&__aux, sizeof(__aux), cur_alloc); \
}

#define pop(vector) \
{\
	typeof(*vector.ptr) __aux = var; \
	__pop((struct __vector*)&vector, (void*)&__aux, sizeof(__aux), cur_alloc); \
}


#endif 
