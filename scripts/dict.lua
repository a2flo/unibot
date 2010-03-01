
-- lua dict.cc [search]

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first words
	if cmd == "dict" then
		local url = ( parameters ~= cmd ) and "http://www.dict.cc/?s="..parameters or "http://www.dict.cc/"
		send_private_msg(target, url_encode(url))
	end
	return 0
end
