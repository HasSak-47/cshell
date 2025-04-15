#include <path.h>
#include <ctype.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>
#include <testing.h>

#include <stdio.h>
#include <state.h>
#include <hot.h>

#ifdef TEST

void test_lua(){
    printf("testing lua functions\n");
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "test");
    lua_call(L, 0, 0);
}

void test_strings(){
    const char* str = "/home/user/Document\\ with\\ spaces\\ and\\ \\/slashes";
    struct Path path = parse_path(str);
    char* cpy = get_path_string(path);
    printf("%s == %s", cpy, str);
    free(cpy);
    destruct_path(&path);
}


int test(){
    printf("testing...\n");

    test_strings();

    /*
    printf("initing shell...\n");
    init_shell_state();
    printf("getting current state...\n");
    get_current_state();


    printf("loading...\n");
    load();

    // tester(L);

    // test_lua();


    printf("unloading...\n");
    unload();
    printf("end shell state...\n");
    end_shell_state();
    */


    return 0;
}

#endif
