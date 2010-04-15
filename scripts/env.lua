
-- !set, !get and !env

dofile (package.path.."include/global.lua")

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
			local env_vars = { "connected", "joined", "parted", "op", "quit", "kicked", "identified", "silenced" }
			
			for i = 1, table.maxn(env_vars) do
				send_private_msg(target, env_vars[i]..": "..tostring(get_bot_state(env_vars[i])))
			end
		end
	end
	return 0
end
