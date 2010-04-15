
-- lua who's your daddy?

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "who\'s your daddy?" or cmd == "who is your daddy?" then
		local owners = get_config_entry("owner_names")
		send_private_msg(target, owners)
	end
	return 0
end
