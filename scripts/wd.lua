
-- lua de.wikipedia.org [search]

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first sentence
	if cmd == "wd" then
		local url = ( parameters ~= cmd ) and "http://de.wikipedia.org/wiki/"..handle_args_chronological(parameters) or "http://de.wikipedia.org/wiki/Hauptseite"
		send_private_msg(target, url_encode(url))
	end
	return 0
end
