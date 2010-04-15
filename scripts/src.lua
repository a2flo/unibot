
-- bot src (git+trac) links

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "src" then
		send_private_msg(target, "git : git clone git://git.assembla.com/unibot.git")
		send_private_msg(target, "trac: http://trac-git.assembla.com/unibot")
	end
	return 0
end
