/*
 *  UniBot
 *  Copyright (C) 2009 - 2010 Florian Ziesche
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "bot_handler.h"

const char* bot_handler::IRC_COMMAND_STR[] = {
	"NONE",
	"ADMIN",
	"AWAY",
	"CONNECT",
	"DIE",
	"ERROR",
	"INFO",
	"NVITE",
	"SON",
	"JOIN",
	"KICK",
	"KILL",
	"LINKS",
	"LIST",
	"LUSERS",
	"MODE",
	"MOTD",
	"NAMES",
	"NICK",
	"NOTICE",
	"OPER",
	"PART",
	"PASS",
	"PING",
	"PONG",
	"PRIVMSG",
	"QUIT",
	"REHASH",
	"RESTART",
	"SERVICE",
	"SERVLIST",
	"SERVER",
	"SQUERY",
	"SQUIT",
	"STATS",
	"SUMMON",
	"TIME",
	"TOPIC",
	"TRACE",
	"USER",
	"USERHOST",
	"USERS",
	"VERSION",
	"WALLOPS",
	"WHO",
	"WHOIS",
	"WHOWAS",
	"001",
	"002",
	"003",
	"004",
	"005",
	"353",
	"372"
};

bot_handler::bot_handler(net<TCP_protocol>* n, bot_states* states, config* conf) : n(n), states(states), conf(conf) {
	servername = "";
	
	start_time = SDL_GetTicks();
	keep_msg_count = 10;
	
	unsigned int cmd_size = sizeof(IRC_COMMAND_STR) / sizeof(const char*);
	for(unsigned int i = 0; i < cmd_size; i++) {
		irc_commands.insert(pair<string, IRC_COMMAND>(IRC_COMMAND_STR[i], (bot_handler::IRC_COMMAND)(bot_handler::NONE + i)));
	}
}

bot_handler::~bot_handler() {
}

void bot_handler::handle() {
	if(n->is_received_data()) {
		deque<string>* data = n->get_received_data();
		for (deque<string>::iterator data_iter = data->begin(); data_iter != data->end(); data_iter++) {
			// handle the data
			// cmd_tokens:
			// [0]: sender
			// [1]: command
			// [2]: location
			// [3+]: data
			IRC_COMMAND current_cmd = parse_irc_cmd(*data_iter);
			vector<string> cmd_tokens;
			tokenize(cmd_tokens, *data_iter, ' ');
			
			string cmd_sender = "", cmd_command = "", cmd_location = "", cmd_data = "", cmd_data2 = "", cmd_joined_data = "";
			if(cmd_tokens.size() > 0) cmd_sender = cmd_tokens[0];
			if(cmd_tokens.size() > 1) cmd_command = cmd_tokens[1];
			if(cmd_tokens.size() > 2) cmd_location = cmd_tokens[2];
			if(cmd_tokens.size() > 3) {
				cmd_data = cmd_tokens[3];
				cmd_joined_data = cmd_data;
			}
			if(cmd_tokens.size() > 4) {
				cmd_data2 = cmd_tokens[4];
				unsigned int pos = (unsigned int)(cmd_tokens[0].length()+cmd_tokens[1].length()+cmd_tokens[2].length()+3);
				cmd_joined_data = data_iter->substr(pos, data_iter->length()-pos);
			}
			
			switch(current_cmd) {
				case NONE:
					// ignore
					break;
				case CMD_001:
					states->set_connected(true);
					logger::log(logger::LT_DEBUG, "bot_handler.cpp", "successfully connected to the server!");
					n->send_identify(conf->get_bot_password());
					break;
				case CMD_004:
					servername = cmd_data;
					logger::log(logger::LT_DEBUG, "bot_handler.cpp", string(string("server name is: ") + servername).c_str());
					break;
				case CMD_353: {
					if(cmd_joined_data.find(':') == string::npos) break;
					// strip ':' and last ' ' (if there is one)
					string user_str = trim(cmd_joined_data.substr(cmd_joined_data.find(':') + 1, cmd_joined_data.length() - cmd_joined_data.find(':') - 1));
					vector<string> users;
					tokenize(users, user_str, ' ');
					states->delete_all_users();
					for(vector<string>::iterator user_iter = users.begin(); user_iter != users.end(); user_iter++) {
						if(user_iter->length() > 0) {
							if((*user_iter)[0] == '+' || (*user_iter)[0] == '@') {
								*user_iter = user_iter->substr(1, user_iter->length() - 1);
							}
							states->add_user(*user_iter, "", "");
						}
					}
				}
				break;
				case PING:
					n->send("PONG " + servername);
					logger::log(logger::LT_DEBUG, "bot_handler.cpp", "PONG!");
					break;
				case NOTICE:
					if(cmd_joined_data.find("You are now identified for", 0) != string::npos && cmd_joined_data.find(conf->get_bot_name(), 0) != string::npos) {
						states->set_identified(true);
					}
					break;
				case JOIN:
					// if the bot joined the channel, set the flag and send a "hi there ;)" message
					if(strip_user(cmd_sender) == conf->get_bot_name()) {
						logger::log(logger::LT_DEBUG, "bot_handler.cpp", "joined the channel");
						states->set_joined(true);
						n->send_channel_msg("hi there ;)");
						
						// check if bot got kicked
						if(states->is_kicked()) {
							// random kick message
							const char* kick_messages[] = { "try me!", "booyah!", "bot pwnage!", "don't mess with me!", "nice try!" };
							
							// wait for 2 seconds before kicking the user
							SDL_Delay(2000);
							
							cout << "kicking: " << states->get_kick_user() << endl;
							n->send_kick(states->get_kick_user(), kick_messages[rand() % (sizeof(kick_messages) / sizeof(const char*))]);
						}
					}
					// if another user joined the channel, greet him
					else {
						if(!(states->is_silenced())) {
							n->send_channel_msg("hey, " + strip_user(cmd_sender));
							states->add_user(strip_user(cmd_sender), strip_user_realname(cmd_sender), strip_user_host(cmd_sender));
						}
					}
					break;
				case PART:
					states->delete_user(strip_user(cmd_sender));
					if(strip_user(cmd_sender) == conf->get_bot_name()) {
						states->set_parted(true);
					}
					break;
				case QUIT:
					states->delete_user(strip_user(cmd_sender));
					break;
				case PRIVMSG: {
					// handle the message (also trim the leading ':')
					string msg = cmd_joined_data.substr(1, cmd_joined_data.length()-1);
					handle_message(cmd_sender, cmd_location, msg);
					if(cmd_location == conf->get_channel()) {
						// update msg store
						if(msg_store.size() > keep_msg_count) {
							msg_store.pop_front();
						}
						msg_store.push_back(strip_special_chars(cmd_joined_data));
						
						// update user info
						states->update_user(strip_user(cmd_sender), strip_user_realname(cmd_sender), strip_user_host(cmd_sender));
						
						// log msg
						logger::log(logger::LT_MSG, "bot_handler.cpp", string(strip_user(cmd_sender) + ": " + msg).c_str());
					}
					// URL - Title
					//if (msg.find("http") == 0) {
					//	n->send_channel_msg(msg);
					//}
				}
				break;
				case MODE:
					// only handle mode stuff in the bots host channel
					if(cmd_location == conf->get_channel()) {
						// mode for the bot was set
						if(cmd_data2 == conf->get_bot_name()) {
							// bot was given op
							if(cmd_data == "+o") {
								states->set_op(true);
							}
							// op was taken from bot
							if(cmd_data == "-o") {
								states->set_op(false);
							}
						}
					}
					break;
				case KICK:
					// check if bot was kicked
					if(cmd_data == conf->get_bot_name()) {
						// check if the user who kicked the bot is the owner
						if(conf->is_owner(strip_user(cmd_sender))) {
							// if not, rejoin the channel and kick the user who kicked the bot
							n->join_channel(conf->get_channel());
							
							states->set_kick_user(strip_user(cmd_sender));
							cout << "to get kicked: " << states->get_kick_user() << endl;
							states->set_kicked(true);
						}
					}
					else {
						states->delete_user(cmd_data);
					}
					break;
				case NICK:
					states->update_user(strip_user(cmd_sender), cmd_location.substr(1, cmd_location.length()-1));
					break;
				case TOPIC:
					break;
				default:
					break;
			}
			
			if(states->is_parted() && states->is_quit()) {
				n->quit();
			}
			
			logger::log(logger::LT_DEBUG, "bot_handler.cpp", string(*data_iter).c_str());
		}
		n->clear_received_data();
	}
}

bot_handler::IRC_COMMAND bot_handler::parse_irc_cmd(string cmd) {
	vector<string> cmd_tokens;
	tokenize(cmd_tokens, cmd, ' ');
	IRC_COMMAND irc_cmd = bot_handler::NONE;
	
	// check for normal irc command, some commands are in the first token ...
	if(cmd_tokens.size() > 0 && irc_commands.count(cmd_tokens[0]) > 0) {
		irc_cmd = irc_commands[cmd_tokens[0]];
	}
	else if(cmd_tokens.size() > 1 && irc_commands.count(cmd_tokens[1]) > 0) {
		irc_cmd = irc_commands[cmd_tokens[1]];
	}
	else if(cmd_tokens.size() > 1 && cmd_tokens[1].length() == 3) {
		// check for numbered irc command
		unsigned int num = (unsigned int)strtoul(cmd_tokens[1].c_str(), NULL, 10);
		if(num >= 0 && num <= 999) {
			irc_cmd = irc_commands[cmd_tokens[1]];
		}
	}
	
	return irc_cmd;
}

string bot_handler::strip_user(string str) {
	// ":name!..." -> name
	if(str.length() < 5) return "";
	if(str.find("!", 0) < 1) return "";
	if(str.find("!", 0) == string::npos) return "";
	return str.substr(1, str.find("!", 0)-1);
}

string bot_handler::strip_user_realname(string str) {
	if(str.length() < 6) return "";
	
	size_t eq_pos = 0, at_pos = 0;
	if((eq_pos = str.find("=", 0)) == string::npos || (at_pos = str.rfind("@")) == string::npos) return "";
	if(at_pos < eq_pos) return "";
	return str.substr(eq_pos+1, at_pos - eq_pos - 1);
}

string bot_handler::strip_user_host(string str) {
	if(str.length() < 8) return "";
	
	size_t at_pos = 0;
	if((at_pos = str.rfind("@")) == string::npos) return "";
	return str.substr(at_pos+1, str.length() - at_pos);
}

void bot_handler::quit_bot() {
	n->part();
}

void bot_handler::handle_message(string sender, string location, string msg) {
	string origin = strip_user(sender);
	string target = (location == conf->get_bot_name() ? origin : location);
	// all bot commands have to start with '!'
	// only accept messages originating from the channel or from a user inside the channel
	if((location == conf->get_channel() || states->is_user(origin)) && msg[0] == '!') {
		msg = msg.substr(1, msg.length()-1);
		cout << "msg: " << msg << "<<<" << endl;
		
		// official commands
		if(msg == "help") {
			// always send help info directly to the user/origin (privmsg)
			
			/*
			n->send_private_msg(origin, "help:");
			n->send_private_msg(origin, "    !src: link to unibot source code");
			n->send_private_msg(origin, "    !system: the bot's host system");
			n->send_private_msg(origin, "    !time: local bot time");
			n->send_private_msg(origin, "    !uptime: time since bot start");
			n->send_private_msg(origin, "    !who's your daddy?: that would be me!");
			n->send_private_msg(origin, "    !users: user list");
			n->send_private_msg(origin, "    !quit: quits the bot (owner only)");
			n->send_private_msg(origin, "    !happa: generates link to MensaSpeiseplan for the next day");
			n->send_private_msg(origin, "    !mensa: generates link to MensaSpeiseplan for the current day");
			n->send_private_msg(origin, "    !version: prints out the unibot version");
			n->send_private_msg(origin, "    !wd / !we <args1>: generates link to german / english wikipedia");
			n->send_private_msg(origin, "    !dict <args1>: generates link to dict.cc (german and english translations)");
			n->send_private_msg(origin, "    !g <args1>: generates link to google");
			n->send_private_msg(origin, "    !wa <args1>: generates link to wolfram alpha");
			n->send_private_msg(origin, "    <args1>: <message offset> <word offset>: extracts the word (given by word offset) or whole msg (if no word offset) " \
								"of the msg specified by message offset (in reverse)");
			*/
