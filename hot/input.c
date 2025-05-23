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

DefineVector(String, char)

struct Input{
    struct VectorString str;
    size_t cur; // position of the cursor inside the string 
};

void insert_char(struct Input* in, char c){
    vector_push(in->str, '\0');
    char prev = c;
    for (size_t i = in->cur; i < in->str.len; ++i) {
        char aux = in->str.data[i];
        in->str.data[i] = prev;
        prev = aux;
    }
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
    for (size_t i = in->cur - 1; i < in->str.len; ++i) {
        in->str.data[i] = in->str.data[i + 1];
    }
    vector_pop(in->str);
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
    if (in->str.len == 0) {
        putchar(' ');
        printf("\e[D");
        fflush(stdout);
        return;
    }
    for (size_t i = 0; i < in->str.len; ++i)
        putchar(in->str.data[i]);
    
    printf("\e[%luD", in->str.len);

    if (in->cur > 0)
        printf("\e[%luC", in->cur);

    fflush(stdout);
    if (in->cur > 0)
        printf("\e[%luD", in->cur);

    for (size_t i = 0; i < in->str.len; ++i)
        putchar(' ');

    printf("\e[%luD", in->str.len);
}

/**
 * takes an string and makes it the buffer of input
 */
void set_string(struct Input* in, char* str){
    free(in->str.data);
    size_t len = strlen(str);

    in->str.data = str;
    in->str.len = len;
    in->cur = len;
}

/**
 * takes an string and clones it to make it the buffer of input
 */
void copy_string(struct Input* in, char* str){
    free(in->str.data);
    size_t len = strlen(str);

    in->str.data = strdup(str);
    in->str.len = len;
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
    if (state->in.str.data != NULL) {
        free(state->in.str.data);
        state->in.str.data = NULL;
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
                if(in->in.cur < in->in.str.len){
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
    char* buffer = realloc(state.in.str.data, state.in.str.len + 1);
    buffer[state.in.str.len] = 0;
    printf("%s\n", buffer);

    return buffer;
}


#ifdef TEST

void print_string(const struct Input* const in){
    for (size_t i = 0; i < in->str.len; ++i) {
        if(i == in->cur){
            printf("\e[48;2;0;128;255m%c\e[0m", in->str.data[i]);
        }
        else{
            printf("%c", in->str.data[i]);
        }
    }
    if (in->cur == in->str.len) {
        printf("\e[48;2;0;128;255m \e[0m");
    }
    printf("(%lu %lu %lu)\n", in->cur, in->str.len, in->str.cap);
}

void test_input(){
    struct Input in = {};
#define TEST_STRING "test string"
#define TARGET_STRING "test string"
    for(size_t i = 0; i < sizeof(TEST_STRING) - 1; ++i){
        insert_char(&in, TEST_STRING[i]);
    }
    print_string(&in);
    if (strncmp(in.str.data, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.str.len, in.str.data, TARGET_STRING);
        exit(-1);
    }

// first test
#undef TARGET_STRING
#define TARGET_STRING "test stringa"
    printf("[TEST 1]: adding another char\n");
    insert_char(&in, 'a');
    print_string(&in);
    if (strncmp(in.str.data, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.str.len, in.str.data, TARGET_STRING);
        exit(-1);
    }

// second test
#undef TARGET_STRING
#define TARGET_STRING "test string"
    printf("[TEST 2]: removing last char\n");
    delete_char(&in);
    print_string(&in);
    if (strncmp(in.str.data, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.str.len, in.str.data, TARGET_STRING);
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

    if (strncmp(in.str.data, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.str.len, in.str.data, TARGET_STRING);
        exit(-1);
    }

#undef TARGET_STRING
#define TARGET_STRING "ataest string"
    printf("[TEST 4]: inserting a char at beg\n");
    in.cur = 0;

    insert_char(&in, 'a');
    print_string(&in);

    if (strncmp(in.str.data, TARGET_STRING, strlen(TARGET_STRING)) != 0){
        printf("error \"%*s != %s\"\n", (int)in.str.len, in.str.data, TARGET_STRING);
        exit(-1);
    }

    free(in.str.data);
}
#endif
