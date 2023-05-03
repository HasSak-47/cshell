#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stddef.h>

typedef void* (*fn)(void*);

#define DECLARE_TYPE(type) extern size_t __type_##type##_id;
#define  DEFINE_TYPE(type) size_t __type_##type##_id = 0;
#define    INIT_TYPE(type) __type_##type##_id = init_type();

#define INIT(type, name, value)\
	type name = value;\
	init( __type_##type##_id , & name );

#define DECLARE_DEFAULT(type) extern const type __type_##type##_default;
#define DEFINE_DEFAULT(type) const type __type_##type##_default = 

#define INIT_DEFAULT(type, name)\
	type name = __type_##type##_default;\
	init( __type_##type##_id , & name );
	

#define DECLARE_FUNC_ID(name) extern size_t __func_##name##_id;
#define  DEFINE_FUNC_ID(name) size_t __func_##name##_id = 0;
#define    INIT_FUNC_ID(name) __func_##name##_id = init_fn();

#define MFNT(name, type) __func_##name##_##type

#define OVERLOAD(name, type) overload_fn( __func_##name##_id , __type_##type##_id, __func_##name##_##type );

#define MKGOF(name)\
void* name(void* ptr){\
	return lookup(__func_##name##_id, ptr)(ptr);\
}

#define OUT(type, name) out(__type_##type##_id, &name);

//init funcs 
// creates table for the type T
// returns the id of the type T
size_t init_type();
// overloads the funtion fn for the type T
void overload_fn(size_t vt_id, size_t t_id, fn f);
//returns the id of the vtable
size_t init_fn();

//vars
// adds value to the type table
void init(size_t t_id, void* ptr);
void out(size_t t_id, void* ptr);



//del funcs
void delete_tables();

//debug
void __print_tables();

fn lookup(size_t vt_id, void* ptr);



#endif
