#include <Object.h>
#include <stdio.h>
#include <stdlib.h>

struct array{
	void** ptrs;
	size_t size;
};

typedef struct array v_table;
typedef struct array t_table;

v_table* vtable = NULL;
size_t vsize = 0;

t_table* ttable = NULL;
size_t tsize = 0;


struct f_table{
	t_table* type_table;
	fn f;
};

void push(void** ptr, size_t* n, size_t len){
	void* aux = realloc(*ptr, (*n + 1) * len);
	if(aux == NULL){
		printf("RAN OUT OF MEMORY!");
		exit(EXIT_FAILURE);
	}

	*ptr = aux;
	*n = *n + 1;
}

static size_t __init(struct array** tt, size_t* t){
	size_t id = *t;
	push((void**)tt, t, sizeof(struct array));

	(*tt)[id].ptrs = NULL;
	(*tt)[id].size = 0;

	return id;
}

size_t init_type(){
	return __init(&ttable, &tsize);
}

size_t init_fn(){
	return __init(&vtable, &vsize);
}

void overload_fn(size_t vt_id, size_t t_id, fn f){
	size_t id = vtable[vt_id].size;
	push((void**)&vtable[vt_id].ptrs, &vtable[vt_id].size, sizeof(struct f_table));

	struct f_table* ft = &((struct f_table*)vtable[vt_id].ptrs)[id];
	ft->f = f;
	ft->type_table = &ttable[t_id];
}

void init(size_t t_id, void* ptr){
	size_t id = ttable[t_id].size;
	push((void**)&ttable[t_id].ptrs, &ttable[t_id].size, sizeof(void*));
	ttable[t_id].ptrs[id] = ptr;
}

fn lookup(size_t vt_id, void* ptr){
	for(size_t i = 0; i < vtable[vt_id].size; ++i){
		struct f_table* f = &((struct f_table*)vtable[vt_id].ptrs)[i];
		for(size_t j = 0; j < f->type_table->size; ++j){
			if(f->type_table->ptrs[j] == ptr){
				return f->f;
			}
		}
	}

	printf("LOOKUP FAILED\n");
	exit(EXIT_FAILURE);
	return NULL;
}

void __print_tables(){
	printf("vtable content:\n");
	printf("init %p:\n", vtable);
	for(size_t i = 0; i < vsize; ++i){
		printf("\tvtable %lu content:\n", i);
		for(size_t j = 0; j < vtable[i].size; ++j){
			struct f_table* f = &((struct f_table*)vtable[i].ptrs)[j];
			printf("\tftable %lu (%p): %p %p\n", j, f,  f->type_table, f->f);
		}
	}

	printf("ttable content:\n");
	printf("init: %p\n", ttable);
	for(size_t i = 0; i < tsize; ++i){
		printf("ttable %lu (%p): %p %lu\n", i, &ttable[i], ttable[i].ptrs, ttable[i].size);
	}
}

void delete_tables(){
	for(size_t i = 0; i < vsize; ++i){
		free(vtable[i].ptrs);
	}
	free(vtable);

	for(size_t i = 0; i < tsize; ++i){
		free(ttable[i].ptrs);
	}
	free(ttable);

}
