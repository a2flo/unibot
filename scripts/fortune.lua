
-- !fortune

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "fortune" then
		fortune = execute_command("fortune -s | tr '\\n' ' ' | tr '\\t' ' ' | sed 's/ \\+/ /g' 2>&1")
		channel = get_config_entry("channel")
		send("TOPIC "..channel.." :"..fortune)
	end
	return 0
end
