#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <object.h>

#include <stdarg.h>

#include <generic_objects.h>

DEFINE_FUNC_ID(fmt)
MKGOF(fmt)

DEFINE_FUNC_ID(print)
MKGOF(print)

void* MFNT(fmt, int)(void* ptr){
	printf("int fucker!\n");
	return NULL;
}

void* MFNT(fmt, str)(void* ptr){
	printf("str fucker!\n");
	return NULL;
}

void* MFNT(print, int)(void* ptr){
	printf("int shitter!\n");
	return NULL;
}

void* MFNT(print, str)(void* ptr){
	printf("str shitter!\n");
	return NULL;
}

int main(){
	atexit(delete_tables);

	INIT_FUNC_ID(fmt);
	INIT_FUNC_ID(print);

	INIT_TYPE(i32)
	INIT_TYPE(str)

	INIT(i32, i, 0)
	INIT(str, s, (str){"pepe", 5})



	fmt(&i);


	return 0;
}
