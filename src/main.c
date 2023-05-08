#include <defs.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector.h>

MAKE_VECTOR(u32);
MAKE_POP(u32);
MAKE_PUSH(u32);

int main(){
	vector_u32 v = DEF_VEC;
	for(size_t i = 0; i < 10; ++i)
		push_u32(&v, i);

	for(size_t i = 0; i < 10; ++i){
		u32 x = pop_u32(&v);
		printf("%u ", x);
	}

	free(v.ptr);


	return 0;
}
