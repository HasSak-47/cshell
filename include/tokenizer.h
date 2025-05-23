#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

enum token_type{
	token_text,
	token_pipe,
	token_misc,
	token_redir
};

struct token{
	char* value;
	enum token_type type;
};

struct token* make_tokens(char*);

#endif
