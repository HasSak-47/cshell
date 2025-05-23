#include <utils.h>
#include <vector.h>
#include <alloc.h>
#include <tokenizer.h>

#include <stdio.h>
#include <stdbool.h>

char* token_types[] = {
	"Text",
	"Unkn",
};

void print_token(struct token t){
	printf("type: %s, \"", token_types[t.type]);
	struct slice s = t.s;
	for(char* beg = s.beg; beg < s.end; ++beg){
		printf("%c", *beg);
	}
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

	while(beg != end){
        if(*beg == ' '){
			current_slice.end = beg;
			v_push(tokens, __make_token(current_slice, false));
			current_slice.beg = beg + 1;
		}
		beg++;
	}
	current_slice.end = beg;
	v_push(tokens, __make_token(current_slice, false));

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

	return tokens;
}
