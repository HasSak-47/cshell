#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

typedef char* str;

struct string{
	str* s;
	size_t len;
};

#endif
