#include <defs.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector.h>
#include <vector_primitives.h>


typedef void (*fn)(void);
typedef void_vector ttable;

typedef struct ftable{
	fn f;
	ttable* t;
} ftable;

typedef struct vtable{
	ftable* f;
	size_t size;
} vtable;

#define CAST(type, v) *((type*)(&v))

int main(){
	return 0;
}
