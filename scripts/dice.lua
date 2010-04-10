
-- bot dice [value]

function handle_message(origin, target, cmd, parameters)
	if cmd == "dice" then
		local name = ( parameters ~= cmd ) and parameters or "6"
		local rand = crand(tonumber(name))
		send_private_msg(target, rand)
	end
	return 0
end
