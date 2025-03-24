#include <utils.h>
#include <ctype.h>
#include <state.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <api.h>
#include <input.h>
#include <interface.h>

struct Input{
    char*  buf; // buffer not null terminated
    size_t cap;
    size_t len;
    size_t cur; // position of the cursor inside the string 
};

void insert_char(struct Input* in, char c){
    size_t target_len = in->len + 1;
    if (target_len >= in->cap) {
        size_t target_cap = (in->cap + 1) * 2;
        void* aux = realloc(in->buf, target_cap);
        if (aux == NULL) {
            temporal_suicide_msg("I dont want to handle this");
        }

        in->buf = aux;
        in->cap = target_cap;
    }

    char prev = c;
    for (size_t i = in->cur; i < in->len + 1; ++i) {
        char aux = in->buf[i];
        in->buf[i] = prev;
        prev = aux;
    }
    in->len++;
    in->cur++;
}

void delete_char(struct Input* in){
    // there is nothing before the cursor
    // and nothing can be deleted
    // so just continue
    if (in->cur == 0){
        return;
    }
    // make everything the next character
    for (size_t i = in->cur - 1; i < in->len; ++i) {
        in->buf[i] = in->buf[i + 1];
    }
    in->len = in->len - 1;
    in->cur = in->cur - 1;
}

struct InputState{
    bool cont; // should continue
    struct Input in;
    /**
    * 0 means the original input
    * [1, inf) means get the [0, inf) history element in reverse order
    */
    size_t index;
};

void render_input(struct Input* in){
    if (in->len == 0) {
        putchar(' ');
        printf("\e[D");
        fflush(stdout);
        return;
    }
    for (size_t i = 0; i < in->len; ++i)
        putchar(in->buf[i]);
    
    printf("\e[%luD", in->len);

    if (in->cur > 0)
        printf("\e[%luC", in->cur);

    fflush(stdout);
    if (in->cur > 0)
        printf("\e[%luD", in->cur);

    for (size_t i = 0; i < in->len; ++i)
        putchar(' ');

    printf("\e[%luD", in->len);
}

/**
 * takes an string and makes it the buffer of input
 */
void set_string(struct Input* in, char* str){
    free(in->buf);
    size_t len = strlen(str);

    in->buf = str;
    in->len = len;
    in->cur = len;
}

/**
 * takes an string and clones it to make it the buffer of input
 */
void copy_string(struct Input* in, char* str){
    free(in->buf);
    size_t len = strlen(str);

    in->buf = strdup(str);
    in->len = len;
    in->cur = len;
}
/**
 * gets the current history element
 * and then increses the index
 */
void next_entry(struct InputState* state){
    char* new = get_history(L, state->index);

    set_string(&state->in, new);
    char* next = get_history(L, state->index + 1);
    if (next == NULL)
        return;
    free(next);

    state->index++;
}

/**
 * decreses the index and then gets the element
 */
void prev_entry(struct InputState* state){
    if (state->index == 0) {
        copy_string(&state->in, "");
        return;
    }
    if (state->in.buf != NULL) {
        free(state->in.buf);
        state->in.buf = NULL;
    }
    char* new = get_history(L, state->index - 1);
    set_string(&state->in, new);
    state->index--;
}

void handle_ctrl(struct InputState* in, char c){
    if(c == 0x08 || c == 0x7f){ // backspace
        delete_char(&in->in);
        return;
    }

    if(c == '\n'){ // newline
        in->cont = false;
        return;
    }


    char buffer[1024] = {};
    if (c == '\e') {
        int r = read(STDIN_FILENO, buffer, 1024);
        if (r < 0)
            return;
    }

    if (buffer[0] == '[') {
        switch (buffer[1]) {
            // up
            case 'A':
                next_entry(in);
            break;
            // down
            case 'B':
                prev_entry(in);
            break;
            case 'C': // right
                if(in->in.cur < in->in.len){
                    in->in.cur++;
                }
                return;
                break;
            case 'D': // left
                if(in->in.cur > 0){
                    in->in.cur--;
                }
                return;
            break;
        }
    
    }


    return;
}

/**
 * returns input string when enter is pressed
 */
char* interactive_input(){
    struct InputState state = { };
    state.cont = true;
    state.index = 0;

    // read a character
    int c = 0;
    // if input is done stop procesing text
    while (state.cont) {
        int len = read(STDIN_FILENO, &c, 1);
        if (len <= 0) {
            continue;
        }
        if (iscntrl(c)) {
            handle_ctrl(&state, c);
        }
        else{
            insert_char(&state.in, c);
        }

        render_input(&state.in);
    }
    char* buffer = realloc(state.in.buf, state.in.len + 1);
    buffer[state.in.len] = 0;
    printf("%s\n", buffer);

    return buffer;
}


#ifdef TEST

void print_string(const struct Input* const in){
    for (size_t i = 0; i < in->len; ++i) {
        if(i == in->cur){
            printf("\e[48;2;0;128;255m%c\e[0m", in->buf[i]);
        }
        else{
            printf("%c", in->buf[i]);
        }
    }
    if (in->cur == in->len) {
        printf("\e[48;2;0;128;255m \e[0m");
    }
    printf("(%lu %lu %lu)\n", in->cur, in->len, in->cap);
}

void test_input(){
    struct Input in = {};
#define TEST_STRING "test string"
#define TARGET_STRING "test string"
    for(size_t i = 0; i < sizeof(TEST_STRING) - 1; ++i){
        insert_char(&in, TEST_STRING[i]);
    }
    print_string(&in);
    if (strncmp(in.buf, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.len, in.buf, TARGET_STRING);
        exit(-1);
    }

// first test
#undef TARGET_STRING
#define TARGET_STRING "test stringa"
    printf("[TEST 1]: adding another char\n");
    insert_char(&in, 'a');
    print_string(&in);
    if (strncmp(in.buf, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.len, in.buf, TARGET_STRING);
        exit(-1);
    }

// second test
#undef TARGET_STRING
#define TARGET_STRING "test string"
    printf("[TEST 2]: removing last char\n");
    delete_char(&in);
    print_string(&in);
    if (strncmp(in.buf, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.len, in.buf, TARGET_STRING);
        exit(-1);
    }

    insert_char(&in, 'a');
    print_string(&in);

#undef TARGET_STRING
#define TARGET_STRING "taest string"
    printf("[TEST 3]: inserting a char in at index 1\n");
    in.cur = 1;

    insert_char(&in, 'a');
    print_string(&in);

    if (strncmp(in.buf, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.len, in.buf, TARGET_STRING);
        exit(-1);
    }

#undef TARGET_STRING
#define TARGET_STRING "ataest string"
    printf("[TEST 4]: inserting a char at beg\n");
    in.cur = 0;

    insert_char(&in, 'a');
    print_string(&in);

    if (strncmp(in.buf, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.len, in.buf, TARGET_STRING);
        exit(-1);
    }

    free(in.buf);
}
#endif
