---@type function
local LuallParser = nil
Luall = {

    ---list of functions that will be tested
    ---@type function[]
    testing = {},

    test = function()
        print('lua tests... #' .. #Luall.testing)
        for _, value in ipairs(Luall.testing) do
            print('test...')
            value()
        end
    end,

	vars = {
        -- this vars are set up at runtime and
        -- the values here are just placeholders
		host = '',
		cwd  = '',
		user = {
			name = '',
			home = '',
		},
		error = 0,
        debug = false,
        -- this variable is lua only
        -- it only exists durent this current session
        history = {
            'lua print("hello world")',
        },
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
    -- this are commands that only exist in lua
    util = {
        ---@param args table
        lua = function (args)
            if #args == 0 then
                return;
            end

            local func, _ = load(args[1])
            if func ~= nil then
                local ok, _ = pcall(func)
                if not ok then
                    print('failed to run')
                end
            end
        end,

        ---@param args table 
        alias = function (args)
            local name = table.remove(args, 1)

            local func, _ = load(args[1])
            if func == nil then
                print('could not create alias!')
                return
            end

            Luall.alias[name] = func
        end
    },
    -- this are commands that extend the utility of the api
    extend = {
        cd = function(args)
            if #args == 0 then
                Luall.api.cd(Luall.vars.user.home)
            else
                local path = Luall.inner.expand_path(args[1])
                Luall.api.cd(path)
            end
        end,
    },
    -- the alias are blueprints for process.new
    alias = {
        ls = function(...)
            return {'ls', {'--color', table.unpack(...)}}
        end,

        ll = function(...)
            return {'ls', {'--color', '-lA', table.unpack(...)}}
        end,
    },
    -- this are not commands but usefull stuff
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
        ---@param r integer
        ---@param g integer
        ---@param b integer
        full_color = function (r, g, b)
            return '\x1b[38;2;' .. r .. ';' .. g .. ';' .. b .. 'm'
        end,
        reset_color = function ()
            return '\x1b[0m'
        end,
        ---@param input string
		parse = function(input)
            if input == '' then
                return
            end

            table.insert(Luall.vars.history, input)
            LuallParser(input)
        end,

        ---@param index number
        history = function(index)
            if Luall.vars.history == nil then
                return
            end
            local indx = #Luall.vars.history - index

            return Luall.vars.history[indx]
        end,
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
            local rc = Luall.inner.reset_color;
            local fc = Luall.inner.full_color;
            local debug = ''
            if Luall.vars.debug then
                debug = '[DEBUG]'
            end

            local function color_text(text, r, g, b)
                return fc(r, g, b) .. text .. rc()
            end
            return debug .. color_text(user.name, 32, 255, 64) .. "@" .. vars.host .. ' ' .. color_text(cwd, 32, 255, 64) .. err .. "\n" .. color_text(">", 255, 255, 128)
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
    end,

}

LuallParser = require('parser')
