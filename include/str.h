#ifndef __LY_STRING_H__
#define __LY_STRING_H__

#include <stddef.h>

struct String {
	size_t len;
	char* cstr;
};

#define MAKE_STRING(s) (struct String) { sizeof(s), s }

struct String from_null_terminated(const char* str);
char* to_null_terminated(struct String s);

#endif
