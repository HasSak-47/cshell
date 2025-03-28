
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
---@alias Command {val: string , type: "command"}
---@alias Argument {val: string , type: "argument"}
---@alias Process {val: {[1]: Command, [2]: Argument}, type: "argument"}

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
            table.insert(tokens, {val=input:sub(start + 1, i - 2), type="str"});
        elseif not char:match("%s") then
            local start = i
            while i <= len and not input:sub(i, i):match("%s") do
                i = i + 1
            end
            table.insert(tokens, {val=input:sub(start, i - 1), type="undefined"})
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
    if debug then
        local s = 'running cmd:' .. name
        for _, value in ipairs(args) do
            s = s .. ' ' .. value
        end

        print(s)
    end

    if Luall.overwrite[name] ~= nil then
        Luall.overwrite[name](args)
    elseif Luall.util[name] ~= nil then
        Luall.util[name](args)
    elseif Luall.api[name] ~= nil then
        Luall.api[name](args)
    else

         --- @param var string
        local function parse_env(var)
            local result = {}
            for part in string.gmatch(var, "([^:]+)") do
                table.insert(result, part)
            end
            return result
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
        Luall.api.exec(target_cmd, table.unpack(args))
    end
end

---@param cmd string
local function parser(cmd)
    local tokens = tokenize(cmd)
    print_tokens(tokens, 0)
    print()
    run_cmd( tokens[1].val[1] )
end


return parser
