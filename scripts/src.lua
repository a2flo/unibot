
-- bot src (git+trac) links

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "src" then
		send_private_msg(target, "\002git\002: git clone git://git.assembla.com/unibot.git")
		send_private_msg(target, "\002trac\002: http://trac-git.assembla.com/unibot/roadmap")
		send_private_msg(target, "\002source browser\002: http://www.assembla.com/code/unibot/git/nodes?rev=master")
	end
	return 0
end
