#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "vector.h"


enum token_type{
	TokenText,
	TokenUndetermined,
};

struct slice{
	char* beg;
	char* end;
};

struct token{
	struct slice s;
	enum token_type type;
};

MAKE_STRUCT_VECTOR_NAMED(token, struct token);

struct vector_token make_tokens(char*);

#endif
