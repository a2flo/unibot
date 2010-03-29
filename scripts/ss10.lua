
-- lua ss10 links

function handle_message(origin, target, cmd, parameters)
	if cmd == "ss10" then
		send_private_msg(target, "colimathe: http://www.coli.uni-saarland.de/kvv/detailpage.php?id=1577")
		send_private_msg(target, "crypto:    http://www.infsec.cs.uni-sb.de/teaching/10SS/Cryptography/")
		send_private_msg(target, "infosys:   http://infosys.cs.uni-sb.de/is10.php")
		send_private_msg(target, "np:        http://d.cs.uni-sb.de/index.php?527&L=1")
		send_private_msg(target, "sysarch:   http://www-wjp.cs.uni-saarland.de/lehre/vorlesung/info2/ss10/index.php")
	end
	return 0
end
