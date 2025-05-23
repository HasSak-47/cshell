#include <testing.h>

#include <stdio.h>
#include <state.h>
#include <hot.h>

#ifdef TEST
int test(){
    printf("testing...\n");

    printf("initing shell...\n");
    init_shell_state();
    printf("getting current state...\n");
    get_current_state();
    printf("loading...\n");
    load();

    tester(L);

    printf("unloading...\n");
    unload();
    printf("end shell state...\n");
    end_shell_state();


    return 0;
}

#endif
