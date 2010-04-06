
function xor(b1, b2)
	return (b1 or b2) and (not (b1 and b2))
end

function toboolean(value)
	if tostring(value) == "true" or tonumber(value) == 1 then
		return true
	end
	return false
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