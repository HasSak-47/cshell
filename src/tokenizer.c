#include <ctype.h>
#include <utils.h>
#include <vector.h>
#include <alloc.h>
#include <tokenizer.h>

#include <stdio.h>
#include <stdbool.h>

static char* token_types[] = {
	"Text",
	"Unkn",
};

static void print_token(struct token t){
	printf("type: %s, \"", token_types[t.type]);
	struct slice s = t.s;
	for(char* beg = s.beg; beg < s.end; ++beg){
		printf("%c", isprint(*beg) ? *beg : '.');
	}
	printf("\"");
}

static void print_slice(struct slice s){
	printf("\"");
	for(char* beg = s.beg; beg < s.end; ++beg)
		printf("%c", isprint(*beg) ? *beg : '.');
	printf("\"");
}

static struct token __make_token(struct slice s, bool text){
	struct token b = {s, text ? TokenText : TokenUndetermined};
	return b;
}

static struct vector_token expand_token(struct token token){
    struct vector_token tokens = {};
	struct slice current_slice = {};
	char* beg = token.s.beg;
	char* end  = token.s.end;


	while(true){
		while(isspace(*beg) && beg < end)
			beg++;
		current_slice.beg = beg;

		while(!isspace(*beg) && beg < end)
			beg++;
		current_slice.end = beg;

		if(current_slice.end != current_slice.beg)
			v_push(tokens, __make_token(current_slice, false));
		if(beg == end)
			break;
	}

	

	return tokens;
}

// not effitien but idc
struct vector_token make_tokens(char* line){
    struct vector_token tokens = {};
	struct slice current_slice = {};

	current_slice.beg = line;
	char* ptr = line + 1;
	bool inside_quote = false;
	while(*ptr != 0){
        if(*ptr == '"'){
			current_slice.end = ptr;
			v_push(tokens, __make_token(current_slice, inside_quote));
			current_slice.beg = ptr + 1;
			inside_quote = !inside_quote;
		}
		ptr++;
	}
	current_slice.end = ptr;
	v_push(tokens, __make_token(current_slice, inside_quote));
	for(size_t i = 0; i < tokens.len; ++i){
		if(tokens.ptr[i].type == TokenUndetermined){
			typeof(tokens) expanded = expand_token(tokens.ptr[i]);
			v_remove(tokens, i);
			for(size_t j = 0; j < expanded.len; ++j)
				v_insert(tokens, expanded.ptr[j], i + j);
			cur_alloc.d(expanded.ptr);
			i += expanded.len;
		}
	}

	return tokens;
}

struct vector_char split_into_args(struct vector_token* vector){
	struct vector_char args = {};
	v_resize(args, vector->len);
	for(size_t i = 0; i < vector->len; ++i){
		size_t token_len = vector->ptr[i].s.end - vector->ptr[i].s.beg;
		args.ptr[i] = (char*)cur_alloc.a(token_len);
	}
	return args;
}
