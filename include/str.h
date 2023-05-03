#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>
#include <object.h>

typedef char* str;

typedef struct string{
	str s;
	size_t len;
} string;

DECLARE_TYPE(string)
DECLARE_DEFAULT(string)
char* MFNT(format, string)(char* fmt, string* str);

#endif
