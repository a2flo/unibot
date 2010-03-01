
-- lua upload link

function handle_message(origin, target, cmd, parameters)
	if cmd == "upload" then
		send_private_msg(target, "http://www.file-upload.net/")
	end
	return 0
end
