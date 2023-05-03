#include "formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <object.h>
#include <stdarg.h>
#include <generic_objects.h>

#include <stdarg.h>

void print(const char* fmt, ...){
	va_list v;
	va_start(v, fmt);
	size_t len = strlen(fmt);
	for(size_t i = 0; i < len; ++i){
		if(fmt[i] == '%'){
			void* p = va_arg(v, void*);
			char* c = format("", p);
			printf("%s", c);
			free(c);
		}
	}

	va_end(v);
}

int main(){
	atexit(delete_tables);
	__init_generic_objects();
	__format_setup();

	INIT(i32, a, 10)
	INIT(f32, b, 10)
	print("% %", &a, &b);


	return 0;
}
