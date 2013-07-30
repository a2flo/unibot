
-- bot src (github) links

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "src" then
		send_private_msg(target, "\002source\002: https://github.com/a2flo/unibot")
		send_private_msg(target, "\002git\002: git clone https://github.com/a2flo/unibot.git")
	end
	return 0
end
