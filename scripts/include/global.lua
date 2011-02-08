
--
require "exttable"


function xor(b1, b2)
	return (b1 or b2) and (not (b1 and b2))
end

function toboolean(value)
	if tostring(value) == "true" or tonumber(value) == 1 then
		return true
	end
	return false
end

function tokenize(str, delim)
	local tokens = {}
	local last_pos = 0
	local delim_len = string.len(delim)
	if string.sub(delim, 0, 1) == "%" then
		delim_len = 1
	end

	local pos = string.find(str, delim)
	if pos == nil then
		table.insert(tokens, str)
	else
		repeat
			table.insert(tokens, string.sub(str, last_pos, pos-1))
			
			last_pos = pos+delim_len
			pos = string.find(str, delim, last_pos)
		until pos == nil
		
		-- add last one
		local last_token = string.sub(str, last_pos, string.len(str))
		table.insert(tokens, last_token)
	end
	
	return tokens
end

-- executes a system/os command and returns the output
function execute_command(cmd)
	local output = ""
	local proc = io.popen(cmd, "r")
	while true do
		local out = proc:read("*a")
		if out == nil or out == "" then -- nil/"" indicates EOF
			break
		end
		
		output = output..out
	end
	proc:close()
	
	return output
end
