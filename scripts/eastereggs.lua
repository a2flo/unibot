
-- bot eastereggs ;)

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "42" then
		send_private_msg(target, "Calculating question ...")
	end
	if cmd == "?" then
		send_private_msg(target, "?!")
	end
	return 0
end
