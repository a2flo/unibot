
-- create a ticket or bug report for unibot

dofile ("scripts/include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "ticket" or cmd == "bug" then
		local summary = url_encode(( parameters ~= cmd ) and parameters or "<please specify a short summary>")
		local ticket_type = ( cmd == "ticket" ) and "new%20functionality" or "defect"
		send_private_msg(target, "Please open this link to send your ticket: http://trac-git.assembla.com/unibot/newticket?reporter="..origin.."&summary="..summary.."&type="..ticket_type.."&version=0.2.x&milestone=v0.2")
	end
	return 0
end
