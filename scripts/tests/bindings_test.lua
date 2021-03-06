
-- lua bindings test

require "global"

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) and cmd == "bindings_test" then
		local channel = get_config_entry("channel")
		
		-- net
		send("PRIVMSG "..target.." :send test")
		send_channel_msg("send_channel_msg test")
		send_private_msg(target, "send_private_msg test")
		send_action_msg(target, "is a bot")
		send_kick("tester", "42")
		local host_ip, host_port = get_local_ip()
		local server_ip, server_port = get_server_ip()
		send_private_msg(target, "local: "..host_ip..":"..host_port..", server: "..server_ip..":"..server_port)
		
		-- bot states
		local silenced = get_bot_state("silenced")
		set_bot_state("silenced", xor(silenced, true))
		local new_silenced = get_bot_state("silenced")
		send_private_msg(target, "bot state test: "..tostring(silenced)..", "..tostring(new_silenced))
		set_bot_state("silenced", xor(silenced, true))
		
		-- config
		local owners = get_config_entry("owner_names")
		send_private_msg(target, "config test, owner names: "..owners)
		
		local is_owner1 = is_owner("[flo]")
		local is_owner2 = is_owner("tester")
		send_private_msg(target, "config test, is owner: [flo]: "..tostring(is_owner1)..", tester: "..tostring(is_owner2))
		
		-- misc
		local test_url = "https://www.google.com/?!? blah blubb /";
		send_private_msg(target, "url: "..test_url..", encoded: "..url_encode(test_url))
		send_private_msg(target, "prev msg: "..get_prev_msg(1))
		
		local sc_str = "s,t.r;i:n!g w?/^ \"special\'chars!"
		send_private_msg(target, "strip_special_chars: "..strip_special_chars(sc_str))
		
		send_private_msg(target, "handle_args_chronological: "..handle_args_chronological("-1"))
		
		-- internal funcs
		local test_table = { "blah", "blubb", "test", "blah", "hmm", "blah", "blubb", "blubb" }
		testtable = table.unique(test_table)
		send_private_msg(target, "unique: "..table.concat(test_table, ", "))
		
		reload_scripts()
	end
	return 0
end
