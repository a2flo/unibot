
-- lua $url [search]

function create_and_send_url(target, cmd, parameters, urlless, urlmore)
	local url = ( parameters ~= cmd ) and urlmore..handle_args_chronological(parameters) or urlless
	send_private_msg(target, url_encode(url))
end

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first ???
	if cmd == "g" then
		create_and_send_url(target, cmd, parameters, "http://www.google.de", "http://www.google.de/search?q=")
	end
	if cmd == "wd" then
		create_and_send_url(target, cmd, parameters, "http://de.wikipedia.org/wiki/Hauptseite", "http://de.wikipedia.org/wiki/")
	end
	if cmd == "we" then
		create_and_send_url(target, cmd, parameters, "http://en.wikipedia.org/wiki/Mainpage", "http://en.wikipedia.org/wiki/")
	end
	if cmd == "wa" then
		create_and_send_url(target, cmd, parameters, "http://www.wolframalpha.com", "http://www.wolframalpha.com/input/?i=")
	end
	if cmd == "dict" then
		create_and_send_url(target, cmd, parameters, "http://www.dict.cc", "http://www.dict.cc/?s=")
	end
	return 0
end
