

---@param input string
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
            table.insert(tokens, {val = input:sub(start + 1, i - 2), type = "string"})
        elseif not char:match("%s") then
            local start = i
            while i <= len and not input:sub(i, i):match("%s") do
                i = i + 1
            end
            table.insert(tokens, {val = input:sub(start, i - 1), type = "undetermined"})
        else
            i = i + 1
        end
    end

    return tokens
end

---@param line string
local function handle_shelllike(line)
    local tokens = tokenize(line)
    ---@type string[]
    local args = {}
    for _, token in pairs(tokens) do
        table.insert(args, token.val)
    end

    local cmd = table.remove(args, 1)
    if cmd == nil then
        -- Luall.api.set_error(-1)
        return
    end

    if Luall.util[cmd] ~= nil then
        Luall.util[cmd](args)
    elseif Luall.overwrite[cmd] ~= nil then
        Luall.overwrite[cmd](args)
    elseif Luall.api[cmd] ~= nil then
        if #args == 0 then
            Luall.api[cmd]()
        else
            Luall.api[cmd](table.unpack(args))
        end
    else
        Luall.inner.exec(cmd, args)
    end
end

---@param line string
local function handle_singleline(line)
    local start  = line:find('lua')
    if start == 1 then
        line = line:gsub('lua', '')
        Luall.util.lua({line})
    else -- run "normal shell"
        handle_shelllike(line)
    end
end

---@param input string
local function parser (input)
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

    if #lines then -- single line
        handle_singleline(lines[1])
    else -- multi line
    end
end

return parser
