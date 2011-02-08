
-- bot eastereggs ;)

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "42" then
		send_private_msg(target, "Calculating question ...")
	end
	return 0
end
