
-- sends an identify/acc request to nickserv

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "identify" then
		-- normal user can only request to identify themselves,
		-- owners can request to reidentify all users or a specific user
		local user = origin
		if is_owner(origin) then
			user = ( parameters ~= cmd ) and parameters or ""
		end
		
		if user ~= "" then
			send_private_msg("nickserv", "acc "..user);
		else
			local users = get_users()
			for key, value in pairs(users) do
				send_private_msg("nickserv", "acc "..key);
			end
		end
	end
	return 0
end
