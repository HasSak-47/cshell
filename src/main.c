#include <shell.h>

#include <stdio.h>
#include <stdlib.h>

int main(){
    load();
    while(run){
        prompt();

        char* input = NULL;
        size_t len = 0;
        int c = getc(stdin);
        while(c != '\n'){
            void* aux = realloc(input, len + 1);
            if(aux == NULL){
                exit(-1);
            }
            input = aux;
            input[len++] = c;
            c = getc(stdin);
        }

        // make it null terminated
        void* aux = realloc(input, len + 1);
        if(aux == NULL){
            exit(-1);
        }

        input = aux;
        input[len++] = 0;
        parse(input);
        free(input);

        if(reload){
            unload();
            load();
            reload = false;
        }
        update_variables();
    }
    unload();
    return 0;
}
