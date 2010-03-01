
-- lua ss10 links

function handle_message(origin, target, cmd, parameters)
	if cmd == "ss10" then
		send_private_msg(target, "Crypto: http://www.infsec.cs.uni-sb.de/teaching/10SS/Cryptography/")
	end
	return 0
end
