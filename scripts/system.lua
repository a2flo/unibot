
-- !system

function handle_message(origin, target, cmd, parameters)
	if cmd == "system" then
		local proc = io.popen("uname -a", "r")
		local sys = proc:read("*a")
		proc:close()
		
		send_private_msg(target, sys)
	end
	return 0
end
