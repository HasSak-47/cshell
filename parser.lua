
---@alias TokenType "statement"
---| "process"
---| "pipe"
---| "redirection"
---| "fd"
---| "string"
---@alias Statement {1:Process, 2:(Pipe | Redir(Fd | string) Process)[]}
---@alias Process string[]
---@alias Pipe "|"|"|&"
---@alias Redir ">"|">>"|"<"|"<<"
---@alias Fd "&0"|"&1"|"&2"

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

---@param tokens string[]
---@return Process
local function take_process(tokens)
    return {}
end

---@param tokens string[]
---@return Statement
local function take_statement(tokens)
    ---@type Process
    local process = {}
    ---@type Statement
    local statement = {process}
end

---@param input string
local function tokenize(input)
    local i = 1
    local len = #input
    ---@type string[]
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

--- testing function
local function main()
    local tokens = tokenize("echo 'test this out' this is wack >> out.txt");
    for _, val in pairs(tokens) do
        print(val)
        
    end
end

if not package.loaded[...] then
    main()
else
    -- File is being required as a module
    -- No action needed
end

return parser
