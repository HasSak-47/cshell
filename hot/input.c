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
            // don't want to handle this
            // so just kill self
            exit(-1);
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
    if (in->cur == 0)
        return;
    for (size_t i = in->cur - 2; i < in->len - 1; ++i) {
        in->buf[i] = in->buf[i + 1];
    }
    in->len--;
    in->cur--;
}

struct InputState{
    bool cont; // should continue
    struct Input in;
};


void handle_ctrl(struct InputState* in, char c){
    if(c == 0x08 || c == 0x7f){ // backspace
        delete_char(&in->in);
        if (in->in.len <= 0) {
            return;
        }
        write(STDOUT_FILENO, "\b \b", 3);
        fflush(stdout);
        return;
    }

    if(c == '\n'){ // newline
        putchar(c);
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
            case 'A': // up
                return;
            case 'B': // down
                return;
            case 'C': // right
                if(in->in.cur < in->in.len){
                    printf("\e[C");
                    fflush(stdout);
                    in->in.cur++;
                }
                return;
            case 'D': // left
                if(in->in.cur > 0){
                    printf("\e[D");
                    fflush(stdout);
                    in->in.cur--;
                }
                return;
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

    // read a characer
    int c = 0;
    // if input is done stop procesing text
    while (state.cont) {
        int len = read(STDIN_FILENO, &c, 1);
        if (len <= 0) {
            continue;
        }
        if (iscntrl(c)) {
            fflush(stdout);
            handle_ctrl(&state, c);
            continue;
        }

        insert_char(&state.in, c);
        for (size_t i = state.in.cur- 1; i < state.in.len; ++i) {
            printf("%c", state.in.buf[i]);
        }
        fflush(stdout);
    }
    char* buffer = realloc(state.in.buf, state.in.len + 1);
    buffer[state.in.len] = 0;

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
    for(size_t i = 0; i < sizeof(TEST_STRING) - 1; ++i){
        insert_char(&in, TEST_STRING[i]);
    }
    print_string(&in);

    printf("inserting and deleting at end\n");
    insert_char(&in, 'a');
    print_string(&in);

    delete_char(&in);
    print_string(&in);

    insert_char(&in, 'a');
    print_string(&in);

    in.cur = in.len / 2;

    print_string(&in);

    insert_char(&in, 'a');
    print_string(&in);

    in.cur = 0;
    insert_char(&in, 'a');
    print_string(&in);

    delete_char(&in);
    delete_char(&in);
    delete_char(&in);
    delete_char(&in);
    delete_char(&in);
    print_string(&in);
    insert_char(&in, 'a');
    print_string(&in);

    free(in.buf);
}
#endif
