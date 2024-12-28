#ifndef __LY_STRING_H__
#define __LY_STRING_H__

#include <stddef.h>

struct String {
	size_t len;
	char* cstr;
};

#endif
