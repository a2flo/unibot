
-- lua ss10 links

function handle_message(origin, target, cmd, parameters)
	if cmd == "ss10" then
		send_private_msg(target, "colimathe: http://www.coli.uni-saarland.de/kvv/detailpage.php?id=1577")
		send_private_msg(target, "crypto:    http://www.infsec.cs.uni-sb.de/teaching/10SS/Cryptography/")
		send_private_msg(target, "infosys:   http://infosys.cs.uni-sb.de/is10.php")
		send_private_msg(target, "np:        http://d.cs.uni-sb.de/index.php?527&L=1")
		send_private_msg(target, "ppp:       http://www.st.cs.uni-saarland.de/edu/parallel10/ppm10.html")
		send_private_msg(target, "sysarch:   http://www-wjp.cs.uni-saarland.de/lehre/vorlesung/info2/ss10/index.php")
		send_private_msg(target, "unix:      http://www.prog.uni-saarland.de/teaching/unix-I/SS2010/Unix-Kurs_SS2010.de.html")
		send_private_msg(target, "atdtrp:    http://ivda.cs.uni-sb.de/index.php?id=57&type=1")
		send_private_msg(target, "sci-vis:   http://ivda.cs.uni-sb.de/index.php?id=54&type=1")
	end
	return 0
end
