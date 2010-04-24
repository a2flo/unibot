
-- !users

dofile (package.path.."include/global.lua")

function print_users(target, extended)
	local users, user_count = get_users()
	
	local users_str = ""
	local user_num = 1
	for key, value in pairs(users) do
		users_str = users_str .. key
		
		-- if extended mode is enabled also send real name and host info
		if extended then
			if string.len(value[1]) > 0 or string.len(value[2]) > 0 then
				users_str = users_str .. " "
				if string.len(value[2]) > 0 then
					users_str = users_str .. "real name: " .. value[2] .. ", "
				end
				if string.len(value[3]) > 0 then
					users_str = users_str .. "host: " .. value[3] .. ", "
				end
				if string.len(value[4]) > 0 then
					users_str = users_str .. "host user: " .. value[4] .. ", "
				end
				if string.len(value[5]) > 0 then
					users_str = users_str .. "registered: " .. value[5] .. ", "
				end
				
				if string.len(value[6]) > 0 then
					users_str = users_str .. "ctcp support: " .. value[6]
					
					-- add comma (no comma needed, if no ctcp support)
					if value[6] == "yes" then
						users_str = users_str .. ", "
					end
				end
				
				if string.len(value[7]) > 0 then
					users_str = users_str .. "client: " .. value[7]
				end
				users_str = users_str .. "\n" -- one msg per user
			end
		else
			if user_num < user_count then
				users_str = users_str .. ", "
			end
		end
		
		user_num = user_num + 1
	end
	
	send_private_msg(target, users_str)
end

function handle_message(origin, target, cmd, parameters)
	if cmd == "users" then
		print_users(target, false)
	end
	if is_owner(origin) and (cmd == "usersext" or cmd == "users_extended") then
		print_users(target, true)
	end
	return 0
end
