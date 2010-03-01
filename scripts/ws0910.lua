
-- lua ws0910 links

function handle_message(origin, target, cmd, parameters)
	if cmd == "ws0910" then
		send_private_msg(target, "MfI3:       http://www.math.uni-sb.de/ag/schreyer/LEHRE/0910_MfI3/index.html")
		send_private_msg(target, "C/C++ Kurs: http://www.prog.uni-saarland.de/teaching/ckurs/2009/")
		send_private_msg(target, "TheoInf:    http://www-tcs.cs.uni-sb.de/Veranstaltungen/vorlesung.php?CourseId=40")
		send_private_msg(target, "AlgoDat:    http://www-cc.cs.uni-saarland.de/teaching/course.php?CourseId=16")
		send_private_msg(target, "PsychoColi: http://www.coli.uni-saarland.de/~berryc/courses/vl_psycholing_ws0910/vl_psycholing_ws0910.html")
		send_private_msg(target, "ColiEinf:   http://www.coli.uni-saarland.de/courses/I2CL-09/page.php?id=index")
	end
	return 0
end
