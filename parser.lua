
---@alias TokenType "statement"
---| "process"
---| "command"
---| "argument"
---| "pipe"
---| "pipe-process"
---| "redir"
---| "fd"
---| "str"
---| "undefined"
---
---@alias Token {type: TokenType, val: (string | Token), }
---
---@alias Command  {span: {[1]: number, [2]: number}, val: string, type: "command"}
---@alias Argument {span: {[1]: number, [2]: number}, val: string, type: "argument", subtype: "path" | "string" | "ws_string"}
---@alias Process  {span: {[1]: number, [2]: number}, val: {[1]: Command, [2]: Argument}, type: "process"}
---@alias Pipe     {span: {[1]: number, [2]: number}, val: "|" | "|&", type: "pipe"}

-- <statement> ::= <process> (<pipe>  <process> )* (<redirection> (<fd> | <string>))
-- <process>   ::= <string> (<string>)*
-- <pipe>      ::= "|" | "|&"
-- <redirection> ::= "<" | ">" | ">>" | "<<"
-- <fd>        ::= "&0" | "&1" | "&2"
-- <string>    ::= '"' [^"]* '"'
-- | "'" [^']* "'"
-- | [^"' \t\n|&<>]+


local pipe_operators = {
    "|",
    "|&",
}

local redir_operators = {
    "<",
    "<<",
    ">",
    ">>",
}

local filedesc = {
    "&0",
    "&1",
    "&2",
}

---@param t table
local function setify_table(t)
    local s = {};
    for _, value in ipairs(t) do
        s[value] = true
    end
    return s;
end

local pipe_set, redir_set, fd_set = setify_table(pipe_operators), setify_table(redir_operators), setify_table(filedesc)

---@param tokens Token[]
---@return Process
local function take_process(tokens)
    local i = 0
    for idx, token in ipairs(tokens) do
        if pipe_set[token.val] ~= nil then
            i = idx
            break
        end
        if redir_set[token.val] ~= nil then
            i = idx
            break
        end
        if fd_set[token.val] ~= nil then
            i = idx
            break
        end
    end

    ---@type Token[]
    local process = {}
    if i == 0 then
        i = #tokens
    else
        i = i - 1
    end

    for _ = 1, i, 1 do
        local tkn = table.remove(tokens, 1)
        tkn.type = "argument"
        table.insert(process, tkn)
    end
    process[1].type = "command"

    return {val=process, type="process"}
end

---@return Token[]
local function tokenize(input)
    local i = 1
    local len = #input
    ---@type Token[]
    local tokens = {}

    while i <= len do
        local char = input:sub(i, i)

        if char == '"' or char == "'" then
            -- Handle quoted strings
            local quote = char
            local start = i
            i = i + 1
            while i <= len and input:sub(i, i) ~= quote do
                i = i + 1
            end
            if i <= len then
                i = i + 1
            end
            -- remove quotes
            table.insert(tokens, {span={start, i-1}, val=input:sub(start + 1, i - 2), type="str"});
        elseif not char:match("%s") then
            local start = i
            while i <= len and not input:sub(i, i):match("%s") do
                i = i + 1
            end
            table.insert(tokens, {span={start, i-1}, val=input:sub(start, i - 1), type="undefined"})
        else
            i = i + 1
        end
    end

    for _, token in ipairs(tokens) do
        if pipe_set[token.val] ~= nil then
            token.type = "pipe"
        elseif redir_set[token.val] ~= nil then
            token.type = "redir"
        elseif fd_set[token.val] ~= nil then
            token.type = "fd"
        else
            token.type = "str"
        end
    end

    local statement = {val = {take_process(tokens), }, type="statement"}

    while #tokens > 0 do
        if tokens[1].type == "pipe" then
            local pipe = table.remove(tokens, 1)
            local process = take_process(tokens)

            table.insert(statement.val, pipe)
            table.insert(statement.val, process)
            goto continue
        end

        if tokens[1].type == "redir" then
            local redir = table.remove(tokens, 1)
            local target = table.remove(tokens, 1)

            table.insert(statement.val, redir)
            table.insert(statement.val, target)

        end

        ::continue::
    end

    return {statement}
end


---@param t Token[]
---@param depth number
local function print_tokens(t, depth)
    for _, value in ipairs(t) do
        local tab = string.rep("\t", depth)
        if type(value.val) == "string" then
            print(tab .. value.type .. ': "' .. value.val .. '"')
        else
            print(tab .. value.type)
            print_tokens(value.val, depth + 1)
        end

    end

end


--- @param var string
local function parse_env(var)
    local result = {}
    for part in string.gmatch(var, "([^:]+)") do
        table.insert(result, part)
    end
    return result
end

---@param cmd Process
local function run_cmd(cmd)

    if cmd.type == nil then
        error('did not got a token!');
    end
    if cmd.type ~= "process" then
        error('wrong type expected process got' .. cmd.type);
    end

    local name = table.remove(cmd.val, 1).val
    local args = {}
    for _, val in ipairs(cmd.val) do
        table.insert(args, val.val)
    end

    if Luall.vars.debug then
        local s = 'running cmd: ' .. name
        for _, value in ipairs(args) do
            s = s .. ' ' .. value
        end

        print(s)
    end

    if Luall.vars.debug then
        print('selecting type')
    end
    if Luall.alias[name] ~= nil then
        if Luall.vars.debug then
            print('cmd ' .. name .. ' is an alias')
        end
        local b = Luall.alias[name](args)
        name = b[1]
        args = b[2]
        if Luall.vars.debug then
            print('new name:', name)
            for _, arg in ipairs(args) do
                print('\t', arg)
            end
        end
    end

    if Luall.extend[name] ~= nil then
        Luall.extend[name](args)
        return
    end
    if Luall.util[name] ~= nil then
        Luall.util[name](args)
        return
    end
    if Luall.api[name] ~= nil then
        Luall.api[name](args)
        return
    end

    if Luall.vars.debug then
        print('running exec...')
    end
    -- check all possible dirs in which cmd could be
    local possible_locs = parse_env(Luall.vars.env.PATH)
    local target_cmd = ''
    for _, path in pairs(possible_locs) do
        local candidate = path ..'/' .. name
        if Luall.api.exists(candidate) then
            target_cmd = candidate
            break
        end
    end

    if Luall.vars.debug then
        print('target cmd:', target_cmd)
        for _, arg in ipairs(args) do
            print('\t', arg)
        end
    end
    Luall.extend.exec(target_cmd, table.unpack(args))
    
end

---@param tokens {[1]: Process, [2]: Pipe, [3]: Process}
local function run_piped(tokens)
    if Luall.vars.debug then
        print_tokens(tokens, 0)
    end

    local src = tokens[1]
    local _pipe = tokens[2]
    local out = tokens[3]

    if src.type ~= "process" then
        print ("first token isn't process")
    end
    if _pipe.type ~= "pipe" then
        print ("second token isn't pipe")
    end
    if out.type ~= "process" then
        print("third token isn't process")
    end

    ---@param p Process
    local function build_process(p)
        local name = table.remove(p.val, 1).val
        local args = {}
        for _, val in ipairs(p.val) do
            table.insert(args, val.val)
        end

        local possible_locs = parse_env(Luall.vars.env.PATH)
        local target_cmd = ''
        for _, path in pairs(possible_locs) do
            local candidate = path ..'/' .. name
            if Luall.api.exists(candidate) then
                target_cmd = candidate
                break
            end
        end
        return Luall.api.process.new(target_cmd, table.unpack(args));
    end

    local src_p = build_process(src)
    local pipe = Luall.api.pipe.new()
    local out_p = build_process(out)
    if src_p == nil then
        print('failed to create src process!')
    end
    if out_p == nil then
        print('failed to create out process!')
    end

    Luall.api.process.bind_pipe(src_p, pipe, "out")
    Luall.api.process.bind_pipe(out_p, pipe, "in")

    local src_pid = Luall.api.process.run(src_p)
    local out_pid = Luall.api.process.run(out_p)
    Luall.api.pipe.close(pipe)

    Luall.api.process.wait(src_pid)
    Luall.api.process.wait(out_pid)

end

---@param cmd string
local function handle_shell_like(cmd)
    local tokens = tokenize(cmd)
    if Luall.vars.debug then
        print('handling shell like:')
        print_tokens(tokens, 0)
    end

    --- check if the token list is just 1
    --- if there is more than 2 it means there is a pipe there
    if #tokens[1].val == 1 then
        run_cmd( tokens[1].val[1] )
    else
        run_piped( tokens[1].val )
    end
end

---@param line string
local function handle_singleline(line)
    local start  = line:find('lua')
    if start == 1 then
        ---TODO: change it so it only takes the first lua
        line = line:gsub('lua', '')
        Luall.util.lua({line})
    else -- run "normal shell"
        handle_shell_like(line)
    end
end

---@param input string
local function parser(input)
    local set_debug = input:sub(1, 1) == '!';
    if set_debug then
        input = string.sub(input, 2)
        Luall.api.set_debug()
    end
    if input == nil or input == '' then
        return;
    end

    local lines = {}
    local i = 1
    -- split into lines
    for line in input:gmatch('[^\n]*') do
        if line == '' then
            goto continue
        end
        lines[i] = line
        i = i + 1
        ::continue::
    end

    if #lines == 1 then -- single line
        handle_singleline(lines[1])
    else -- multi line
    end

    if set_debug then
        print('unset debug')
        Luall.api.unset_debug()
    end
end

local function test_run_cmd()
    print('parser.test_run_cmd')
    local tokens = tokenize('echo hello this is echo')
    print_tokens(tokens, 0)
    print()

    run_cmd( tokens[1].val[1] )
end

local function test_pipes()
    print(Luall.inner.full_color(64, 255, 128) .. 'parser.test_pipes' .. Luall.inner.reset_color())
    local ls = Luall.api.process.new('/bin/ls', '-lA')
    local wc = Luall.api.process.new('/bin/wc', '-l')
    local pipe = Luall.api.pipe.new()

    Luall.api.process.bind_pipe(ls, pipe, "out")
    Luall.api.process.bind_pipe(wc, pipe, "in")


    local ls_pid = Luall.api.process.run(ls)
    local wc_pid = Luall.api.process.run(wc)

    Luall.api.pipe.close(pipe)

    Luall.api.process.wait(ls_pid)
    Luall.api.process.wait(wc_pid)


end

local function config()
    table.insert(Luall.testing, test_run_cmd )
    table.insert(Luall.testing, test_pipes )
end

return {
    parser=parser, tokenize=tokenize, config=config}
