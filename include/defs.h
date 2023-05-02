#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>

#define MAKE_INT(b)\
typedef uint##b##_t u##b;\
typedef int##b##_t u##b;


#undef MAKE_INT

#endif
