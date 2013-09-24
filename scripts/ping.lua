
-- lua is the bot is still active?

require "global"

function handle_message(origin, target, cmd, parameters)
	-- TODO: ping -> avg server ping (+min/max/bars)?
	if cmd == "ping" then
		send_private_msg(target, "pong!")
	end
	if cmd == "pong" then
		send_private_msg(target, "ping!")
	end
	return 0
end
