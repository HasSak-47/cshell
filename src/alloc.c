#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <alloc.h>
#include <defs.h>

struct allocator stdalloc = {
    realloc,
    malloc,
    calloc,
    free
};

struct ptrs{
    void** ptr;
    size_t size;
    size_t cap;
};

// #define __LOG_STDOUT

struct ptrs inited_ptrs = {};
FILE* log_out = 0;

void __mem_debug_init(){
    inited_ptrs.ptr  = NULL;
    inited_ptrs.size = 0;
    inited_ptrs.cap  = 0;
#ifdef __LOG_STDOUT
    log_out = stdout;
#else
    log_out = fopen("log/mem_log.txt", "w+");
    if(log_out == NULL){
        printf("could not make file\n");
        exit(-1);
    }
#endif
	fprintf(log_out, "current allocator is debugalloc...\n");
	fprintf(log_out, "started debugalloc...\n");
}

void __mem_debug_end(){
    fprintf(log_out, "ending mem debug...\n");
    fprintf(log_out, "ending data: %lu %lu %p\n", inited_ptrs.cap, inited_ptrs.size, inited_ptrs.ptr);
    if(inited_ptrs.ptr != NULL){
        fprintf(log_out, "freeing tracker at: %p\n", inited_ptrs.ptr);
        free(inited_ptrs.ptr);
	}

#ifndef __LOG_STDOUT
	fclose(log_out);
#endif

}

static void __add_to_tracker(void* ptr){
	if(ptr == NULL){
        fprintf(log_out, "will not add nullptr to trackerptr!");
		return;
	}
	for(size_t i = 0; i < inited_ptrs.size; ++i){
		if(inited_ptrs.ptr[i] == NULL){
			fprintf(log_out, "[TRACKER]: found null at %lu, filling with ptr %p\n", i, ptr);
			inited_ptrs.ptr[i] = ptr;
			return;
		}
	}
    if(inited_ptrs.cap == inited_ptrs.size){
        size_t new_cap = inited_ptrs.cap != 0 ? inited_ptrs.cap * 2: 1;
        fprintf(log_out, "resized tracker to: %lu entries\n", new_cap);
        void** aux = (void**)realloc(inited_ptrs.ptr, new_cap * sizeof(void*));
        fprintf(log_out, "tracker org %p aux %p\n", inited_ptrs.ptr, aux);
        if(aux == NULL){
            fprintf(log_out, "could not increase ptr tracker!\n");
            exit(-1);
        }
        inited_ptrs.ptr = aux;
        inited_ptrs.cap = new_cap;
    }
    inited_ptrs.ptr[inited_ptrs.size++] = ptr;
}

static void __remove_from_tracker(void* ptr){
    for(size_t i = 0; i < inited_ptrs.size; ++i){
        if(inited_ptrs.ptr[i] == ptr){
            inited_ptrs.ptr[i] = NULL;
            return;
        }
    }
    fprintf(log_out, "ptr double deleted or not allocated!\n");
}

static void __change_in_tracker(void* org, void* new){
    for(size_t i = 0; i < inited_ptrs.size; ++i){
        if(inited_ptrs.ptr[i] == org){
            inited_ptrs.ptr[i] = new;
            return;
        }
    }
    fprintf(log_out, "ptr double deleted or not allocated!\n");
}

void* malloc_debug(usize);

void* realloc_debug(void* ptr, usize len){
	if(ptr == NULL){
		fprintf(log_out, "[REALLOC]: realloc ptr is null, calling alloc initing to bytes %lu\n", len);
		return malloc_debug(len);
	}
    fprintf(log_out, "[REALLOC]: %p to %lu bytes\n", ptr, len);
	void* aux = realloc(ptr, len);
	if(aux != ptr){
		fprintf(log_out, "[REALLOC]: moved %p to %p\n", ptr, aux);
		__change_in_tracker(ptr, aux);
	}
    return aux;
}

void* malloc_debug(usize len){
    void* p = malloc(len);
    __add_to_tracker(p);
    fprintf(log_out, "[MALLOC]: %lu at %p\n", len, p);

    return p;
}
void* calloc_debug(usize size_of, usize len){
    void* p = calloc(size_of, len);
    fprintf(log_out, "[CALLOC]: %lu x %lu at %p\n", size_of, len, p);
    __add_to_tracker(p);
    return p;
}
void free_debug(void* ptr){
    fprintf(log_out, "[FREE]: %p\n", ptr);
    __remove_from_tracker(ptr);
    free(ptr);
}

struct allocator debugalloc = {
    realloc_debug,
    malloc_debug,
    calloc_debug,
    free_debug
};
