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

        // TODO: here goes updating the env
        // it is probably an expensive thing to check the table and the __environ
        // and it should be done with and Luall.api.set_env and Luall.api.unset_env
        // but it is more fun this way
        // the Luall.vars.env is only set at load so it is ready to do that
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
