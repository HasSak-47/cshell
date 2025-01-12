#include <shell.h>
#include <stdio.h>

int main(){
    printf("loading lua...\n");
    load();

    printf("starting shell...\n");
    shell();

    printf("ending program...\n");
    unload();

    return 0;
}
