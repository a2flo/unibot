
function xor(b1, b2)
	return (b1 or b2) and (not (b1 and b2))
end

function toboolean(value)
	if tostring(value) == "true" or tonumber(value) == 1 then
		return true
	end
	return false
end
