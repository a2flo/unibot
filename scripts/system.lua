
-- !system

require "global"

bot_system = ""
if get_os() == 1 then
	local sys = execute_command("systeminfo /NH /FO CSV")
	local sys_tokens = tokenize(sys, "\",\"")
	if(#sys_tokens > 1) then bot_system = bot_system .. sys_tokens[2] end
	if(#sys_tokens > 2) then bot_system = bot_system .. sys_tokens[3] end
end

function handle_message(origin, target, cmd, parameters)
	if cmd == "system" then
		if get_os() ~= 1 then
			bot_system = execute_command("uname -a 2>&1")
		end
		send_private_msg(target, bot_system)
	end
	return 0
end
