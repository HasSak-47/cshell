#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

typedef char* str;

typedef struct string{
	str* s;
	size_t len;
} string;

#endif
