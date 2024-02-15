#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>

/*
 * catastrophic realloc
 * if realloc returns null kill the program
 */
void cat_realloc(void** c, size_t s);
void hex_dump(void*, size_t);

#endif
