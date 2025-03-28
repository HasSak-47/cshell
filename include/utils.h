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
    T* data;   \
    size_t len;   \
    size_t cap;   \
};

void __vector_push(struct __Vector* v, void* data, size_t size);
void __vector_pop (struct __Vector* v, size_t size);

#define vector_push(vector, data){ \
    typeof(data) __vector_data; \
    __vector_push((struct __Vector)&vector, &__vector_data, sizeof(*vector.data))\
}

#define vector_pop(vector)\
    __vector_pop((struct __Vector)&vector, sizeof(*vector.data))

#endif
