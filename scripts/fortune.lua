
-- !fortune
-- TODO: don't update the topic within 48-72 hours, if the topic was set by someone else (?)
-- TODO: do this within the core, so the topic can change automatically every 24 hours

require "global"

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) and cmd == "fortune" then
		fortune = execute_command("fortune -s | tr '\\n' ' ' | tr '\\t' ' ' | sed 's/ \\+/ /g' 2>&1")
		channel = get_config_entry("channel")
		send("TOPIC "..channel.." :"..fortune)
	end
	return 0
end
