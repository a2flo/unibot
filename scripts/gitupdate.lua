
-- !gitupdate, calls 'git pull origin master' (if possible, if the current bot binary folder also
-- contains a unibot git checkout) and reloads all scripts

require "global"

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) then
		if cmd == "gitupdate" then
			-- git pull
			local git_output = execute_command("git pull origin master 2>&1")
			
			-- check for error (strings)
			local err_strs = {
								string.find(git_output, "command not found", 1, true),
								string.find(git_output, "Already up-to-date", 1, true),
								string.find(git_output, "fatal: Not a git repository", 1, true),
								string.find(git_output, "fatal", 1, true),
								string.find(git_output, "error", 1, true)
			}
			
			for i = 1, #err_strs do
				if err_strs[i] ~= nil then
					-- git update error, send output and return
					send_private_msg(target, git_output)
					return 0
				end
			end
			
			-- everything went well, reload scripts
			-- note that this effectively kills all lua states and current executions, including this one
			send_private_msg(target, "git pull successful - reloading scripts")
			reload_scripts()
		end
	end
	return 0
end
