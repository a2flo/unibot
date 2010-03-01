
-- lua roulette [name]

function handle_message(origin, target, cmd, parameters)
	if cmd == "roulette" then
		local name = ( parameters ~= cmd ) and parameters or origin
		-- alternative: if name is owner and origin no owner, then the origin will be kicked.
		if is_owner(origin) then
			send_private_msg(target, "Fire!")
			if ((math.mod(math.random(0,1000),42)) <= 21) then
				send_private_msg(target, "Boom!")
				-- TODO random message
				send_kick(name, "Bad luck!")
			else
				send_private_msg(target, "Click!")
			end
		else
			send_kick(origin, "Permission denied.")
		end
	end
	return 0
end
