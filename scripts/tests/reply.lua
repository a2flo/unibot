
-- bot test reply

dofile (package.path.."include/global.lua")

local bot_name = get_config_entry("bot_name")
function handle_message(origin, target, cmd, parameters)
	local msg = ( parameters ~= cmd ) and cmd.." "..parameters or parameters
	local tokens = tokenize(msg, "%s")
	
	if table.contains(tokens, bot_name) or table.contains(tokens, "bot") then
		for i = 1, table.maxn(tokens) do
			local token = strip_special_chars(tokens[i])
			if token == "thx" or token == "tnx" or token == "danke" then
				send_private_msg(target, "np, "..origin)
			end
		end
	end
end
