
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
	vars= {
		host = '',
		cwd  = '',
		user = {
			name = '',
			home = '',
		},
		error = 0,
        debug = false,
        -- special vars that don't get updated at each cycle
        -- changes to env should trigger an env update
        env = {},
        config = {
            init_path = '',
            config_path = '',
            hot_path = '',
        },
	},

    -- this is set up by c
    api = {},
    util = {
        ---@param args table
        lua = function (args)
            if #args == 0 then
                return;
            end

            -- print("exec: " .. args[1])
            local func, _ = load(args[1])
            if func ~= nil then
                pcall(func)
            end
        end
    },
    overwrite = {
        cd = function(args)
            if #args == 0 then
                Luall.api.cd(Luall.vars.user.home)
                print('cd to home')
            else
                local path = Luall.inner.expand_path(args[1])
                print('cd to: ' .. path)
                Luall.api.cd(path)
            end
        end,
    },
	inner = {

        ---@param path string
        expand_path = function(path)
            local start = path:sub(1, 2)
            if  start == './' then
                path = string.gsub(path, '.', Luall.vars.cwd, 1)
            elseif start == '~/' then
                path = string.gsub(path, '~', Luall.vars.user.home, 1)
            end
            return path
        end,
        ---@param path string
        format_path = function (path)
            return path:gsub(Luall.vars.home, '~')
        end,
		parse = function(input)
            local tokens = first_pass(input)
            ---@type string[]
            local args = {}
            for _, token in pairs(tokens) do
                table.insert(args, token.val)
            end

            local cmd = table.remove(args, 1)
            if cmd == nil then
                Luall.api.set_error(-1)
                return
            end

            -- print("cmd : " .. cmd)
            -- print("argc: " .. #args)
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
                -- TODO: should check PATH instead of just putting /bin/
                local path = '/bin/' .. cmd
                local ok, _ = pcall(Luall.api['exec'], path, table.unpack(args))
                if not ok then
                    print("could not find command")
                end
            end
		end
	},
    prompts = {
        prompt = function()
            local vars = Luall.vars;
            local user = vars.user;
            local err = ''
            if Luall.vars.error ~= 0 then
                err = ' [' .. Luall.vars.error .. ']'
            end
            local cwd = string.gsub(vars.cwd, user.home, '~');
            return user.name .. "@" .. vars.host .. ' ' .. cwd .. err .. ">"
        end,
        right_prompt = function() return "" end,
        greeting = function() return "" end,
    },

    setup = function()
        local config = Luall.vars.config
        local expand_path = Luall.inner.expand_path
        config.config_path = expand_path(config.config_path)
        config.init_path   = expand_path(config.init_path)
        config.hot_path    = expand_path(config.hot_path)
    end
}
