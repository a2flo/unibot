
-- help command, sends an overview of all available commands to the requesting user

require "global"

function handle_message(origin, target, cmd, parameters)
	if cmd == "help" then
		-- Note: 0x02 is used for highlighting text (-> bold text)

		-- usage: !help <command>
		-- Bot Info
		if parameters == "who's your daddy?" or parameters == "who is your daddy?" then
			send_private_msg(origin, " who's your daddy?: outputs the owner of the bot; they're able to use commands which controls the bot")
		elseif parameters == "system" then
			send_private_msg(origin, " system: outputs \"uname -a\" on the machine the bot was started")
		elseif parameters == "time" then
			send_private_msg(origin, " time: outputs the time")
		elseif parameters == "uptime" then
			send_private_msg(origin, " uptime: outputs the uptime of the bot since the last - not the first! - start")
		elseif parameters == "src" then
			send_private_msg(origin, " src: outputs links, where one can get the source code of the bot")
		elseif parameters == "spec" then
			send_private_msg(origin, " spec: outputs a link to the irc-rfc")
		elseif parameters == "version" then
			send_private_msg(origin, " version: outputs the version of the bot")
		elseif parameters == "ping" or parameters == "pong" then
			send_private_msg(origin, " ping: one can test the response time of the bot with this command")
		elseif parameters == "bug" then
			send_private_msg(origin, " bug <report>: you found a bug? please report it with \"!bug <report>\" and open the link to specify your bug")
		elseif parameters == "ticket" then
			send_private_msg(origin, " ticket <summary>: you have a good idea for the bot? please report it with \"ticket <summary>\" and open the link to specify your ticket")
		elseif parameters == "unibot" then
			send_private_msg(origin, " unibot: UniBot !version; developed by Florian Ziesche, Yannic Haupenthal")
		-- Channel
		elseif parameters == "users" then
			send_private_msg(origin, " users: outputs all current users in the channel")
		elseif parameters == "roulette" then
			if is_owner(origin) then
				send_private_msg(origin, " roulette [<user>]: use roulette at yourself [or the given user]")
			else
				send_private_msg(origin, " roulette: play the well-known game! you'll be kicked if the bullet hits you. if the bullet is in the last chamber, a random player will be kicked")
			end
		elseif parameters == "slap" then
			send_private_msg(origin, " slap [<user>]: slap yourself [or the given user]")
		elseif parameters == "dice" then
			send_private_msg(origin, " dice [<number>]: dice with a standard dice (6) [or a given dice]")
		elseif parameters == "identify" then
			if is_owner(origin) then
				send_private_msg(origin, " identify [<user>]: sends an identify/acc request to nickserv for all users [or only the given one]")
			else 
				send_private_msg(origin, " identify: sends an identify/acc request to nickserv")
			end
		-- Uni
		elseif parameters == "paste" then
			send_private_msg(origin, " paste: outputs a link to a pasteservice")
		elseif parameters == "upload" then
			send_private_msg(origin, " upload: outputs a link to an uploadservice")
		elseif parameters == "mensa" then
			send_private_msg(origin, " mensa: outputs a link, where one can find an overview of meals at the current day in the mensa")
		elseif parameters == "happa" then
			send_private_msg(origin, " happa: outputs a link, where one can find an overview of meals at the current day of the whole campus")
		elseif parameters == "unikram" then
			send_private_msg(origin, " unikram: outputs a link to the unikram at thedarkwebsite.de")
		-- Links
		elseif parameters == "w" or parameters == "wp" then
			send_private_msg(origin, " w [<term>]: outputs a link to en.wikipedia.org [or to the given term]")
		elseif parameters == "wa" then
			send_private_msg(origin, " wa [<term>]: outputs a link to wolframalpha.com [or to the given term]")
		elseif parameters == "g" then
			send_private_msg(origin, " g [<term>]: outputs a link to google.com [or to the given term]")
		elseif parameters == "dict" then
			send_private_msg(origin, " dict [<term>]: outputs a link to dict.cc [or to the given term]")
		elseif parameters == "v" then
			send_private_msg(origin, " v [<term>]: outputs a link to chefkoch.de [or to the given term with 'vegan' as prefix]")
		-- Owner Cmds
		elseif parameters == "cmd" and is_owner(origin) then
			send_private_msg(origin, " cmd <irc cmd>: executes the given command, i.e. \"!cmd PRIVMSG #unichannel :bla\"")
		elseif parameters == "quit" and is_owner(origin) then
			send_private_msg(origin, " quit [<bot name>]: quits every bot in the channel [or only the given one]")
		elseif parameters == "kick" and is_owner(origin) then
			send_private_msg(origin, " kick [<user>]: kicks yourself [or the given user]")
		elseif parameters == "set" and is_owner(origin) then
			send_private_msg(origin, " set <bot state> <value>: sets the given <bot state> to <value>, i.e. \"!set silenced true\"")
		elseif parameters == "get" and is_owner(origin) then
			send_private_msg(origin, " get <bot state>: gets the value of the given bot state")
		elseif parameters == "env" and is_owner(origin) then
			send_private_msg(origin, " env: gets the values of the bot state")
		elseif parameters == "usersext" and is_owner(origin) then
			send_private_msg(origin, " usersext: gets further information about the current users in the channel")
		-- Dev Cmds
		elseif parameters == "bindings_test" and is_owner(origin) then
			send_private_msg(origin, " bindings_test: outputs several tests")
		elseif parameters == "gitupdate" and is_owner(origin) then
			send_private_msg(origin, " gitupdate: executes git pull and reloads the scripts")
		elseif parameters == "reload_scripts" and is_owner(origin) then
			send_private_msg(origin, " reload_scripts: reloads the scripts")
		elseif parameters == "reload_script" and is_owner(origin) then
			send_private_msg(origin, " reload_script <filename>: reloads the given script")
		else
			send_private_msg(origin, "***** \002UniBot Help\002 *****")
			send_private_msg(origin, "Use '!help <command>' to get further information about this command.")
			send_private_msg(origin, "* \002Bot Info\002: who's your daddy?, system, time, uptime, src, spec, version, ping, bug <report>, ticket <summary>")
			send_private_msg(origin, "* \002Channel\002: users, roulette, slap <user>, dice <range>, identify")
			send_private_msg(origin, "* \002Uni\002: paste, upload, mensa, happa, unikram")
			send_private_msg(origin, "* \002Links\002: w, wa, g, dict")
			send_private_msg(origin, "  \002+options\002: <message offset> <word offset>, extracts the word (given by word offset) or whole msg (if no word offset) of the msg specified by message offset (in reverse)")

			-- TODO: clean up add2file and add command here
			if is_owner(origin) then
				send_private_msg(origin, "* \002Owner Cmds\002: cmd <irc cmd>, quit <bot name>, kick <user>, set <bot state> <value>, get <bot state>, env, usersext, identify <user>")
				send_private_msg(origin, "* \002Dev Cmds\002: bindings_test, gitupdate, reload_scripts, reload_script <filename>")
			end
		end
	end
	return 0
end
