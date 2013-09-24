
-- !$url [message offset] [word offset]

require "global"

function create_and_send_url(target, cmd, parameters, urlless, urlmore)
	local url = ( parameters ~= cmd ) and urlmore..handle_args_chronological(parameters) or urlless
	send_private_msg(target, url_encode(url))
end

function handle_message(origin, target, cmd, parameters)
	local pages = {
		{ "g", "https://www.google.com", "https://www.google.com/search?q=" },
		{ "w", "https://en.wikipedia.org/wiki/Mainpage", "https://en.wikipedia.org/wiki/" },
		{ "wp", "https://en.wikipedia.org/wiki/Mainpage", "https://en.wikipedia.org/wiki/" },
		{ "wa", "https://www.wolframalpha.com", "https://www.wolframalpha.com/input/?i=" },
		{ "dict", "https://dict.leo.org", "https://dict.leo.org/ende/index_en.html#search=" },
	}
	
	for i = 1, #pages do
		if cmd == pages[i][1] then
			create_and_send_url(target, cmd, parameters, pages[i][2], pages[i][3])
		end
	end

	return 0
end
