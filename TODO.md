# Utils
Add generic vectors and rewrite resizeble arrays into those

# UX
add autocompletion and handle right and left cursor movement

# Binding
Make a luabinder from c
```c
//@luabind {metatable} Foo
struct Foo{
    //@luabind get set
    int bar;

    //@luabind get
    int zad;
};

//@luabind {metatable Foo} Foo.new
struct Foo new();
```

```c
//@luabind {mt} Foo
struct Foo{
    //@luabind get set
    int bar;

    //@luabind get
    int zad;
};

//@luabind {mt Foo} new
struct Foo foo_new(const char* name);

void _lua_bind_new(lua_State* L){
    if(!lua_isstring(L, 1)){
        lua_pushstring(L, "An error occurred");
        return lua_error(L);
    }
    const char* arg_name = lua_tostring(L, 1);
    struct Foo* table = (struct Foo*)lua_newuserdata(L, sizeof(struct Foo));
    *table = foo_new();
    return 1;
}

struct luaL_Reg = {
    {"new", _lua_bind_new},
    NULL, NULL,
};
```

## history
fix memory leak

# Lua API
## Pipes
Create a pipe interface in Luall.api
modify so you can bind more than one pipe

# Parsing
Finish pipe parser

