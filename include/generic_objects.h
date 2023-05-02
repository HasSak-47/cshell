#ifndef __GENERIC_OBJECTS__
#define __GENERIC_OBJECTS__

#include "object.h"
#include "str.h"
#include "defs.h"

DECLARE_TYPE(u8)
DECLARE_TYPE(u16)
DECLARE_TYPE(u32)
DECLARE_TYPE(u64)

DECLARE_TYPE(i8)
DECLARE_TYPE(i16)
DECLARE_TYPE(i32)
DECLARE_TYPE(i64)

DECLARE_TYPE(f32)
DECLARE_TYPE(f64)

DECLARE_TYPE(str)
DECLARE_TYPE(string)

void __init_generic_objects();

#endif
