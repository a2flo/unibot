
-- lua 42

function handle_message(origin, target, cmd, parameters)
	if cmd == "42" then
		send_private_msg(target, "Frage wird berechnet...")
	end
	return 0
end
