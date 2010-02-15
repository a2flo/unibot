
-- lua bindings test

function xor(b1, b2)
	return (b1 or b2) and (not (b1 and b2))
end

function handle_message(origin, target, cmd, parameters)
	if cmd == "test" then
		local channel = get_config_entry("channel")
		
		-- net
		send("PRIVMSG "..channel.." :send test")
		send_channel_msg("send_channel_msg test")
		send_private_msg(channel, "send_private_msg test")
		send_kick("tester", "42")
		local host_ip, host_port = get_local_ip()
		local server_ip, server_port = get_server_ip()
		send_channel_msg("local: "..host_ip..":"..host_port..", server: "..server_ip..":"..server_port)
		
		-- bot states
		local silenced = get_bot_state("silenced")
		set_bot_state("silenced", xor(silenced, true))
		local new_silenced = get_bot_state("silenced")
		send_channel_msg("bot state test: "..tostring(silenced)..", "..tostring(new_silenced))
		
		-- config
		local owners = get_config_entry("owner_names")
		send_channel_msg("config test, owner names: "..owners)
		
		local is_owner1 = is_owner("[flo]")
		local is_owner2 = is_owner("tester")
		send_channel_msg("config test, is owner: [flo]: "..tostring(is_owner1)..", tester: "..tostring(is_owner2))
		
		-- misc
		local test_url = "http://www.google.com/?!? blah blubb /";
		send_channel_msg("url: "..test_url..", encoded: "..url_encode(test_url))
		send_channel_msg("prev msg: "..get_prev_msg(1))
		
		local sc_str = "s,t.r;i:n!g w?/^ \"special\'chars!"
		send_channel_msg("strip_special_chars: "..strip_special_chars(sc_str))
		
		send_channel_msg("handle_args_chronological: "..handle_args_chronological("-1"))
	end
	return 0
end