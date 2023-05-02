#include <Object.h>
#include <stdio.h>
#include <stdlib.h>

struct v_table* tables = NULL;
size_t vtable_size = 0;

struct l_table{
	void** ptrs;
	size_t size;
	fn f;
};

struct v_table{
	struct l_table* tables;
	size_t table_size;
};

// HASH REMEMBER TO HASH
fn lookup(void *ptr, size_t vID){
	struct v_table* v = &tables[vID];
	for(size_t i = 0; i < v->table_size; ++i)
		for(size_t j = 0; j < v->tables[i].size; ++j)
			if(ptr == v->tables[i].ptrs[j])
				return v->tables[i].f;

	printf("value not found!\n");
	exit(EXIT_FAILURE);
}

void init_value(size_t vID, size_t lID, void *ptr){
	size_t i = tables[vID].tables[lID].size;
	push((void**)&tables[vID].tables[lID].ptrs, &tables[vID].tables[lID].size, sizeof(void*));

	tables[vID].tables[lID].ptrs[i] = ptr;
}

void push(void** ptr, size_t* n, size_t size){
	// printf("%p %p %p %lu %lu\n", ptr, *ptr, n, *n, size);
	void* aux = realloc(*ptr, (*n + 1) * size);
	if(aux == NULL){
		printf("ran out of memory\n");
		exit(EXIT_FAILURE);
	}

	*ptr = aux;
	*n = *n + 1;
}

size_t add_function(size_t vID, fn f){
	size_t n = tables[vID].table_size;
	push((void**)&tables[vID].tables, &tables[vID].table_size, sizeof(struct l_table));
	tables[vID].tables[n].f = f;
	tables[vID].tables[n].size = 0;
	tables[vID].tables[n].ptrs = NULL;

	return n;
}

size_t init_table(){
	size_t n = vtable_size;
	push((void**)&tables, &vtable_size, sizeof(struct v_table));
	tables[n].tables = NULL;
	tables[n].table_size = 0;

	return n;
}

//memory leak
void delete_tables(){
	for(size_t i = 0; i < vtable_size; ++i){
		for(size_t j = 0; j < tables[i].table_size; ++j){
			free(tables[i].tables[j].ptrs);
		} 
		free(tables[i].tables);
	}

	free(tables);
}
