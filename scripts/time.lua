
-- !time

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "time" then
		local data_format = ( get_os() == 1 ) and "%H:%M:%S %d.%m.%Y" or "%H:%M:%S %Z %d.%m.%Y"
		send_private_msg(target, os.date(data_format, os.time()))
	end
	return 0
end
