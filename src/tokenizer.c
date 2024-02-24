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

void print_token(struct token t){
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

static struct token vectored* expand_token(struct token token){
    struct token vectored* tokens = {};
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
struct token vectored* make_tokens(char* line){
    struct token vectored* tokens = {};
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
	for(size_t i = 0; i < v_len(tokens); ++i){
		if(tokens[i].type == TokenUndetermined){
			typeof(tokens) expanded = expand_token(tokens[i]);
			v_remove(tokens, i);
			for(size_t j = 0; j < v_len(expanded); ++j)
				v_insert(tokens, expanded[j], i + j);
			i += v_len(expanded);
			v_delete(expanded);
		}
	}

	return tokens;
}

char vectored* vectored* split_into_args(struct token vectored* vector){
	char vectored* vectored* args = NULL;
	size_t len = v_len(vector);
	for(size_t i = 0; i < len; ++i){
		char vectored* new_char = NULL;
		size_t t_len = vector[i].s.end - vector[i].s.beg;
		for(size_t j = 0; j < t_len; ++j){
			v_push(new_char, vector[i].s.beg[j]);
		}
		v_push(new_char, 0);
		v_push(args, new_char);
		new_char = NULL;
	}
	v_push(args, NULL);
	return args;
}
