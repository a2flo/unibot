
-- !calc <problem>
-- requires: apcalc (on debian based systems) or calc

dofile (package.path.."include/global.lua")

function handle_message(origin, target, cmd, parameters)
        if cmd == "calc" then
                local sys = execute_command("calc \""..parameters.."\" 2>&1")
                send_private_msg(target, sys)
        end
        return 0
end
