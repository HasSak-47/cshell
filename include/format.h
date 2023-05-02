#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <stddef.h>

typedef char* (*format_ptr)(char*, void*);
typedef char* (format_decl)(char*, void*);

format_decl formatter;

#define DECLARE(type)\
format_decl __##type##_formatter;\
extern const size_t __##type##_formatter_id;

DECLARE(u64);
DECLARE(char);
DECLARE(str);

#endif
