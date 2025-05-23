#include <stdarg.h>
#include <stdio.h>

#include <debug.h>
#include <state.h>

static void __debug_printf_set_debug(const char* fmt, va_list args){
    vprintf(fmt, args);
}

static void __debug_printf_unset_debug(const char* fmt, va_list args){}

typedef void (*__debug_printf)(const char*, va_list args);

static __debug_printf __functions[] = {__debug_printf_unset_debug, __debug_printf_set_debug};

void debug_printf(const char* fmt, ...){
    va_list(args);
    va_start(args, fmt);
    __functions[debug](fmt, args);
    va_end(args);
}
