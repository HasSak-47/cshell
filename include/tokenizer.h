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

void print_token(struct token token);
struct token vectored* make_tokens(char*);
char vectored* vectored* split_into_args(struct token vectored* vector);


#endif
