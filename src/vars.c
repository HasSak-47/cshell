#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <shell.h>
#include <str.h>

struct String api_getcwd(){
    char* cwd = getcwd(NULL, 0);
    struct String s = from_null_terminated(cwd);
    free(cwd);

    return s;
}

struct String api_gethost(){
    struct String s = {256, malloc(256)};
    gethostname(s.cstr, 256);
    size_t len = strnlen(s.cstr, 256);
    void * aux = realloc(s.cstr, len);
    if(aux == NULL){
        exit(-1);
    }
    s.cstr = aux;
    s.len = len;

    return s;
}

struct User{
    struct String name;
    struct String home;
};

struct User api_getuser(){
    uid_t uid = getuid();
    struct passwd* p = getpwuid(uid);
    struct passwd lp = {};
    memcpy(&lp, p, sizeof(*p));

    struct User u = {};
    u.home = from_null_terminated(lp.pw_dir);
    u.name = from_null_terminated(lp.pw_name);

    return u;
}

void update_variables(){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

    struct String s = api_getcwd();
    char* c = to_null_terminated(s);
    lua_pushstring(L, c);
    lua_setfield(L, -2, "cwd");

    free(s.cstr);
    free(c);

    s = api_gethost();
    c = to_null_terminated(s);
    lua_pushstring(L, c);
    lua_setfield(L, -2, "host");
    free(s.cstr);
    free(c);

    lua_getfield(L, -1, "user");

    struct User u = api_getuser();
    c = to_null_terminated(u.name);
    lua_pushstring(L, c);
    lua_setfield(L, -2, "name");
    free(c);

    c = to_null_terminated(u.home);
    lua_pushstring(L, c);
    lua_setfield(L, -2, "home");
    free(c);

    free(u.name.cstr);
    free(u.home.cstr);
    lua_pop(L, 1);

    lua_pushinteger(L, last_return_code);
    lua_setfield(L, -2, "error");

    lua_pushboolean(L, true);
    lua_setfield(L, -2, "debug");

    lua_pop(L, 2);
}
