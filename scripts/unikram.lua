
-- lua unikram link

function handle_message(origin, target, cmd, parameters)
	if cmd == "unikram" then
		send_private_msg(target, "This service is outdated! Yannic will move it to the new website. Soon.")
		send_private_msg(target, "http://thedarkwebsite.de/tdw-old/?n=u&s=unikram")
	end
	return 0
end
