
-- lua mensa links

function handle_message(origin, target, cmd, parameters)
	if cmd == "mensa" then
		local day = os.date("%w", os.time())
		if day == 1 then
	                send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/montag.htm")
		elseif day == 2 then
	                send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/dienstag.htm")
		elseif day == 3 then
	                send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/mittwoch.htm")
		elseif day == 4 then
	                send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/donnerstag.htm")
		elseif day == 5 then
	                send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/freitag.htm")
		else
	                send_private_msg(target, "Es ist Wochenende! Steh auf und kauf dir was ;)")
		end
	end
	return 0
end
