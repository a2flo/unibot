-- lua who's your daddy?

function handle_message(origin, target, cmd, parameters)
	if cmd == "who\'s your daddy?" then
		local owners = get_config_entry("owner_names")
		send_private_msg(target, owners)
	end
	return 0
end
