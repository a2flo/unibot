
-- lua who's your daddy?

require "global"

function handle_message(origin, target, cmd, parameters)
	local joined_cmd = ( parameters ~= cmd ) and cmd.." "..parameters or cmd
	if joined_cmd == "who\'s your daddy?" or joined_cmd == "who is your daddy?" then
		local owners = get_config_entry("owner_names")
		send_private_msg(target, owners)
	end
	return 0
end
