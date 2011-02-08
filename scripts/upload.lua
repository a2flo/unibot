
-- lua upload link

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "upload" then
		--send_private_msg(target, "http://www.file-upload.net/")
		send_private_msg(target, "http://www.megaupload.com")
	end
	return 0
end
