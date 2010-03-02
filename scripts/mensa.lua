
-- lua mensa links

function handle_message(origin, target, cmd, parameters)
	if cmd == "mensa" then
		local day = os.date("%w", os.time())
		local days = { "montag", "dienstag", "mittwoch", "donnerstag", "freitag" }
		if day >= 1 and day < 6 then
			send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/"..days[day])
		else
	                send_private_msg(target, "Es ist Wochenende! Steh auf und kauf dir was ;)")
		end
	end
	return 0
end
