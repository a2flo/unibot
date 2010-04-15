
-- lua spec link

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "spec" then
		send_private_msg(target, "http://www.ietf.org/rfc/rfc2812.txt")
	end
	return 0
end
