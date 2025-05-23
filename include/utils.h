#ifndef __UTILS__
#define __UTILS__

#include <stddef.h>

#define unrecoverable_error(msg)\
    __error_msg(__LINE__, __FILE__, "[ERROR @ %s : %d]: %s\n", msg, -1)

#define temporal_suicide_crash(msg, code)\
    __error_msg(__LINE__, __FILE__, "TODO: handle err at %s in line %d %s\n", msg, code)

#define temporal_suicide_msg(msg)\
    temporal_suicide_crash(msg, -1) 

#define temporal_suicide()\
    temporal_suicide_crash("[?]", -1) 


void __error_msg(int line, char* file, char* fmt, char* msg, int code);

void set_to_foreground();

struct __Vector{
    void* data;
    size_t len;
    size_t cap;
};

#define DefineVector(Name, T) \
struct Vector##Name{ \
    T* data; \
    size_t len; \
    size_t cap; \
}; \

void __vector_push(struct __Vector* v, const void* data, const size_t size);
void __vector_pop (struct __Vector* v, const size_t size);
void __vector_reserve(struct __Vector* v, const size_t cap, const size_t size);
void __vector_clone(struct __Vector* dst, const struct __Vector* const src, const size_t size);

#define vector_push(__vector, __v_data){ \
    const typeof(__v_data) __vector_data = __v_data; \
    __vector_push((struct __Vector*)&__vector, &__vector_data, sizeof(*__vector.data));\
}

#define vector_clone(__dst, __src){ \
    __vector_clone(\
        (struct __Vector*)&__dst,\
        (struct __Vector*)&__src,\
        sizeof(*__src.data)\
    );\
}

#define vector_pop(vector)\
    __vector_pop((struct __Vector*)&vector, sizeof(*vector.data))

#define vector_reserve(vector, cap)\
    __vector_reserve((struct __Vector*)&vector, cap, sizeof(*vector.data))

#endif
