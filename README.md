# luall
```lua
luall = {
    run = function(cmd),
    -- exec
    cmd = function(cmd, args, env), -- first search for it in api then bin
    api = function(cmd, args, env),
    bin = function(cmd, args, env),

    add_env = function(var, val)
}
```

# simple
`cd /foo/bar` is eq to
```lua
luall.cmd("cd", {"/foo/bar"}, {}).run()

```

# more complex
`cat foo > bar` is eq to
```lua
-- api version
luall.api.pipe_file("cat foo", "bar")
-- non api version
local stdout = luall.cmd("cat", {"foo"}).capture(luall.stdout).run().stdout()
local file = luall.create_file("bar")
file.write("bar")
```

# more more complex
`RUST_ANALYZER=1 cargo run` is eq
```lua
luall.cmd("cargo", {"run"}, {{"RUST_ANALYZER", "1"}}).run()
```
Command: cmd (arg)\*
TempEnv: \[ENV(=VAL)?\]* 
Pipe: > | >> | \|
Statement: (TempEnv)* Command (Pipe Command)*
