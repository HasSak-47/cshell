#include <alloc.h>
#include <defs.h>
#include <stdlib.h>
#include <stdio.h>


#ifndef __DEBUG_MEM
struct allocator stdalloc = {
    realloc,
    malloc,
    calloc,
    free
};
#else // __DEBUG_MEM
#include <stdbool.h>

struct ptrs{
    void** ptr;
    size_t size;
    size_t cap;
};

struct ptrs inited_ptrs;

void __mem_debug_init(){
    printf("initing mem debug...\n");
    inited_ptrs.ptr  = NULL;
    inited_ptrs.size = 0;
    inited_ptrs.cap  = 0;
}

void __mem_debug_end(){
    printf("ending mem debug...\n");
    printf("ending data: %lu %lu %p\n", inited_ptrs.cap, inited_ptrs.size, inited_ptrs.ptr);
    if(inited_ptrs.ptr != NULL)
        free(inited_ptrs.ptr);
}

void __add_to_tracker(void* ptr){
    if(inited_ptrs.cap == inited_ptrs.size){
        size_t new_cap = inited_ptrs.cap != 0 ? inited_ptrs.cap * 2: sizeof(void*);
        printf("resized tracker to: %lu\n", new_cap);
        void* aux = realloc(inited_ptrs.ptr, new_cap);
        if(aux == NULL){
            printf("could not increase ptr tracker!\n");
            exit(-1);
        }
        inited_ptrs.ptr = aux;
        inited_ptrs.cap = new_cap;
    }
    inited_ptrs.ptr[inited_ptrs.size++] = ptr;
}

void __remove_from_tracker(void* ptr){
    for(size_t i = 0; i < inited_ptrs.size; ++i){
        if(inited_ptrs.ptr[i] == ptr){
            inited_ptrs.ptr[i] = NULL;
            inited_ptrs.size--;
            return;
        }
    }
    printf("ptr double deleted or not allocated!\n");
}

void* realloc_debug(void* ptr, usize len){
    printf("\nreallocating %p to %lu bytes\n", ptr, len);
    return realloc(ptr, len);
}
void* malloc_debug(usize len){
    void* p = malloc(len);
    __add_to_tracker(p);
    printf("\nallocation %lu bytes at %p\n", len, p);

    return p;
}
void* calloc_debug(usize size_of, usize len){
    void* p = calloc(size_of, len);
    __add_to_tracker(p);
    printf("\ncallocation %lu x %lu bytes at %p\n", size_of, len, p);
    return p;
}
void free_debug(void* ptr){
    printf("\nfreeing at %p\n", ptr);
    __remove_from_tracker(ptr);
    free(ptr);
}

struct allocator stdalloc = {
    realloc_debug,
    malloc_debug,
    calloc_debug,
    free_debug
};


#endif // !__DEBUG_MEM

