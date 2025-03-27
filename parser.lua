
---@alias TokenType "statement"
---| "process"
---| "pipe"
---| "redir"
---| "fd"
---| "str"
---
---@alias Token {val: any, type: TokenType}


-- <statement> ::= <process> ( (<pipe> | <redirection> (<fd> | <string>)) <process> )*
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

---@param tokens string[]
---@return Token
local function take_process(tokens)
    local i = 0
    for idx, token in ipairs(tokens) do
        if pipe_set[token] ~= nil then
            i = idx
            break
        end
        if redir_set[token] ~= nil then
            i = idx
            break
        end
        if fd_set[token] ~= nil then
            i = idx
            break
        end
    end

    local process = {}
    for _ = 1, i, 1 do
        local str = table.remove(tokens, 1)
        table.insert(process, {val=str, type="string"})
    end

    return {val=process, type="process"}
end

local function take_statement(tokens)
    return {val= {take_process(tokens)}, type= "statement"}

end

---@return Token
local function tokenize(input)
    local i = 1
    local len = #input
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
            table.insert(tokens, input:sub(start + 1, i - 2));
        elseif not char:match("%s") then
            local start = i
            while i <= len and not input:sub(i, i):match("%s") do
                i = i + 1
            end
            table.insert(tokens, input:sub(start, i - 1))
        else
            i = i + 1
        end
    end

    return tokens
end

local function parser()
    error('in testing...');
end

---@param t Token[]
---@param depth number
local function print_tokens(t, depth)
    for _, value in ipairs(t) do
        local tab = string.rep("\t", depth)
        if value.type == "str" then
            goto bottom
        end
        if value.type == "pipe" then
            goto bottom
        end
        if value.type == "redir" then
            goto bottom
        end

            print(tab .. value.type)
            print_tokens(value.val, depth + 1)
        goto continue

        ::bottom::
            print(tab .. 'val: "' .. value.val .. '" ' .. value.type)
        goto continue

        ::continue::
    end
end

--- testing function
local function main()
    local tokens = tokenize("echo 'test this out' this is wack >> out.txt");
    local process = take_process(tokens)
    print_tokens({process}, 0)
end

if not package.loaded[...] then
    main()
else
    -- File is being required as a module
    -- No action needed
end

return parser
