
-- lua paste services links

function handle_message(origin, target, cmd, parameters)
	if cmd == "paste" then
		send_private_msg(target, "http://pastebin.com/")
		-- send_private_msg(target, "http://pastie.org/")
	end
	return 0
end
