
-- lua paste services links

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "paste" then
		send_private_msg(target, "http://pastebin.com/")
	end
	return 0
end
