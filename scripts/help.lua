
-- help command, sends an overview of all available commands to the requesting user

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
	if cmd == "help" then
		-- Note: 0x02 is used for highlighting text (-> bold text)
		
		send_private_msg(origin, "***** \002UniBot Help\002 *****")
		--send_private_msg(origin, "Use '!help <command>' to get further information about this command.") -- TODO: maid3n!
		send_private_msg(origin, "* \002Bot Info\002: who's your daddy?, system, time, uptime, src, spec, version, ping, bug <report>, ticket <summary>")
		send_private_msg(origin, "* \002Channel\002: users, roulette, slap <user>, dice <range>, identify")
		send_private_msg(origin, "* \002Uni\002: paste, upload, mensa, happa, ss10")
		send_private_msg(origin, "* \002Links\002: we, wd, wa, g, dict")
		send_private_msg(origin, "  \002+options\002: <message offset> <word offset>, extracts the word (given by word offset) or whole msg (if no word offset) of the msg specified by message offset (in reverse)")

		-- TODO: clean up add2file and add command here
		
		if is_owner(origin) then
			send_private_msg(origin, "* \002Owner Cmds\002: cmd <irc cmd>, quit <bot name>, kick <user>, set <bot state> <value>, get <bot state>, env, usersext, identify <user>")
			send_private_msg(origin, "* \002Dev Cmds\002: bindings_test, gitupdate, reload_scripts, reload_script <filename>")
		end
	end
	
	return 0
end
