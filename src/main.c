#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Object.h>

#include <stdarg.h>

DECLARE_TYPE(int)
DEFINE_TYPE(int)

DECLARE_FUNC_ID(fmt)
DEFINE_FUNC_ID(fmt)

MKGOF(fmt)

void* MFNT(fmt, int)(void* ptr){
	printf("fucker!\n");
	return NULL;
}


int main(){
	atexit(delete_tables);

	INIT_FUNC_ID(fmt);
	INIT_TYPE(int)
	INIT(int, i, 0)


	OVERLOAD(fmt, int);

	fmt(&i);


	return 0;
}
