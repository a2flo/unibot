
-- bot slap [name]

function handle_message(origin, target, cmd, parameters)
	if cmd == "slap" then
		local name = ( parameters ~= cmd ) and parameters or origin

		local random_quotes = {
			"schlaegt "..name.." mit einer alten Maus",
			"macht Bäuerchen auf "..name,
			"isst "..name.." auf",
			"testet Atomraketen an "..name,
			"haut "..name.." auf seinen dicken... Popo",
			"slaps "..name.." around a bit with a large trout",
			"slaps "..name.." around a little bit with a baseball bat",
			"pulls out "..name.."\'s eyeballs and plays baseball with them",
			"smacks a juicy red tomato over "..name.."\'s head",
			"flogs "..name.." up side the head with his old 286 computer",
			"slaps "..name.." around a while with his big autoexec.bat",
			"picks up a bazooka and shoots continuously at "..name,
			"slaps "..name.." with a bigger trout",
			"throws water balloons at "..name,
			"hugs "..name,
			"throws a knife at "..name.."\'s feet",
			"runs up behind "..name.." and gives a wedgeeeeeeeeeee ..Feel better NOW ?????",
			"slaps "..name.." around a bit with a slimy wet noodle",
			"sneaks in and reformats "..name.."\'s c drive",
			"slaps "..name.." with a James Brown single",
			"slaps "..name.." with a great big, wet, 100% rubber DUCK",
			"slaps "..name.." around a bit with a Texas Striper",
			"thinks "..name.." is the lamest thing that has ever walked the IRC!",
			"slaps "..name.." with an old used popup",
			"slaps "..name.." with an old diaper full of extricated chile burritos",
			"slaps "..name.." with a soft nearly clean t-shirt",
			"throws lemon cream pie at "..name,
			"slaps "..name.." with a squishy pair of old superman underoos",
			"slaps "..name.." around with a rotten trout",
			"puts a tight rope around "..name.."\'s neck and opens the channel trapdoor",
			"grabs "..name.." by the hair and jams him face first into the toilet",
			"hits "..name.." to death with a large sewing needle",
			"bashes "..name.." around the head with a small piano",
			"hits "..name.." with a mouse",
			"smashes "..name.." in the face with a large fork",
			"smashes "..name.." to death with a small cello",
			"smashes "..name.." to death with a tiny trout",
			"blats "..name.." around the head with a huge cello",
			"smacks "..name.." around the head with a small mosquito", 
			"smacks "..name.." repeatedly with a tiny fork",
			"beats "..name.." to death with a small mouse",
			"bashes "..name.." in the face with a huge mouse",
			"stabs "..name.." repeatedly with a bear",
			"bashes "..name.." repeatedly with a small vacuum",
			"beats "..name.." with a big list of IRC slaps",
			"haut "..name.." mit einem Löblichen aufn Deckel",
			"überollt "..name.." mit einem großen Panzer",
			"fesselt "..name.." an eine Laterne und geht weg",
			"pinkelt "..name.." ans Bein",
			"geht in eine Kinovorstellung: \""..name.."\'s fat ass\"", 
			"baut sich eine gravity gun und wirft einen Betonlastwagen auf "..name,
			"lässt "..name.." seine Banane auf den Tisch legen - *klack* - ich hoffe du kennst nen guten Arzt",
			"slaps "..name.." with a cup of warm honey-milk", 
			"schlägt "..name.." herum ein wenig mit Gummiente",
			"pokes "..name.." in the eye.",
			"discovered "..name.."\'s picture on uglypeople.com", 
			"pupst "..name.." an",
			"pupst "..name.." ins Gesicht", 
			"slaps a trout with a large "..name,
			"slaps "..name.." with a FUCKING large trout"
		}

		local rand = math.random(1, table.maxn(random_quotes))
		send_action_msg(target, random_quotes[rand])
	end
	return 0
end
