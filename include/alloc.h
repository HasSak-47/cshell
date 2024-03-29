#ifndef __ALLOC_H__
#define __ALLOC_H__

// #define __DEBUG_MEM

#include <stddef.h>
typedef void* (*__reallocator)(void* ,size_t);
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
extern struct allocator debugalloc;


#ifdef __DEBUG_MEM
	#define cur_alloc debugalloc
void __mem_debug_init();
void __mem_debug_end();

#else
    #define cur_alloc stdalloc
#endif // __DEBUG_MEM

#define mem_alloc(size) cur_alloc.a(size)
#define zer_alloc(size, size2) cur_alloc.c(size, size2)
#define res_alloc(ptr, size) cur_alloc.r(ptr, size)
#define del_alloc(ptr) cur_alloc.d(ptr)

#endif // !__DEBUG_MEM

