#include <ctype.h>
#include <state.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

char* interactive_input(){
    char* input = malloc(64);
    size_t len = 0; 
    size_t cap = 64;

    // read
    int c = 0;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if(c == '\n'){
            putchar('\n');
            break;
        }
        // backspace
       if(c == 0x08 || c == 0x7f){
            if(len == 0)
                continue;

            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            fflush(stdout);
            continue;
       }

       if(!isprint(c)) continue;

       printf("%c", c);
       // write(STDOUT_FILENO, &c, 1);
       fflush(stdout);

       // resize if necessary
       if(len + 1 == cap){
           cap *= 2;
           void* aux = realloc(input, cap);
           if(aux == NULL){
               // just nuke it
               exit(-1);
           }
           input = aux;
       }
       input[len++] = c;
    }

    // make the input null terminated
    void* aux = realloc(input, len + 1);
    if (aux == NULL) 
        exit(-1);
    input = aux;
    input[len++] = 0;

    return input;
}
