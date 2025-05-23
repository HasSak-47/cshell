#include <stdlib.h>
#include <string.h>
#include <str.h>

struct String from_null_terminated(const char* str){
	struct String s = {};
	size_t len = strlen(str);
	s.len = len;
	s.cstr = malloc(len);
	memcpy(s.cstr, str, len);

	return s;
}

char* to_null_terminated(struct String s){
	char* str = malloc(s.len + 1);
    if(str == NULL){
        exit(-1);
    }
	str[s.len] = 0;
    strncpy(str, s.cstr, s.len);

	return str;
}
