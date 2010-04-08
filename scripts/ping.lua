
-- lua is the bot is still active?

function handle_message(origin, target, cmd, parameters)
	if cmd == "ping" then
		send_private_msg(target, "pong!")
	end
	if cmd == "pong" then
		send_private_msg(target, "ping!")
	end
	return 0
end
