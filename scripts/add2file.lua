
-- lua add something to a file

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
