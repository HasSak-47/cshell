#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    // set initial settings
    init_path = malloc(sizeof(INIT_PATH));
    strncpy(init_path, INIT_PATH, sizeof(INIT_PATH) - 1);
    init_path[sizeof(INIT_PATH) - 1] = 0;

    config_path = malloc(sizeof(CONFIG_PATH));
    strncpy(config_path, CONFIG_PATH, sizeof(CONFIG_PATH) - 1);
    config_path[sizeof(CONFIG_PATH) - 1] = 0;

    hot_path = malloc(sizeof(HOT_PATH));
    strncpy(hot_path, HOT_PATH, sizeof(HOT_PATH) - 1);
    hot_path[sizeof(HOT_PATH) - 1] = 0;

    load();
    set_conf_variables();
    setup();

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
            setup();
            reload = false;
        }
        update_variables();
    }
    unload();
    free(init_path);
    free(config_path);
    free(hot_path);
    return 0;
}
