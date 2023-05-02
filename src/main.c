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

void* MFNT(fmt, i32)(void* ptr){
	printf("int fucker!\n");
	return NULL;
}

void* MFNT(fmt, str)(void* ptr){
	printf("str fucker!\n");
	return NULL;
}

void* MFNT(print, i32)(void* ptr){
	printf("int shitter!\n");
	return NULL;
}

void* MFNT(print, str)(void* ptr){
	printf("str shitter!\n");
	return NULL;
}

int main(){
	atexit(delete_tables);
	// ---   setup   --- //
	INIT_FUNC_ID(fmt);
	INIT_FUNC_ID(print);

	__init_generic_objects();


	OVERLOAD(fmt, i32)
	OVERLOAD(fmt, str)
	OVERLOAD(print, i32)
	OVERLOAD(print, str)
	// --- end setup --- //

	INIT(i32, i, 0)
	INIT(str, s, "string")

	fmt(&i);
	fmt(&s);

	print(&i);
	print(&s);

	return 0;
}
