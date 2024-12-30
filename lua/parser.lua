local INNER = {
-- convert a cmd line to exec
---@param line string
	_parser = function(line)
		line = string.gsub(line, "\n", " ")
		line = string.gsub(line, "\t", " ")

		while string.match(line, "  ") do
			line = string.gsub(line, "  ", " ")
		end

		local parts = {}
		for str in string.gmatch(line, "[^%s]+") do
			table.insert(parts, str)
		end

	end
};


return INNER
