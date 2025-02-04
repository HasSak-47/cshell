#include <lauxlib.h>
#include <lua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <shell.h>
#include <str.h>

// TODO: use char* instead of struct String

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

void set_conf_variables(){
    printf("setting config\n");
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_pushstring(L, init_path);
    lua_setfield(L, -2, "init_path");

    lua_pushstring(L, config_path);
    lua_setfield(L, -2, "config_path");

    lua_pushstring(L, hot_path);
    lua_setfield(L, -2, "hot_path");

    lua_pop(L, lua_gettop(L));
}

void update_config_variables(){
    free(init_path);
    free(config_path);
    free(hot_path);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_getfield(L, -1, "init_path");
    if(lua_isstring(L, -1) == LUA_OK){
        init_path = strdup(lua_tostring(L, -1));
    }
    else{
        init_path = strdup(INIT_PATH);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "config_path");
    if(lua_isstring(L, -1) == LUA_OK){
        config_path = strdup(lua_tostring(L, -1));
    }
    else{
        config_path = strdup(CONFIG_PATH);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "hot_path");
    hot_path = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);



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

void set_inital_setup(){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "setup");
    lua_call(L, 0, 0);
}
