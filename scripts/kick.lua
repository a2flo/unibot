
-- lua kick [name [message]]

function handle_message(origin, target, cmd, parameters)
	if cmd == "kick" then
		-- kick no owner & (no) parameter: kick origin
		-- kick owner & no params: kick owner
		-- kick owner & name: kick name random_message
		-- kick owner & name & message: kick name message

		-- no params / only name
		local name = ( parameters ~= cmd ) and parameters or origin
		-- TODO random message
		local msg = "Cause I can!"

		-- name + msg
		if name ~= origin then
			local space_pos = string.find(parameters, "%s")
			if space_pos ~= nil then
				name = string.sub(parameters, 0, space_pos-1)
				msg = string.sub(parameters, space_pos+1)
			end
		end 

		if is_owner(origin) then
			send_kick(name, msg)
		else
			send_kick(origin, "Permission denied.")
		end
	end
	return 0
end