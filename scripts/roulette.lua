
-- lua roulette [name]
-- alternative: if name is owner and origin no owner, then the origin will be kicked.

bullets = 1
lastuser = "root"
function handle_message(origin, target, cmd, parameters)
	if cmd == "roulette" then
		local name = origin
		lastuser = origin
		if is_owner(origin) then
			name = ( parameters ~= cmd ) and parameters or origin
		else
			name = origin
		end
                local wait = {
			"Das Klicken laesst "..name.." die Luft anhalten...",
			name.." drueckt ab..."
		}
		local kill = {
                        "Das Gehirn von "..name.." landet an der Wand!",
			name.." hinterlaesst eine Sauerei... holt mal jemand schnell die Putzfrau",
			"und die Kugel toetet "..name..".",
			"Boom!"
		}
		local luck = {
			"Waffe klemmt.. Glueck gehabt",
			name.." hat es ueberlebt... "..name.." muss einen Schutzengel haben.",
			"und "..name.." hat Glueck!",
			"nichts passiert.",
			"Click!"
		}
		if lastuser == origin then
			send_private_msg(target, "You can't pull the trigger twice in a row, dolt!")
		else
			send_private_msg(target, wait[math.random(1, table.maxn(wait))])
			if (math.random(1, 6) == 3) or (bullets == 6) then
				send_private_msg(target, name..": chamber #"..bullets.." of 6 => +BANG+")
				send_kick(name, kill[math.random(1, table.maxn(kill))])
				send_action_msg(get_config_entry("channel"), " reloads.")
				bullets = 1
			else
				send_private_msg(target, name..": chamber #"..bullets.." of 6 => +click+")
				send_private_msg(target, luck[math.random(1, table.maxn(luck))])
				bullets = (bullets + 1)
			end
		end
	end
	return 0
end
