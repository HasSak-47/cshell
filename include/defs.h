#ifndef __DEFS_H__
#define __DEFS_H__

#include <stddef.h>
#include <stdint.h>

#define MAKE_INT(b)\
typedef uint##b##_t u##b;\
typedef int##b##_t i##b;

MAKE_INT(8)
MAKE_INT(16)
MAKE_INT(32)
MAKE_INT(64)

typedef float f32;
typedef double f64;

typedef size_t usize;
typedef size_t isize;

typedef void* void_ptr;
typedef char* str;

#endif
