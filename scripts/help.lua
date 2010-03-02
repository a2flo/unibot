
-- help command, sends an overview of all available commands to the requesting user

function handle_message(origin, target, cmd, parameters)
	if cmd == "help" then
--		local helpparam = ( parameters ~= cmd ) and parameters or nil
--		if helpparam == nil then
		send_private_msg(origin, "Help (add a ! to use a command):")
		send_private_msg(origin, "Use '!help <command>' to get further information about this command. // coming soon")
		send_private_msg(origin, "    Links:       wd, we, wa, g, dict")
		send_private_msg(origin, "    Bot/Channel: who's your daddy?, system, time, uptime, src, spec, users (not implemented), quit, roulette, version, kick, set, get, env, ping, bindings_test, help")
		send_private_msg(origin, "    Uni:         unikram (outdated), paste, upload, mensa, happa, ws0910, ss10")
--		send_private_msg(origin, "    Misc:        learn, rev, ?")
		send_private_msg(origin, "    <args1>: <message offset> <word offset>: extracts the word (given by word offset) or whole msg (if no word offset) of the msg specified by message offset (in reverse)")
--		else
--		...
	end
	
	return 0
end
