---@class User
---@field name string
---@field home string

---@class Values
---@field cwd  string
---@field host string
---@field user User
---@field error integer
---@field debug boolean

---@class Prompts
---@field prompt function
---@field right_prompt function
---@field greeting function

---@class BuiltIn
---@field cd function
---
---@class Inner
---@class Api

---@class Luall
---@field values Values
---@field prompts Prompts
---@field api Api
---@field inner Inner
