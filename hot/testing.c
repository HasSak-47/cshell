#include <testing.h>
#ifdef TEST

#include <stdio.h>

#include <input.h>
#include <process.h>
#include <interface.h>

void __test(lua_State* L){
    printf("testing...\n");
    // test_input();
    test_process();
}
#endif
