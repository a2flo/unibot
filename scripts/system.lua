
-- !system

dofile ("scripts/include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "system" then
		local sys = execute_command("uname -a 2>&1")
		send_private_msg(target, sys)
	end
	return 0
end
