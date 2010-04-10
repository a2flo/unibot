
-- !users

function handle_message(origin, target, cmd, parameters)
	if cmd == "users" then
		local users, user_count = get_users()
		
		local users_str = ""
		local user_num = 1
		for key, value in pairs(users) do
			users_str = users_str .. key
			if string.len(value[1]) > 0 or string.len(value[2]) > 0 then
				users_str = users_str .. " ("
				if string.len(value[1]) > 0 then
					users_str = users_str .. value[1] .. ", "
				end
				if string.len(value[2]) > 0 then
					users_str = users_str .. value[2]
				end
				users_str = users_str .. ")"
			end
			
			if user_num < user_count then
				users_str = users_str .. ", "
			end
			
			user_num = user_num + 1
		end
		
		send_private_msg(target, users_str)
	end
	return 0
end
