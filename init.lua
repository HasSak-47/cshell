
local function first_pass(input)
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

Luall = {
	values = {
		host = "",
		cwd  = "",
		user = {
			name = "",
			home = "",
		},
		error = 0,
        debug = false,
	},
	api = {},
	inner = {
		parse = function(input)
            if input == "exit" then
                Luall.api.exit()
            end
            local tokens = first_pass(input)
            local args = {}
            for _, token in pairs(tokens) do
                table.insert(args, token.val)
            end
            local cmd = table.remove(args, 1)
            if cmd == "exit" then
                Luall.api.exit()
            elseif cmd == "cd" then
                Luall.api.cd(cmd)
            elseif cmd == "reload" then
                Luall.api.reload()
            elseif cmd == "lua_info" then
                Luall.api.lua_info()
            elseif type(cmd) ~= "nil" then
                Luall.api.execv(cmd, table.unpack(args))
            else
                print(cmd)
            end
		end
	},
    prompts = {
        prompt = function()
            local vals = Luall.values;
            local user = vals.user;
            return user.name .. "@" .. vals.host .. vals.cwd .. ">"
        end,
        right_prompt = function() return "" end,
        greeting = function() return "" end,
    }
}
