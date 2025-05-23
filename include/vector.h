#ifndef __VECTOR_H__
#define __VECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"
#include "alloc.h"

#define vectored

/**
 * Header before a dynamic vector
 * this solution will not allow to differientate between static arrays and dynamic arrays
 */
struct vector_header {
    usize len;
    usize cap;
};

#define __SIZE_H sizeof(struct vector_header)

void __vector_push(void** vector, usize size_of, struct allocator a);
void __vector_pop(void** vector, usize size_of, struct allocator a);
void __vector_delete(void** vector, usize size_of, struct allocator a);


#define v_len(v) (((struct vector_header*)((void*)v - __SIZE_H))->len)

#define v_push(v, data){\
	__vector_push((void**)&v, sizeof(*v), cur_alloc);\
	v[v_len(v) - 1] = data;\
}

#define v_pop(v){\
	__vector_pop((void**)&v, sizeof(*v), cur_alloc);\
}

#define v_insert(v, data, pos){\
	__vector_push((void**)&v, sizeof(*v), cur_alloc);\
	usize __len = v_len(v);\
	typeof(*v) __buf_b = {};\
	typeof(*v) __buf_a = data;\
	for(usize __i = pos; __i < __len; ++__i){\
		__buf_b = v[__i];\
		v[__i] = __buf_a;\
		__buf_a = __buf_b;\
	}\
}

#define v_remove(v, pos){\
	usize __len = v_len(v);\
	for(usize __i = pos; __i < __len - 1; ++__i){\
		v[__i] = v[__i + 1];\
	}\
	__vector_pop((void**)&v, sizeof(*v), cur_alloc);\
}

#define v_delete(v) \
	__vector_delete((void**)&v, sizeof(*v), cur_alloc)


#ifdef __cplusplus
}
#endif
#endif // __VECTOR_H__
