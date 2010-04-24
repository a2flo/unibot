
-- unikram

function handle_message(origin, target, cmd, parameters)
	if cmd == "unikram" then
		send_private_msg(target, "http://thedarkwebsite.de/yannic/?s=unikram")
	end
	return 0
end
