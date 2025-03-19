Luall = {
	vars= {
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

            print("exec: " .. args[1])
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