//			n->send_channel_msg("https://pure-project.ssl.goneo.de/tdw/unibot");

			n->send_private_msg(origin, "Help (add a ! to use a command):");
			n->send_private_msg(origin, "Use '!help <command>' to get further information about this command. // coming soon");
			n->send_private_msg(origin, "    Links:       wd, we, wa, uw, ae, ad, uu, g, dict");
			n->send_private_msg(origin, "    Bot/Channel: who's your daddy?, system, time, uptime, src, spec, users, quit, roulette, version, kick, silence");
			n->send_private_msg(origin, "    Uni:         unikram, paste, upload, mensa, happa, mfi, prog, coli, theoinf, algodat, courses");
			n->send_private_msg(origin, "    Misc:        learn, rev, ?");
			n->send_private_msg(origin, "    <args1>: <message offset> <word offset>: extracts the word (given by word offset) or whole msg (if no word offset) " \
								"of the msg specified by message offset (in reverse)");
		}
		else if(msg == "who\'s your daddy?") {
			stringstream out;
			vector<string> owner_names = conf->get_owner_names();
			for(vector<string>::iterator owner_iter = owner_names.begin(); owner_iter != owner_names.end(); owner_iter++) {
				out << *owner_iter;
				pair<string, string> owner_info = states->get_user(*owner_iter);
				if(owner_info.second != "") {
					IPaddress owner_address;
					SDLNet_ResolveHost(&owner_address, owner_info.second.c_str(), 80);
					unsigned int host = owner_address.host;
					string addr = to_str(host & 0xFF) + "." + to_str((host >> 8) & 0xFF) + "." + to_str((host >> 16) & 0xFF) + "." + to_str((host >> 24) & 0xFF);
					out << " @" << addr;
				}
				if(owner_iter+1 != owner_names.end()) out << ", ";
			}

			n->send_private_msg(target, out.str());
		}
		else if(msg == "system") {
			// TODO: add support for non-unix (aka windows) platforms?
			string system_out = "";
			char buffer[512];
			memset(buffer, 0, 512);
			FILE* sys_pipe = popen("uname -a", "r");
			while(fgets(buffer, 512, sys_pipe) != NULL) {
				system_out += buffer;
				memset(buffer, 0, 512);
			}
			pclose(sys_pipe);
			
			n->send_private_msg(target, system_out);
		}
		else if(msg == "time") {
			stringstream local_time;
			const time_put<char>& tmput = use_facet<time_put<char> >(loc);
			time_t timestamp;
			time(&timestamp);	
			const char* pattern = "%H:%M:%S %Z %d.%m.%Y";
			tmput.put(local_time, local_time, ' ', localtime(&timestamp), pattern, pattern+strlen(pattern));
			n->send_private_msg(target, local_time.str());
		}
		else if(msg == "uptime") {
			unsigned long int uptime = SDL_GetTicks() - start_time;
			string uptime_str = "";
			
			unsigned long int t_day = 1000*60*60*24;
			unsigned long int t_hour = 1000*60*60;
			unsigned long int t_minute = 1000*60;
			unsigned long int t_second = 1000;
			
			uptime_str += to_str(uptime / t_day) + "d ";
			uptime %= t_day;
			uptime_str += to_str(uptime / t_hour) + "h ";
			uptime %= t_hour;
			uptime_str += to_str(uptime / t_minute) + "m ";
			uptime %= t_minute;
			uptime_str += to_str(uptime / t_second) + "s";
			uptime %= t_second;
			
			n->send_private_msg(target, uptime_str);
		}
		else if(msg.find("quit") == 0) {
			string in_bot_name = "";
			if(msg.length() > 5) in_bot_name = msg.substr(5, msg.length()-5);
			
			if(in_bot_name == "" || in_bot_name == conf->get_bot_name()) {
				if(conf->is_owner(origin)) {
					quit_bot();
					states->set_quit(true);
				}
				else {
					if(states->is_op()) {
						n->send_kick(origin, "you're no bot owner!");
					}
				}
			}
		}
		else if(msg == "src") {
			n->send_private_msg(target, "git : git clone git://git.assembla.com/unibot.git");
			n->send_private_msg(target, "trac: http://trac-git.assembla.com/unibot");
		}
		else if(msg == "users") {
			map<string, pair<string, string> >* users = states->get_users();
			stringstream out;
			for(map<string, pair<string, string> >::iterator user_iter = users->begin(); user_iter != users->end();) {
				out << user_iter->first;
				if(user_iter->second.first.length() > 0 || user_iter->second.second.length() > 0) {
					out << " (";
					if(user_iter->second.first.length() > 0) out << user_iter->second.first << ", ";
					if(user_iter->second.second.length() > 0) out << user_iter->second.second;
					out << ")";
				}
				if(++user_iter != users->end()) out << ", ";
			}
			n->send_private_msg(target, out.str());
		}
		else if(msg.find("wd ") == 0 || (msg.length() == 2 && msg.find("wd") == 0)) {
			msg = handle_args_chronological(msg, 3);
			n->send_private_msg(target, "http://de.wikipedia.org/wiki/" + encode_url(msg));
		}
		else if(msg.find("we ") == 0 || (msg.length() == 2 && msg.find("we") == 0)) {
			msg = handle_args_chronological(msg, 3);
			n->send_private_msg(target, "http://en.wikipedia.org/wiki/" + encode_url(msg));
		}
		else if(msg.find("dict ") == 0 || (msg.length() == 4 && msg.find("dict") == 0)) {
			msg = handle_args_chronological(msg, 5);
			n->send_private_msg(target, "http://www.dict.cc/?s=" + encode_url(msg));
		}
		else if(msg.find("g ") == 0 || (msg.length() == 1 && msg.find("g") == 0)) {
			msg = handle_args_chronological(msg, 2);
			n->send_private_msg(target, "http://www.google.de/search?q=" + encode_url(msg));
		}
		else if(msg.find("wa ") == 0 || (msg.length() == 2 && msg.find("wa") == 0)) {
			msg = handle_args_chronological(msg, 3);
			n->send_private_msg(target, "http://www.wolframalpha.com/input/?i=" + encode_url(msg));
		}
		// Unikram
		else if(msg == "happa") {
			n->send_private_msg(target, "http://happa.dfki.de/");
		}
		else if(msg == "mensa") {
			time_t cur_time;
			time(&cur_time);
			tm* time_info = localtime(&cur_time);
			const char* days[] = { "montag", "dienstag", "mittwoch", "donnerstag", "freitag" };
			int current_day = time_info->tm_wday;
			if(current_day >= 1 && current_day <= 5) {
				n->send_private_msg(target, "http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/" + string(days[current_day-1]) + ".htm");
			}
			else n->send_private_msg(target, "Heute ist die Mensa geschlossen ...");
		}
		else if(msg == "unikram") {
			n->send_private_msg(target, "https://pure-project.ssl.goneo.de/tdw/?n=u&s=unikram");
		}
		else if(msg == "upload") {
			n->send_private_msg(target, "https://pure-project.ssl.goneo.de/tdw/?n=c&s=uls");
		}
		else if(msg == "paste") {
			n->send_private_msg(target, "https://pure-project.ssl.goneo.de/tdw/?n=c&s=ps");
		}
		else if(msg == "mfi") {
			n->send_private_msg(target, "http://www.math.uni-sb.de/ag/schreyer/LEHRE/0910_MfI3/index.html");
		}
		else if(msg == "prog") {
			n->send_private_msg(target, "https://depend.cs.uni-sb.de/cms/login.php");
		}
		else if(msg == "coli") {
			n->send_private_msg(target, "Psycho: http://www.coli.uni-saarland.de/~berryc/courses/vl_psycholing_ws0910/vl_psycholing_ws0910.html");
			n->send_private_msg(target, "Einfuehrung: http://www.coli.uni-saarland.de/courses/I2CL-09/page.php?id=index");
		}
		else if (msg == "theoinf") {
			n->send_private_msg(target, "http://www-tcs.cs.uni-sb.de/Veranstaltungen/vorlesung.php?CourseId=40");
		}
		else if (msg == "algodat") {
			n->send_private_msg(target, "http://www-cc.cs.uni-saarland.de/teaching/course.php?CourseId=16");
		}
		else if (msg == "courses") {
			n->send_private_msg(target, "http://www.prog.uni-saarland.de/teaching/ckurs/2009/");
		}
