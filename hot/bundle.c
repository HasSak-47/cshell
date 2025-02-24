#include <ctype.h>
#include <state.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>

static int api_exit(lua_State* L){
    running = false;
    return 0;
}

static int api_reload(lua_State* L){
    reload = true;
    return 0;
}

static int api_cd(lua_State* L){
    const char* path = lua_tostring(L, -1);
    if( path == NULL ){
        return 0;
    }
    error = chdir(path);
    return 0;
}

static int api_exists(lua_State* L){
    const char* path = lua_tostring(L, -1);
    struct stat statm;
    if(stat(path, &statm) == 0)
        lua_pushboolean(L, true);
    else
        lua_pushboolean(L, false);
    return 1;
}

// NOTE: this should not be run with valgrind!!
static int api_exec(lua_State* L){
    const size_t argc = lua_gettop(L);
    char** args = malloc((argc + 1) * sizeof(char*));
    // args must be null terminated
    args[argc] = NULL;

    for(size_t i = 0; i < argc; ++i){
        // lua indices start at 1 for some fucking reason
        const char* arg = lua_tostring(L, i + 1);
        // something weird was passed as an argument just ignore
        if(arg == NULL)
            continue;
        args[i] = strdup(arg);
    }
    const char* cmd = args[0];

    // if the cmd does not exists just don't execute it
    // if I fork() for an unexisting path some reason the termios resets
    // and I don't need it to do that
    struct stat statbuf;
    if(stat(cmd, &statbuf) == 0){
        pid_t pid = fork();
        if(pid == 0) { // child
            execv(cmd, args);
        }
        else if(pid > 0){ // parent
            waitpid(pid, &error, 0);
        }
        else { // not found
            printf("could not exec\n");
            error = -1;
        }
    }
    else{
        printf("binary not found!");
    }

    for(size_t i = 0; i < argc; ++i)
        free(args[i]);
    free(args);

    return 0;
}

#define REG(name) { #name, api_##name }

luaL_Reg api[] = {
    REG(cd),
    REG(exit),
    REG(reload),
    REG(exec),
    REG(exists),
    {NULL, NULL},
};

void update_lua_state(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

    lua_pushinteger(L, error);
    lua_setfield(L, -2, "error");

    lua_pushstring(L, host);
    lua_setfield(L, -2, "host");

    lua_pushstring(L, cwd);
    lua_setfield(L, -2, "cwd");

    lua_getfield(L, -1, "user");

    lua_pushstring(L, user.home);
    lua_setfield(L, -2, "home");

    lua_pushstring(L, user.name);
    lua_setfield(L, -2, "name");
    lua_pop(L, 2);
}


void prompt(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "prompts");
    lua_getfield(L, -1, "prompt");
    lua_call(L, 0, 1);
    const char* prompt = lua_tostring(L, -1);
    printf("%s", prompt);
}

char* read_keyboard(){
    char* input = malloc(64);
    size_t len = 0; 
    size_t cap = 64;

    // read
    int c = 0;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if(c == '\n'){
            putchar('\n');
            break;
        }
        // backspace
       if(c == 0x08 || c == 0x7f){
            if(len == 0)
                continue;

            len--;
            write(STDOUT_FILENO, "\b \b", 3);
            fflush(stdout);
            continue;
        }
        if(!isprint(c)) continue;

        printf("%c", c);
        // write(STDOUT_FILENO, &c, 1);
        fflush(stdout);

        // resize if necessary
       if(len + 1 == cap){
           cap *= 2;
           void* aux = realloc(input, cap);
           if(aux == NULL){
               // just nuke it
               exit(-1);
           }
           input = aux;
       }
       input[len++] = c;
    }

    // make the input null terminated
    void* aux = realloc(input, len + 1);
    if (aux == NULL) 
        exit(-1);
    input = aux;
    input[len++] = 0;

    return input;
}


// WARN: this is a bad name for the function
void handle_input(lua_State* L){
    update_lua_state(L);
    prompt(L);

    // since it is raw mode make sure to print the prompt
    fflush(stdout);

    char* input = read_keyboard();

    // parse the function
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "inner");
    lua_getfield(L, -1, "parse");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, input);

        int error = lua_pcall(L, 1, 1, 0);
        if(error != LUA_OK){
            luaL_error(L, "Prompt Error: %s\n", lua_tostring(L, -1));
            return;
        }
    }
    else{
        printf("error Luall.inner.parse is not a function");
    }
    lua_pop(L, 2);

    free(input);
}

static int __env_index(lua_State* L){
    const char* okey = lua_tostring(L, -1);
    if(okey == NULL){
        lua_pushnil(L);
        return 1;
    }

    size_t len = strlen(okey);
    char* key = malloc(strlen(okey) + 1);
    for(size_t i = 0; i < len; ++i)
        key[i] = toupper(okey[i]);
    key[len] = 0;

    const char* value = getenv(key);
    if(value == NULL)
        lua_pushnil(L);
    else
        lua_pushstring(L, value);
    free(key);

    return 1;
}

static int __env_newindex(lua_State* L){
    const char* okey = lua_tostring(L, -2);
    if(okey == NULL)
        return 0;

    size_t len = strlen(okey);
    char* key = malloc(strlen(okey) + 1);
    for(size_t i = 0; i < len; ++i)
        key[i] = toupper(okey[i]);
    key[len] = 0;

    if(lua_isnil(L, -1)){
        unsetenv(key);
        return 0;
    }
    const char* value = lua_tostring(L, -1);
    setenv(key, value, 1);

    return 0;
}

void create_env(lua_State* L){
    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");

    lua_newtable(L);
    luaL_newmetatable(L, "__env");
    lua_pushcfunction(L, __env_index);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, __env_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);

    lua_setfield(L, -2, "env");
    lua_pop(L, 2);
}

void lua_setup(lua_State* L){
    luaL_openlibs(L);

    // load blueprint
    if(luaL_dofile(L, init_path) != LUA_OK){
        // if it doesn't load just nuke it
        running = false;
        return;
    }
    update_lua_state(L);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_pushstring(L, hot_path);
    lua_setfield(L, -2, "hot_path");

    lua_pushstring(L, config_path);
    lua_setfield(L, -2, "config_path");

    lua_pushstring(L, init_path);
    lua_setfield(L, -2, "init_path");

    lua_pop(L, 2);

    create_env(L);

    // blueprint setup
    lua_createtable(L, 4, 4);
    luaL_setfuncs(L, api, 0);
    lua_setfield(L, -2, "api");

    // load user config
    if(luaL_dofile(L, config_path) != LUA_OK){
        // just like before if it doesn't load just nuke it
        running = false;
        return;
    }

    // setup make sure everything in the config
    // that was a relative path is now an absolute path
    lua_getfield(L, -1, "setup");
    if(lua_isfunction(L, -1)){
        lua_call(L, 0, 0);
    }

    // just clean the stack
    lua_pop(L, lua_gettop(L));

    // bring the expanded paths to the global state
    free(hot_path);
    free(config_path);
    free(init_path);

    lua_getglobal(L, "Luall");
    lua_getfield(L, -1, "vars");
    lua_getfield(L, -1, "config");

    lua_getfield(L, -1, "hot_path");
    hot_path = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, "config_path");
    config_path = strdup(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, "init_path");
    init_path = strdup(lua_tostring(L, -1));
    lua_pop(L, lua_gettop(L));


}

void lua_cleanup(lua_State* L){
    // ...
}
