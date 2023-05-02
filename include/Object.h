#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stddef.h>
#define PTR_SIZE sizeof(void*)

typedef void* (*fn)(void*);


// should be replaced with a hash function
fn lookup(void* ptr, size_t vID);
void init_value(size_t vID, size_t lID, void* ptr);

void push(void**, size_t*, size_t);

size_t init_table();
size_t add_function(size_t vID, fn f);
void delete_tables();

#define MAKE_OVERLOAD(type, function) __##type##_id = add_function(function##_table, type##_##function)

#endif
