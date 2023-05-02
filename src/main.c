#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Object.h>

#include <stdarg.h>

#include <format.h>

int main(){
	// setup
	atexit(delete_tables);

	__format_table = init_table();
	__u64_formatter_id = add_function(__format_table, (fn)__u64_formatter);
	__char_formatter_id = add_function(__format_table, (fn)__char_formatter);
	__float_formatter_id = add_function(__format_table, (fn)__float_formatter);
	__str_formatter_id = add_function(__format_table, (fn)__str_formatter);

	int   a = 10;
	char  b = 'a';
	float c = 5;

	init_value(__format_table, __u64_formatter_id, &a);
	init_value(__format_table, __char_formatter_id, &b);
	init_value(__format_table, __float_formatter_id, &c);


	return 0;
}
