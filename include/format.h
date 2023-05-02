#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <stddef.h>

#include "Object.h"

typedef char* (*format_ptr)(char*, void*);
typedef char* (format_decl)(char*, void*);

format_decl formatter;

#define MFUNC_NAME(type) __##type##_formatter
#define MID_NAME(type) __##type##_formatter_id

#define DECLARE(type)\
format_decl MFUNC_NAME(type);\
extern size_t MID_NAME(type);

DECLARE(u64)
DECLARE(float)
DECLARE(char)
DECLARE(str)

extern size_t __format_table;

#ifndef __FORMAT_C__
#undef DECLARE
#undef MFUNC_NAME
#undef MID_NAME
#endif

#define ADD_FORMAT(type, fn)\
	add_function(__format_table, __##type##_formatter_id);

#endif
