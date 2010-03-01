
-- lua google [search]

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first hits (with hitquote)
	if cmd == "g" then
		local url = ( parameters ~= cmd ) and "http://www.google.de/search?q="..parameters or "http://www.google.de"
		send_private_msg(target, url_encode(url))
	end
	return 0
end
