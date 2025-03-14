

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

            --- print("exec: " .. args[1])
            local func, _ = load(args[1])
            if func ~= nil then
                local ok, _ = pcall(func)
                if not ok then
                    print('failed to run')
                end
            end
        end
    },
    overwrite = {
        cd = function(args)
            if #args == 0 then
                Luall.api.cd(Luall.vars.user.home)
            else
                local path = Luall.inner.expand_path(args[1])
                Luall.api.cd(path)
            end
        end,
        ls = function (args)
            table.insert(args, 1, '--color')
            Luall.inner.exec('ls', args)
        end,
    },
	inner = {
        ---@param name string
        ---@param args table
        exec = function (name, args)
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

            local ok, _ = pcall(Luall.api['exec'], target_cmd, table.unpack(args))
            if not ok then
                print('failed to exec cmd: '.. target_cmd)
            end
        end,
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
        ---@param r integer
        ---@param g integer
        ---@param b integer
        full_color = function (r, g, b)
            return '\x1b[38;2;' .. r .. ';' .. g .. ';' .. b .. 'm'
        end,
        reset_color = function ()
            return '\x1b[0m'
        end,
		parse = require('parser')
	},
    matching = {
        files = {}
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
            local fc = Luall.inner.full_color;
            local rc = Luall.inner.reset_color;
            return fc(0, 128, 0) .. user.name .. rc() .. "@" .. vars.host .. ' ' .. fc(0, 128, 0) .. cwd .. rc() .. err .. " >"
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
