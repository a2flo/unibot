
-- lua happa link

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "happa" then
		send_private_msg(target, "http://happa.dfki.de/")
	end
	return 0
end
