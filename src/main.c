#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Object.h>

void* int_formatter(void* p){
	char* c;
	asprintf(&c, "%i", *(int*)p);

	return c;
}

void* char_formatter(void* p){
	char* c;
	asprintf(&c, "%c", *(char*)p);

	return c;
}

void* float_formatter(void* p){
	char* c;
	asprintf(&c, "%f", *(float*)p);

	return c;
}

struct str{
	char* c;
	size_t s;
};

void* str_formatter(void* p){
	struct str* s = (struct str* )p;
	char* c; asprintf(&c, "%*.s", (int)s->s, s->c);

	return c;
}

size_t format_table;
size_t int_id, char_id, float_id, str_id;

#include <stdarg.h>

void print(char* fmt, ...){
	va_list v;
	va_start(v, fmt);
	size_t len = strlen(fmt);
	for(size_t i = 0; i < len; ++i){
		if(fmt[i] == '%'){
			void* p = va_arg(v, void*);
			fn format = lookup(p, format_table);
			char* c = format(p);
			printf("%s", c);
			free(c);
		}
		else{
			printf("%c", fmt[i]);
		}
	}
	fflush(stdout);
}

int main(){
	// setup
	atexit(delete_tables);

	format_table = init_table();
	int_id = add_function(format_table, int_formatter);
	char_id = add_function(format_table, char_formatter);
	float_id = add_function(format_table, float_formatter);
	str_id = add_function(format_table, str_formatter);

	int   a = 10;
	char  b = 'a';
	float c = 5;

	init_value(format_table, int_id, &a);
	init_value(format_table, char_id, &b);
	init_value(format_table, float_id, &c);

	print("test: % % %\n", &a, &b, &c);

	return 0;
}
