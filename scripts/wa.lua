
-- lua wolframalpha [search]

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first ??
	if cmd == "wa" then
		local url = ( parameters ~= cmd ) and "http://www.wolframalpha.com/input/?i="..parameters or "http://www.wolframalpha.com"
		send_private_msg(target, url_encode(url))
	end
	return 0
end
