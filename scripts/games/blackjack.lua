
-- blackjack, b
-- cmds: start, join, leave, hit, stand, help, rules, kick, reset
-- TODO: player timeout/kick after 1 min? inactivity

dofile (package.path.."include/global.lua")

cards = { "2", "3", "4", "5", "6", "7", "8", "9", "Jack", "Queen", "King", "Ace" }
suits = { "♠", "♥", "♦", "♣" }
decks = 4
delt = {}
user_list = {}
dealer_name = get_config_entry("bot_name")
game_running = false

-- sum of all user cards in this round
user_sum = {}

STATUS = {}
STATUS.INACTIVE = -1
STATUS.PLAYING = 0
STATUS.STAND = 1
STATUS.LOST = 2
STATUS.WON = 3
user_status = {}

-- join <channel>.blackjack channel
blackjack_channel = get_config_entry("channel")..".blackjack"
join_channel(blackjack_channel)

function reset_blackjack()
	delt = {}
	user_list = { }
	user_sum = {}
	user_status = {}
	game_running = false
	collectgarbage()
	
	-- add dealer
	table.insert(user_list, dealer_name)
	user_sum[dealer_name] = {}
	user_status[dealer_name] = STATUS.PLAYING
end

function count_cards_value(user)
	local soft_sum = 0
	local hard_sum = 0
	local ace_count = 0
	
	local card_count = table.maxn(user_sum[user])
	for i = 1, card_count do
		local card = user_sum[user][i]
		if card == "Jack" or card == "Queen" or card == "King" then
			soft_sum = soft_sum + 10
		elseif card == "Ace" then
			ace_count = ace_count + 1
			soft_sum = soft_sum + 1
		else
			soft_sum = soft_sum + tostring(card)
		end
	end
	
	-- we only need to consider 1 ace (since 2 aces hard == 22 points)
	hard_sum = soft_sum
	if ace_count > 0 then
		hard_sum = hard_sum + 10 -- (-1 + 11)
	end
	
	return soft_sum, hard_sum
end

function check_game()
	-- check game status of all players
	local users_done = true
	local users_lost = true -- all users lost
	for i = 1, table.maxn(user_list) do
		if user_list[i] ~= dealer_name and user_status[user_list[i]] ~= STATUS.INACTIVE then
			if user_status[user_list[i]] == STATUS.PLAYING then
				users_done = false
			elseif user_status[user_list[i]] == STATUS.WON or user_status[user_list[i]] == STATUS.STAND then
				users_lost = false
			end
		end
	end
	
	-- if all players are done, it's dealer deal time!
	local total_win_msg = dealer_name.." wins!"
	local win_msg = dealer_name.." wins over "
	local loose_msg = dealer_name.." looses against "
	local push_msg = dealer_name.." push against "
	if users_done then
		if users_lost then
			send_private_msg(blackjack_channel, total_win_msg)
		else
			while true do
				local sv, hv = count_cards_value(dealer_name)
				local dealer_blackjack = (sv == 21) or (hv == 21)
				-- deal cards until dealer has >= 17
				-- TODO: if all player points > 17 and <= 21 -> also dealer hit
				if sv < 17 and (hv > 21 or (hv <= 21 and hv < 17)) and (not dealer_blackjack) then
					deal_card(dealer_name, 1)
				else
					local dealer_lost = false
					if sv > 21 then
						dealer_lost = true
					end
					
					-- iterate over users and check who wins
					for i = 1, table.maxn(user_list) do
						if user_list[i] ~= dealer_name and user_status[user_list[i]] ~= STATUS.INACTIVE then
							-- 0 = push, 1 = dealer wins, 2 = user wins
							local outcome = 0 -- push = standard
							if user_status[user_list[i]] == STATUS.STAND then
								local usv, uhv = count_cards_value(user_list[i])
								if dealer_blackjack then
									outcome = 1
								elseif dealer_lost then
									outcome = 2
								else
									-- dealer hv must equal sv or is > 21 (since dealers hits on sv < 17),
									-- so we only have to check the players sv and hv
									-- also: sv <= 20, usv <= 20, uhv == unknown
									-- first: check for player win
									if sv < usv or (uhv <= 21 and sv < uhv) then
										outcome = 2
									-- second: check for player push
									elseif sv == usv or (uhv <= 21 and sv == uhv) then
										outcome = 0
									-- third: all cases checked, dealer must be the winner
									else
										outcome = 1
									end
								end
							elseif user_status[user_list[i]] == STATUS.WON then
								if not dealer_blackjack then
									outcome = 2
								end
							elseif user_status[user_list[i]] == STATUS.LOST then
								outcome = 1
							end
							
							--
							if outcome == 0 then
								send_private_msg(blackjack_channel, push_msg..user_list[i].."!")
							elseif outcome == 1 then
								send_private_msg(blackjack_channel, win_msg..user_list[i].."!")
							elseif outcome == 2 then
								send_private_msg(blackjack_channel, loose_msg..user_list[i].."!")
							end
						end
					end
					break
				end
			end
		end
		
		game_running = false
		-- TODO: start new round in 20/30 seconds
	end
