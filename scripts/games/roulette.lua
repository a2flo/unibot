
-- lua roulette [name]
-- alternative: if name is owner and origin no owner, then the origin will be kicked.

require "global"

bullets = -1
slots = -1
chamber = -1
lastuser = ""
user_list = {}

function reset_roulette()
	bullets = 1
	slots = 6
	chamber = crand(slots)
	lastuser = ""
	user_list = {}
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
		
		-- if this is the last bullet, choose a random user
		if bullets == slots then
			table.unique(user_list)
			name = table.random(user_list)
		else
			table.insert(user_list, name)
		end
		
		-- msgs
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
			"Der Schlagbolzen trifft ins Leere."
		}
		local double_msg = {
			"You can't pull the trigger twice in a row, dolt!",
			"Lass anderen auch eine Chance!"
		}
		
		-- and go
		local channel = get_config_entry("channel")
		if bullets ~= slots and lastuser == origin then
			send_private_msg(target, table.random(double_msg))
		else
			lastuser = name
			
			if bullets == chamber then
				if bullets ~= slots then
					send_private_msg(channel, "chamber #"..bullets.." of "..slots.." => *BANG*")
				else
					send_private_msg(channel, "random last bullet hits "..name.." => *BANG*")
				end
				
				send_kick(name, table.random(kill_msg))
				send_action_msg(channel, "reloads")
				
				reset_roulette()
			else
				send_private_msg(channel, "chamber #"..bullets.." of "..slots.." => *CLICK* "..table.random(luck_msg))
				bullets = bullets + 1
			end
		end
	end
	return 0
end
