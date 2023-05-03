#ifndef __FORMATER_H__
#define __FORMATER_H__

#include "object.h"

DECLARE_FUNC_ID(format)
char* format(char* fmt, void* ptr);
void __format_setup();

#ifdef FORMAT_DEF_GENERIC
#endif

#endif
