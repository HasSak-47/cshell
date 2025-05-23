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


#define MAKE_STRUCT_VECTOR(type)\
struct vector_##type{\
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

void   __push(struct __vector*, void*, size_t, struct allocator a);
void __insert(struct __vector*, void*, size_t, size_t, struct allocator a);
void __remove(struct __vector*, size_t, size_t, struct allocator a);
void    __pop(struct __vector*, size_t, struct allocator a);

#define v_push(vector, var) \
{\
	typeof(*vector.ptr) __aux = var; \
	__push((struct __vector*)&vector, (void*)&__aux, sizeof(__aux), cur_alloc); \
}

#define v_pop(vector) \
__pop((struct __vector*)&vector, sizeof(*vector.ptr), cur_alloc);

#define v_insert(vector, var, pos) \
{\
	typeof(*vector.ptr) __aux = var; \
	__insert((struct __vector*)&vector, (void*)&__aux, pos, sizeof(__aux), cur_alloc); \
}

#define v_remove(vector, pos) \
__remove((struct __vector*)&vector, pos, sizeof(*vector.ptr), cur_alloc);


#endif 
