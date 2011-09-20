
-- lua add something to a file
-- note: folder output has to exist (in unibot/)

require "global"

function handle_message(origin, target, cmd, parameters)
	if is_owner(origin) and cmd == "add" then
		local name = "output/outputfile-"..os.time()
		io.open(name, a)
		io.output(name)
		io.write(parameters)
		io.close()
	end
	return 0
end
