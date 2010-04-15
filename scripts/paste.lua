
-- lua paste services links

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "paste" then
		send_private_msg(target, "http://pastebin.com/")
	end
	return 0
end
