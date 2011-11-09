
-- unikram

function handle_message(origin, target, cmd, parameters)
	if cmd == "unikram" then
		send_private_msg(target, "http://thedarksite.org/uni.htm")
	end
	return 0
end
