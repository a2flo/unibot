
-- create a ticket or bug report for unibot

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "ticket" or cmd == "bug" then
		local ticket_type_str = ( cmd == "ticket" ) and "ticket" or "bug report"
		send_private_msg(target, "Please open this link to send your "..ticket_type_str..": https://github.com/a2flo/unibot/issues/new")
	end
	return 0
end
