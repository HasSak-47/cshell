#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <object.h>
#include <generic_objects.h>
#include <formatter.h>

#include <str.h>
#include <defs.h>

DEFINE_FUNC_ID(format)

#define MAKE_FMT(type, type_str) \
	char* fmt_buf;\
	char* buf;\
	asprintf(&fmt_buf, "%%%s%s", fmt, type_str);\
	asprintf(&buf, fmt_buf, *val);\
	free(fmt_buf);\
	return buf;\

char* MFNT(format, char)(char* fmt, char* val){ MAKE_FMT(char, "c") }
char* MFNT(format, i8 )(char* fmt, i8 * val){ MAKE_FMT(i8 , "i") }
char* MFNT(format, i16)(char* fmt, i16* val){ MAKE_FMT(i16, "i") }
char* MFNT(format, i32)(char* fmt, i32* val){ MAKE_FMT(i32, "i") }
char* MFNT(format, i64)(char* fmt, i64* val){ MAKE_FMT(i64, "i") }
char* MFNT(format, u8 )(char* fmt, i8 * val){ MAKE_FMT(u8 , "u") }
char* MFNT(format, u16)(char* fmt, i16* val){ MAKE_FMT(u16, "u") }
char* MFNT(format, u32)(char* fmt, i32* val){ MAKE_FMT(u32, "u") }
char* MFNT(format, u64)(char* fmt, i64* val){ MAKE_FMT(u64, "u") }
char* MFNT(format, f32)(char* fmt, f32* val){ MAKE_FMT(f32, "f") }
char* MFNT(format, f64)(char* fmt, f64* val){ MAKE_FMT(f64, "f") }
char* MFNT(format, str)(char* fmt, str* val){ MAKE_FMT(str, "s") }

#define OVERLOADFMT(name, type)\
	overload_fn( __func_##name##_id , __type_##type##_id, (fn) __func_##name##_##type);


void __format_setup(){
	INIT_FUNC_ID(format)
	OVERLOADFMT(format, char)
	OVERLOADFMT(format, i8 )
	OVERLOADFMT(format, i16)
	OVERLOADFMT(format, i32)
	OVERLOADFMT(format, i64)
	OVERLOADFMT(format, u8 )
	OVERLOADFMT(format, u16)
	OVERLOADFMT(format, u32)
	OVERLOADFMT(format, u64)
	OVERLOADFMT(format, f32)
	OVERLOADFMT(format, f64)
	OVERLOADFMT(format, str)
}

char* format(char* fmt, void* ptr){
	char* (*rt)(char*,void*) = (char*(*)(char*,void*))lookup(__func_format_id, ptr);
	return rt(fmt, ptr);
}
