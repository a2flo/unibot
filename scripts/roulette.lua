
-- lua roulette [name]
-- alternative: if name is owner and origin no owner, then the origin will be kicked.

function handle_message(origin, target, cmd, parameters)
	if cmd == "roulette" then
		local name = origin
		if is_owner(origin) then
			name = ( parameters ~= cmd ) and parameters or origin
		else
			name = origin
		end
                local wait = {
			"Das Klicken laesst "..name.." die Luft anhalten...",
			name.."drueckt ab..."
		}
		local kill = {
                        "Das Gehirn von "..name.." landet an der Wand!",
			name.."hinterlaesst eine Sauerei... holt mal jemand schnell die Putzfrau",
			"und die Kugel toetet "..name..".",
			"Boom!"
		}
		local luck = {
			"Waffe klemmt.. Glueck gehabt",
			name.." hat es ueberlebt... er muss einen Schutzengel haben.",
			"und "..name.." hat Glueck!",
			"nichts passiert.",
			"Click!"
		}
		send_private_msg(target, wait[math.random(1, table.maxn(wait))])
		local bullets = 1
		if (math.random(0, 1000) <= 500) or (bullets == 6) then
			send_private_msg(target, name..": chamber #"..bullets.." of 6 => +BANG+")
			send_kick(name, kill[math.random(1, table.maxn(kill))])
			send_action_msg(get_config_entry("channel"), get_config_entry("bot_name").." reloads.")
			bullets = 1
		else
			send_private_msg(target, name..": chamber #"..bullets.." of 6 => +click+")
			send_private_msg(target, luck[math.random(1, table.maxn(luck))])
			bullets = (bullets + 1)
		end
	end
	return 0
end
