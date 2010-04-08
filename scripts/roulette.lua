
-- lua roulette [name]
-- alternative: if name is owner and origin no owner, then the origin will be kicked.

bullets = -1
slots = -1
chamber = -1
lastuser = ""

function reset_roulette()
	bullets = 1
	slots = 6
	chamber = crand(slots)
	lastuser = ""
end

reset_roulette()
		
function handle_message(origin, target, cmd, parameters)
	if cmd == "roulette" then
		local name = origin
		if is_owner(origin) then
			name = ( parameters ~= cmd ) and parameters or origin
		else
			name = origin
		end
		
		local wait_msg = {
			"Das Klicken lässt "..name.." die Luft anhalten...",
			name.." drückt ab..."
		}
		local kill_msg = {
			"Das Gehirn von "..name.." landet an der Wand!",
			name.." hinterlässt eine Sauerei... holt mal jemand schnell die Putzfrau",
			"und die Kugel tötet "..name..".",
			"Boom!",
			"Headshot!",
			"Medic!"
		}
		local luck_msg = {
			"Waffe klemmt.. Glück gehabt!",
			name.." hat es überlebt... "..name.." muss einen Schutzengel haben.",
			"und "..name.." hat Glück!",
			"nichts passiert.",
			"Click!"
		}
		
		local channel = get_config_entry("channel")
		if lastuser == origin then
			send_private_msg(target, "You can't pull the trigger twice in a row, dolt!")
		else
			lastuser = origin
			send_private_msg(channel, wait_msg[math.random(1, table.maxn(wait_msg))])
			
			if bullets == chamber then
				send_private_msg(channel, name..": chamber #"..bullets.." of "..slots.." => *BANG*")
				send_kick(name, kill_msg[math.random(1, table.maxn(kill_msg))])
				send_action_msg(channel, "reloads")
				
				reset_roulette()
			else
				send_private_msg(channel, name..": chamber #"..bullets.." of "..slots.." => *click*")
				send_private_msg(channel, luck_msg[math.random(1, table.maxn(luck_msg))])
				bullets = bullets + 1
			end
		end
	end
	return 0
end
