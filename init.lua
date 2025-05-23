---@class User
---@field name string
---@field home string

---@class Values
---@field cwd  string
---@field host string
---@field user User
---@field error integer

---@class Prompts
---@field prompt function
---@field right_prompt function
---@field greeting function

---@class BuiltIn
---@field cd function
---
---@class Inner
---@field parse function

---@class Api
---@field path function
---@field builtin BuiltIn
---@field inner Inner

---@class Luall
---@field values Values
---@field prompts Prompts
---@field api Api
Luall = {
	values = {
		host = "",
		cwd  = "",
		user = {
			name = "",
			home = "",
		},
		error = 0,
	},
	api = {
		---@param _ string
		path = function (_) end,
		builtin = {
			---@param _ string
			cd = function (_) end,
            exit = function (_) end,
		},
		inner = {
			---@param line string
			---@return string
			parse = function(line)
				line = string.gsub(line, "\n", " ")
				line = string.gsub(line, "\t", " ")
				while string.match(line, "  ") do
					line = string.gsub(line, "  ", " ")
				end
				local parts = {}
				for str in string.gmatch(line, '"([^"]+)"|%w+') do
					table.insert(parts, str)
				end
				for part in parts do
					print(part)
				end

                return ""
			end
		},
	},
    prompts = {
        prompt = function() return ">" end,
        right_prompt = function() return "" end,
        greeting = function() return "" end,
    }
}
