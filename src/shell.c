#include <lualib.h>
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

#include <shell.h>
#include <str.h>
#include <cmds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

lua_State* L = NULL;

void prompt(){
    lua_getglobal(L, "Luall");
        lua_getfield(L, -1, "prompts");
            lua_getfield(L, -1, "prompt");
    if (lua_isfunction(L, -1)) {
        int error = lua_pcall(L, 0, 1, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            printf("fallback >");
            return;
        }

        const char* va = lua_tostring(L, -1);
        printf("%s", va);
    }
    lua_pop(L, 2);
}

void parse(const char* prompt){
    lua_getglobal(L, "Luall");
        lua_getfield(L, -1, "api");
            lua_getfield(L, -1, "inner");
	            lua_getfield(L, -1, "parse");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, prompt);

        int error = lua_pcall(L, 1, 1, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            printf("fallback >");
            return;
        }

        const char* va = lua_tostring(L, -1);
        printf("return: %s", va);
    }
}

void shell(){
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
        printf("sending to lua...\n");
        parse(input);
        free(input);
    }
}

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

void update_values(){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "values");

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

    lua_pop(L, 2);
}

static void init_cmds(){
    lua_getglobal(L, "Luall");
        lua_getfield(L, -1, "api");
    lua_pushcfunction(L, cmd_cd);
    lua_setfield(L, -2, "cd");

    lua_pushcfunction(L, cmd_exit);
    lua_setfield(L, -2, "exit");
    lua_pop(L, 1);
}

void load(){
    L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, "./init.lua") != LUA_OK){
        luaL_error(L, "Could not init the Luall: %s\n", lua_tostring(L, -1));
    }
    update_values();
    init_cmds();
}

void unload(){
    lua_close(L);
}

int last_return_code = 0;
bool run = true;
