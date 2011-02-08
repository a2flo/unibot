
-- create a ticket or bug report for unibot

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "ticket" or cmd == "bug" then
		local summary = url_encode(( parameters ~= cmd ) and parameters or "<please specify a short summary>")
		local ticket_type = ( cmd == "ticket" ) and "new%20functionality" or "defect"
		local ticket_type_str = ( cmd == "ticket" ) and "ticket" or "bug report"
		send_private_msg(target, "Please open this link to send your "..ticket_type_str..": http://trac-git.assembla.com/unibot/newticket?reporter="..origin.."&summary="..summary.."&type="..ticket_type.."&version=0.3.x&milestone=v0.3")
	end
	return 0
end
