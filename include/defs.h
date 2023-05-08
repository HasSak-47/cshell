#ifndef __DEFS_H__
#define __DEFS_H__
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

typedef void* void_ptr;

#undef MAKE_INT

#endif
