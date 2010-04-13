
-- !time

dofile ("scripts/include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "time" then
		send_private_msg(target, os.date("%H:%M:%S %Z %d.%m.%Y", os.time()))
	end
	return 0
end
