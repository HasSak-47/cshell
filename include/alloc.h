#ifndef __ALLOC_H__
#define __ALLOC_H__

#define __DEBUG_MEM

#include <stddef.h>
typedef void* (*__reallocator)(void*,size_t);
typedef void* (*__allocator)(size_t);
typedef void* (*__callocator)(size_t, size_t);
typedef void  (*__deleter)(void*);

struct allocator{
    __reallocator r;
    __allocator a;
    __callocator c;
    __deleter d;
};

extern struct allocator stdalloc;

#ifdef __DEBUG_MEM
void __mem_debug_init();
void __mem_debug_end();
#endif // __DEBUG_MEM

#endif