end

function deal_card(user, amount)
	local msg = "deals "..user.." "
	for i = 1, amount do
		local suit = table.random(suits)
		local card = table.random(cards)
		local card_str = "["..suit..card.."]"
		table.insert(user_sum[user], card)
		
		if i > 1 then
			msg = msg .. " and "
		end
		msg = msg .. card_str
	end
	
	-- get card value
	local sv, hv = count_cards_value(user)
	msg = msg .. " - total: " .. tostring(sv)
	if sv ~= hv and hv <= 21 then
		msg = msg .. " or " .. tostring(hv)
	end
	send_action_msg(blackjack_channel, msg)
	
	-- check value
	if sv > 21 then
		user_status[user] = STATUS.LOST
		send_private_msg(blackjack_channel, user.." busts!")
	elseif sv == 21 or hv == 21 then
		user_status[user] = STATUS.WON
		send_private_msg(blackjack_channel, user..": Blackjack!")
	end
	
	--
	if user ~= dealer_name then
		check_game()
	end
end

function new_round()
	send_private_msg(blackjack_channel, "***** New Round *****")
	
	-- reset dealer and all players
	user_sum[dealer_name] = {}
	user_status[dealer_name] = STATUS.PLAYING
	for i = 1, table.maxn(user_list) do
		user_status[user_list[i]] = STATUS.PLAYING
		user_sum[user_list[i]] = {}
	end
	
	-- first dealer/bank card
	deal_card(dealer_name, 1)
	
	for i = 1, table.maxn(user_list) do
		if user_list[i] ~= dealer_name then
			deal_card(user_list[i], 2)
		end
	end
end

reset_blackjack()
		
function handle_message(origin, target, cmd, parameters)
	if (cmd == "blackjack" or cmd == "b") and cmd ~= parameters then
		-- get second cmd
		local blackjack_cmd = parameters
		local sep = string.find(parameters, "%s")
		if sep ~= nil then
			blackjack_cmd = string.sub(parameters, 0, sep-1)
		end
		
		-- Note: help and rules can be requested in a private qry to the bot,
		-- all other cmds have to come from the blackjack channel directly
		if blackjack_cmd == "join" and target == blackjack_channel then
			if not table.contains(user_list, origin) then
				table.insert(user_list, origin)
				user_sum[origin] = {}
				user_status[origin] = STATUS.INACTIVE
				
				send_private_msg(blackjack_channel, origin.." joins the table!")
			end
		elseif blackjack_cmd == "leave" and target == blackjack_channel then
			if table.contains(user_list, origin) then
				table.remove_elem(user_list, origin)
				send_private_msg(blackjack_channel, origin.." leaves the table!")
			end
		elseif (blackjack_cmd == "start" or blackjack_cmd == "restart") and target == blackjack_channel then
			if game_running ~= true and table.maxn(user_list) > 1 then
				game_running = true
				new_round()
			end
		elseif blackjack_cmd == "rules" then
			send_private_msg(origin, "http://en.wikipedia.org/wiki/Blackjack#Rules_of_play_against_a_casino")
		elseif blackjack_cmd == "kick" and is_owner(origin) and sep ~= nil and target == blackjack_channel then
			local len = string.len(parameters)
			if len > sep then
				local kick_user = string.sub(parameters, sep+1)
				if table.contains(user_list, kick_user) and kick_user ~= dealer_name then
					table.remove_elem(user_list, kick_user)
					send_action_msg(blackjack_channel, "removes "..kick_user.." from the table!")
					
					-- if only the dealer remains, stop the game
					if table.maxn(user_list) == 1 then
						game_running = false
					else
						-- check game/check if all players are done playing
						check_game()
					end
				end
			end
		elseif blackjack_cmd == "reset" and is_owner(origin) and target == blackjack_channel then
			reset_blackjack()
		elseif blackjack_cmd == "help" then
			send_private_msg(origin, "\002cmds\002: !blackjack <cmd> or !b <cmd>")
			send_private_msg(origin, "\002blackjack start\002: start new round")
			send_private_msg(origin, "\002blackjack join\002: join the table")
			send_private_msg(origin, "\002blackjack leave\002: leave the table")
			send_private_msg(origin, "\002blackjack hit\002: deal another card")
			send_private_msg(origin, "\002blackjack stand\002: no more card")
			send_private_msg(origin, "\002blackjack rules\002: blackjack rules")
			if is_owner(origin) then
				send_private_msg(origin, "\002blackjack kick <player>\002: removes <player> from the table")
				send_private_msg(origin, "\002blackjack reset\002: resets blackjack")
			end
		else
			-- inner game block
			if table.contains(user_list, origin) and user_status[origin] == STATUS.PLAYING and target == blackjack_channel then
				if blackjack_cmd == "hit" then
					deal_card(origin, 1)
				elseif blackjack_cmd == "stand" then
					user_status[origin] = STATUS.STAND
					check_game()
				end
			end
		end
	end
	return 0
end
