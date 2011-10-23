
-- !$url [message offset] [word offset]

require "global"

function create_and_send_url(target, cmd, parameters, urlless, urlmore)
	local url = ( parameters ~= cmd ) and urlmore..handle_args_chronological(parameters) or urlless
	send_private_msg(target, url_encode(url))
end

function handle_message(origin, target, cmd, parameters)
	local pages = {
		{ "g", "http://www.google.com", "http://www.google.com/search?q=" },
		{ "w", "http://en.wikipedia.org/wiki/Mainpage", "http://en.wikipedia.org/wiki/" },
		{ "wp", "http://en.wikipedia.org/wiki/Mainpage", "http://en.wikipedia.org/wiki/" },
		{ "wa", "http://www.wolframalpha.com", "http://www.wolframalpha.com/input/?i=" },
		{ "dict", "http://www.dict.cc", "http://www.dict.cc/?s=" },
		{ "v", "http://www.chefkoch.de", "http://www.chefkoch.de/suche.php?wo=0&suche=vegan " },
	}
	
	for i = 1, #pages do
		if cmd == pages[i][1] then
			create_and_send_url(target, cmd, parameters, pages[i][2], pages[i][3])
		end
	end

	return 0
end
