
-- lua $url [search]

function create_and_send_url(target, cmd, parameters, urlless, urlmore)
	local url = ( parameters ~= cmd ) and urlmore..handle_args_chronological(parameters) or urlless
	send_private_msg(target, url_encode(url))
end

function handle_message(origin, target, cmd, parameters)
	-- TODO if page exist, then print the first ???
	
	local pages = {
		{ "g", "http://www.google.de", "http://www.google.de/search?q=" },
		{ "wd", "http://de.wikipedia.org/wiki/Hauptseite", "http://de.wikipedia.org/wiki/" },
		{ "we", "http://en.wikipedia.org/wiki/Mainpage", "http://en.wikipedia.org/wiki/" },
		{ "wa", "http://www.wolframalpha.com", "http://www.wolframalpha.com/input/?i=" },
		{ "dict", "http://www.dict.cc", "http://www.dict.cc/?s=" },
	}
	
	for i = 1, table.maxn(pages) do
		if cmd == pages[i][1] then
			create_and_send_url(target, cmd, parameters, pages[i][2], pages[i][3])
		end
	end

	return 0
end