
-- lua cmd

function handle_message(origin, target, cmd, parameters)
	if origin == "[flo]" or origin == "maid3n" and cmd == "cmd" then
		send(parameters)
	end
end
