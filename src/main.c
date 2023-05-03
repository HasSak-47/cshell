#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <generic_objects.h>
#include <formatter.h>
#include <object.h>
#include <colored_string.h>

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
		else{
			printf("%c", fmt[i]);
		}
	}

	va_end(v);
}

int main(){
	atexit(delete_tables);
	INIT_TYPE(string)
	__init_generic_objects();
	__format_setup();
	overload_fn(FUNC_ID(format), TYPE_ID(string), (fn)MFNT(format, string));
	overload_fn(FUNC_ID(format), TYPE_ID(colored_string), (fn)MFNT(format, colored_string));
	INIT_DEFAULT(colored_string, str_test)
	str_test.b[0] = 0;
	str_test.b[1] = 0;
	str_test.b[2] = 0;
	str_test.f[0] = 255;
	str_test.f[1] = 255;
	str_test.f[2] = 255;
	str_test.str.s = "12345";
	str_test.str.len = 3;

	print("%\n", &str_test);

	return 0;
}
