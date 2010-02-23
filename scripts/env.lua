
-- !set, !get and !env

function toboolean(value)
	if tostring(value) == "true" or tonumber(value) == 1 then
		return true
	end
	return false
end

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) then
		if cmd == "set" then
			local sep = string.find(parameters, "%s")
			local state_name = string.sub(parameters, 0, sep-1)
			local state = string.sub(parameters, sep+1)
			set_bot_state(state_name, toboolean(state))
		end
		if cmd == "get" then
			send_private_msg(target, parameters..": "..tostring(get_bot_state(parameters)))
		end
		if cmd == "env" then
			send_private_msg(target, "connected: "..tostring(get_bot_state("connected")))
			send_private_msg(target, "joined: "..tostring(get_bot_state("joined")))
			send_private_msg(target, "parted: "..tostring(get_bot_state("parted")))
			send_private_msg(target, "op: "..tostring(get_bot_state("op")))
			send_private_msg(target, "quit: "..tostring(get_bot_state("quit")))
			send_private_msg(target, "kicked: "..tostring(get_bot_state("kicked")))
			send_private_msg(target, "identified: "..tostring(get_bot_state("identified")))
			send_private_msg(target, "silenced: "..tostring(get_bot_state("silenced")))
		end
	end
	return 0
end
