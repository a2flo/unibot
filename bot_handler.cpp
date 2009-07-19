/*
 *  UniBot
 *  Copyright (C) 2009 Florian Ziesche
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
			if(cmd_tokens.size() > 4) cmd_data2 = cmd_tokens[4];
			if(cmd_tokens.size() > 4) {
				for(unsigned int i = 4; i < cmd_tokens.size(); i++) {
					cmd_joined_data += " " + cmd_tokens[i]; // TODO: is there a better way in java to do this?
				}
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
					string user_str = cmd_joined_data.substr(cmd_joined_data.find(':') + 1,
															 cmd_joined_data.length() - cmd_joined_data.find(':') - (cmd_joined_data[cmd_joined_data.length()-1] == ' ' ? 2 : 1));
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
						n->send_channel_msg("hi there, " + strip_user(cmd_sender));
						states->add_user(strip_user(cmd_sender), strip_user_realname(cmd_sender), strip_user_host(cmd_sender));
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
				case PRIVMSG:
					// handle the message (also trim the leading ':')
					handle_message(cmd_sender, cmd_location, cmd_joined_data.substr(1, cmd_joined_data.length()-1));
					if(cmd_location == conf->get_channel()) {
						states->update_user(strip_user(cmd_sender), strip_user_realname(cmd_sender), strip_user_host(cmd_sender));
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
			
			logger::log(logger::LT_MSG, "bot_handler.cpp", string(string(string(">>>") + string(*data_iter)) + string("<<<")).c_str());
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
		unsigned int num = strtoul(cmd_tokens[1].c_str(), NULL, 10);
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
	// all bot commands have to start with '!'
	if((location == conf->get_channel() || conf->is_owner(strip_user(sender))) && msg[0] == '!') {
		msg = msg.substr(1, msg.length()-1);
		cout << "msg: " << msg << "<<<" << endl;
		
		// official commands
		if(msg == "help") {
			string helpless_person = strip_user(sender);
			n->send_private_msg(helpless_person, "help:");
			n->send_private_msg(helpless_person, "    !src: link to unibot source code");
			n->send_private_msg(helpless_person, "    !system: the bot's host system");
			n->send_private_msg(helpless_person, "    !time: local bot time");
			n->send_private_msg(helpless_person, "    !uptime: time since bot start");
			n->send_private_msg(helpless_person, "    !who's your daddy?: that would be me!");
			n->send_private_msg(helpless_person, "    !users: user list");
			n->send_private_msg(helpless_person, "    !wiki / !wikien: generates link to german / english wiki");
			n->send_private_msg(helpless_person, "    !quit: quits the bot (op only)");
			n->send_private_msg(helpless_person, "    !dict: generates link to dict.cc (german and english translations)");
			n->send_private_msg(helpless_person, "    !g: generates link to google");
			n->send_private_msg(helpless_person, "    !happa: generates link to MensaSpeiseplan for the next day");
			n->send_private_msg(helpless_person, "    !mensa: generates link to MensaSpeiseplan for the current day");
			n->send_private_msg(helpless_person, "    !wa: generates link to wolfram alpha");
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

			n->send_channel_msg(out.str());
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
			
			n->send_channel_msg(system_out);
		}
		else if(msg == "time") {
			stringstream local_time;
			const time_put<char>& tmput = use_facet<time_put<char> >(loc);
			time_t timestamp;
			time(&timestamp);	
			const char* pattern = "%H:%M:%S %Z %d.%m.%Y";
			tmput.put(local_time, local_time, ' ', localtime(&timestamp), pattern, pattern+strlen(pattern));
			n->send_channel_msg(local_time.str());
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
			
			n->send_channel_msg(uptime_str);
		}
		else if(msg.find("quit") == 0) {
			string in_bot_name = "";
			if(msg.length() > 5) in_bot_name = msg.substr(5, msg.length()-5);
			
			if(in_bot_name == "" || in_bot_name == conf->get_bot_name()) {
				if(conf->is_owner(strip_user(sender))) {
					quit_bot();
					states->set_quit(true);
				}
				else {
					if(states->is_op()) {
						n->send_kick(strip_user(sender), "you're no bot owner!");
					}
				}
			}
		}
		else if(msg == "src") {
			n->send_channel_msg("git : git clone git://git.assembla.com/unibot.git");
			n->send_channel_msg("trac: http://trac-git.assembla.com/unibot");
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
			n->send_channel_msg(out.str());
		}
		else if(msg.find("wiki ") == 0) {
			n->send_channel_msg("http://de.wikipedia.org/wiki/" + encode_url(msg.substr(5, msg.length()-5).c_str()));
		}
		else if(msg.find("wikien ") == 0) {
			n->send_channel_msg("http://en.wikipedia.org/wiki/" + encode_url(msg.substr(7, msg.length()-7).c_str()));
		}
		else if(msg.find("dict ") == 0) {
			n->send_channel_msg("http://www.dict.cc/?s=" + encode_url(msg.substr(5, msg.length()-5).c_str()));
		}
		else if(msg.find("g ") == 0) {
			n->send_channel_msg("http://www.google.de/search?q=" + encode_url(msg.substr(2, msg.length()-2).c_str()));
		}
		else if(msg == "happa") {
			n->send_channel_msg("http://happa.dfki.de/");
		}
		else if(msg == "mensa") {
			time_t cur_time;
			time(&cur_time);
			tm* time_info = localtime(&cur_time);
			const char* days[] = { "montag", "dienstag", "mittwoch", "donnerstag", "freitag" };
			int current_day = time_info->tm_wday;
			if(current_day >= 1 && current_day <= 5) {
				n->send_channel_msg("http://www.studentenwerk-saarland.de/seiten/verpflegung/speiseplan_sbr/" + string(days[current_day-1]) + ".htm");
			}
			else n->send_channel_msg("Heute ist die Mensa geschlossen ...");
		}
		else if(msg.find("wa ") == 0) {
			n->send_channel_msg("http://www.wolframalpha.com/input/?i=" + encode_url(msg.substr(3, msg.length()-3).c_str()));
		}
		else if(msg == "unikram") {
			n->send_channel_msg("https://pure-project.ssl.goneo.de/tdw/?n=u&s=unikram");
		}
		else if(msg == "upload") {
			n->send_channel_msg("https://pure-project.ssl.goneo.de/tdw/?n=c&s=uls");
		}
		else if(msg == "paste") {
			n->send_channel_msg("http://pastebin.com");
		}
		else if(msg == "quote") {
			if(conf->is_owner(strip_user(sender))) {
				//connection.send(msg.substring(4));
			}
		}
		// ... and the rest ;)
		else if(msg == "spec") {
			n->send_channel_msg("http://www.ietf.org/rfc/rfc2812.txt");
		}
		else if(msg.find("cmd ") == 0) {
			if(conf->is_owner(strip_user(sender))) {
				n->send(msg.substr(4, msg.length() - 4));
			}
		}
		else if(msg.find("learn ") == 0) {
		}
		else if(msg.find("what is ") == 0 && msg.find("?") != string::npos) {
		}
		else if(msg.find("what is the answer to") != string::npos || msg.find("what\'s the answer to") != string::npos || msg.find("?") != string::npos) {
			n->send_channel_msg("42");
		}
	}
}