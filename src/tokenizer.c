#include <utils.h>
#include <alloc.h>
#include <tokenizer.h>
#include <stdlib.h>

struct slice{
    char* b;
    char* e;
};

// not effitien but idc
struct token* make_tokens(char* line){
    struct token* tokens = NULL;
	//first pass
	//find all the ""
    size_t quote_count = 0;
    char* ptr = line;
    while(ptr != NULL){
        if(*(ptr++) == '"')
            quote_count++;

    }

    struct slice* slices = stdalloc.c(sizeof(struct token), quote_count);


	return NULL;
}
