
-- lua cmd

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) and cmd == "cmd" then
		send(parameters)
	end
end
