
-- lua en.wikipedia.org [search]

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first sentence
	if cmd == "we" then
		local url = ( parameters ~= cmd ) and "http://en.wikipedia.org/wiki/"..parameters or "http://en.wikipedia.org/wiki/Mainpage"
		send_private_msg(target, url_encode(url))
	end
	return 0
end