//		else if (msg == "sysarch") {
//			n->send_private_msg(target, "");
//		}
		// Unikram End
		else if(msg == "quote") {
			if(conf->is_owner(origin)) {
				//connection.send(msg.substring(4));
			}
		}
		else if(msg.find("kick ") == 0) {
			if(conf->is_owner(origin) && msg.length() > 5) {
				n->send_kick(msg.substr(5, msg.length()-5), "cause i can!");
			}
		}
		else if(msg.find("roulette ") == 0) {
			if(conf->is_owner(origin) && msg.length() > 9) {
				if((rand() % 42) <= 21) n->send_kick(msg.substr(9, msg.length()-9), "bad luck!");
			}
		}
		else if(msg.find("version") == 0) {
			n->send_private_msg(target, "UniBot "+to_str(sizeof(void*) == 4 ? "x86" : (sizeof(void*) == 8 ? "x64" : "unknown"))+" v"+to_str(UNIBOT_MAJOR_VERSION)+"."+
								to_str(UNIBOT_MINOR_VERSION)+"."+to_str(UNIBOT_REVISION_VERSION)+"-"+to_str(UNIBOT_BUILD_VERSION)+" ("+UNIBOT_BUILD_DATE+" "+UNIBOT_BUILD_TIME+")");
		}
		else if(msg.find("silence") == 0) {
			if(conf->is_owner(origin)) {
				states->set_silenced(states->is_silenced() ^ true);
			}	
		}
		// ... and the rest ;)
		else if(msg == "spec") {
			n->send_private_msg(target, "http://www.ietf.org/rfc/rfc2812.txt");
		}
		else if(msg.find("cmd ") == 0) {
			if(conf->is_owner(origin)) {
				n->send(msg.substr(4, msg.length() - 4));
			}
		}
		else if(msg.find("learn ") == 0) {
		}
		else if(msg.find("what is ") == 0 && msg.find("?") != string::npos) {
		}
		else if(msg.find("what is the answer to") != string::npos || msg.find("what\'s the answer to") != string::npos || msg.find("?") != string::npos) {
			n->send_private_msg(target, "42");
		}
		else if(msg.find("rev ") == 0) {
			msg = handle_args_chronological(msg, 4);
			n->send_private_msg(target, rev(msg));
		}
	}
}

