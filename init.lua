---@class User
---@field name string
---@field home string

---@class values
---@field cwd  string
---@field host string
---@field user User
---@field error integer

---@class functions
---@field prompt function
---@field right_prompt function
---@field greeting function

luall.functions.prompt = function ()
    return ">";
end

luall.functions.right_prompt = function ()
    return ">";
end

luall.functions.greeting = function ()
    return "lua!";
end