string bot_handler::handle_args_chronological(string msg, unsigned int offset) {
	if(msg.length() > offset) {
		string args = msg.substr(offset, msg.length()-offset);
		vector<string> arg_tokens;
		tokenize(arg_tokens, args, ' ');
		int msg_offset = 0, word_offset = 0;
		if(arg_tokens.size() > 0) msg_offset = (unsigned int)strtoul(arg_tokens[0].c_str(), NULL, 10);
		if(arg_tokens.size() > 1) word_offset = (unsigned int)strtoul(arg_tokens[1].c_str(), NULL, 10);
		
		if(msg_offset != 0) {
			msg = extract_word(msg_offset, word_offset);
		}
		if(msg_offset == 0 || msg == "") msg = args;
	}
	else {
		// get longest word of last message
		vector<string> last_msg_tokens;
		tokenize(last_msg_tokens, msg_store.back(), ' ');
		msg = "";
		for(vector<string>::iterator str_iter = last_msg_tokens.begin(); str_iter != last_msg_tokens.end(); str_iter++) {
			if(str_iter->length() > msg.length()) msg = *str_iter;
		}
	}
	return msg;
}

string bot_handler::extract_word(int msg_offset, int word_offset) {
	msg_offset = abs(msg_offset);
	unsigned int abs_word_offset = abs(word_offset);
	
	int msg_number = (int)msg_store.size() - msg_offset;
	if(msg_number >= 0 && msg_number < (int)msg_store.size()) {
		string msg = msg_store[msg_number];
		
		if(word_offset == 0) return msg;
		
		vector<string> msg_tokens;
		tokenize(msg_tokens, msg, ' ');
		
		if(word_offset > (int)msg_tokens.size()) return msg_tokens.back();
		if(abs_word_offset > msg_tokens.size()) return msg_tokens[0];
		
		int word_number = word_offset > 0 ? word_offset-1 : ((int)msg_tokens.size() + word_offset);
		return msg_tokens[word_number];
	}
	
	return "";
}

string bot_handler::strip_special_chars(string str) {
	//string special_chars = ",.;:!?=*^<>\"\'";
	string special_chars = ",.;:!?^\"\'";
	string new_str = "";
	for(string::iterator citer = str.begin(); citer != str.end(); citer++) {
		if(special_chars.find(*citer) == string::npos) new_str += *citer;
	}
	return new_str;
}
