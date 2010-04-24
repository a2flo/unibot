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
#include "lua.h"

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
	"352",
	"353",
	"372"
};

bot_handler::bot_handler(unibot_irc_net* n, bot_states* states, config* conf) : thread_base(), n(n), states(states), conf(conf) {
	servername = "";
	
	start_time = SDL_GetTicks();
	keep_msg_count = numeric_limits<size_t>::max();
	
	unsigned int cmd_size = sizeof(IRC_COMMAND_STR) / sizeof(const char*);
	for(unsigned int i = 0; i < cmd_size; i++) {
		irc_commands.insert(pair<string, IRC_COMMAND>(IRC_COMMAND_STR[i], (bot_handler::IRC_COMMAND)(bot_handler::NONE + i)));
	}
	
	lua_obj = new lua(n, this, states, conf);
	
	this->start(); // start thread
}

bot_handler::~bot_handler() {
}

void bot_handler::run() {
	if(n->is_running() && n->is_received_data()) {
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
				case CMD_352:
					// this might get nasty, catch exceptions, just to be on the safe side
					try {
						size_t colon_pos = cmd_joined_data.find(":");
						if(colon_pos == string::npos) break;
						
						size_t space_pos = cmd_joined_data.find(" ", colon_pos);
						string real_name = (space_pos == string::npos ? cmd_joined_data.substr(colon_pos+1, cmd_joined_data.length()-colon_pos-1) :
											cmd_joined_data.substr(space_pos+1, cmd_joined_data.length()-space_pos-1));
						
						states->update_user_info(cmd_tokens[7], real_name, cmd_tokens[5], cmd_tokens[4], "", "", "");
					}
					catch(...) {
						logger::log(logger::LT_ERROR, "bot_handler.cpp", string("CMD_352 failed").c_str());
					}
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
							states->add_user(*user_iter);
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
						n->join_channel(conf->get_channel());
					}
					else if(strip_user(cmd_sender) == "NickServ") {
						// acc <nick> info, ACC 3 == nick/user is registered and identified
						if(cmd_joined_data.find("ACC 3") != string::npos) {
							// :<nick> ACC 3
							string reg_user = cmd_joined_data.substr(1, cmd_joined_data.find(" ")-1);
							states->update_user_info(reg_user, "", "", "", "yes", "", "");
						}
						else if(cmd_joined_data.find(" ACC ") != string::npos) {
							// :<nick> ACC # (# != 3) == unregistered
							string reg_user = cmd_joined_data.substr(1, cmd_joined_data.find(" ")-1);
							states->update_user_info(reg_user, "", "", "", "no", "", "");
						}
					}
					// ctcp notice
					else if(cmd_joined_data.length() >= 2 &&
							(cmd_joined_data[0] == ':' && cmd_joined_data[1] == 0x01 && cmd_joined_data[cmd_joined_data.length()-1] == 1)) {
						string ctcp_msg = cmd_joined_data.substr(2, cmd_joined_data.length()-3);
						
						size_t space_pos = ctcp_msg.find(" ");
						if(space_pos != string::npos && space_pos < ctcp_msg.length()-1) {
							string ctcp_type = ctcp_msg.substr(0, space_pos);
							string ctcp_info = ctcp_msg.substr(space_pos+1, ctcp_msg.length()-space_pos-1);
							
							if(ctcp_type == "VERSION") {
								states->update_user_info(strip_user(cmd_sender), "", "", "", "", "yes", ctcp_info);
							}
						}
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
						}
						states->add_user(strip_user(cmd_sender));
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
						states->update_user_info(strip_user(cmd_sender), strip_user_realname(cmd_sender), strip_user_host(cmd_sender), "", "", "", "");
						
						// log msg
						logger::log(logger::LT_MSG, "bot_handler.cpp", string(strip_user(cmd_sender) + ": " + msg).c_str());
					}
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
					states->update_user_name(strip_user(cmd_sender), cmd_location.substr(1, cmd_location.length()-1));
					break;
				case TOPIC:
					break;
				default:
					break;
			}
			
			// TODO: add lua events
			
			if(states->is_parted() && states->is_quit()) {
				n->quit();
				
				// and finally finish both threads
				set_thread_should_finish();
				n->set_thread_should_finish();
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
		if(num <= 999) {
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
		
		lua_obj->handle_message(origin, target, msg);
		
		// official commands
		size_t cmd_end = msg.find(' ', 0);
		string cmd = (cmd_end != string::npos ? msg.substr(0, cmd_end) : msg);
		if(cmd == "uptime") {
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
		else if(cmd == "version") {
			n->send_private_msg(target, UNIBOT_VERSION_STRING);
		}
		else if(cmd == "reload_scripts") {
			lua_obj->reload_scripts();
		}
		else if(msg.find("reload_script ") == 0 && msg.length() > 14) {
			string script = msg.substr(14, msg.length()-14);
			lua_obj->reload_script(script);
			logger::log(logger::LT_DEBUG, "bot_handler.cpp", ("script "+script+" reloaded!").c_str());
		}
	}
	
	// handle ctcp messages that start and end with 0x01 (these can come from everywhere)
	// http://www.irchelp.org/irchelp/rfc/ctcpspec.html
	if(((unsigned char)msg[0]) == 1 && ((unsigned char)msg[msg.length()-1]) == 1) {
		// handle VERSION, PING, TIME, FINGER, SOURCE, USERINFO, CLIENTINFO
		const string stripped_msg = msg.substr(1, msg.length()-2);
		if(stripped_msg == "VERSION") {
			n->send_ctcp_msg(target, "VERSION", UNIBOT_VERSION_STRING);
		}
		else if(stripped_msg == "PING") {
			time_t timestamp;
			time(&timestamp);
			n->send_ctcp_msg(target, "PING", to_str(timestamp));
		}
		else if(stripped_msg == "TIME") {
			stringstream local_time;
			const time_put<char>& tmput = use_facet<time_put<char> >(loc);
			time_t timestamp;
			time(&timestamp);
			const char* pattern = "%Y-%m-%d %H:%M:%S %Z";
			tmput.put(local_time, local_time, ' ', localtime(&timestamp), pattern, pattern+strlen(pattern));
			n->send_ctcp_msg(target, "TIME", ":"+local_time.str());
		}
		else if(stripped_msg == "FINGER") {
			n->send_ctcp_msg(target, "FINGER", ":"+conf->get_bot_realname());
		}
		else if(stripped_msg == "SOURCE") {
			n->send_ctcp_msg(target, "SOURCE", UNIBOT_SOURCE_URL);
		}
		else if(stripped_msg == "USERINFO") {
			n->send_ctcp_msg(target, "USERINFO", UNIBOT_VERSION_STRING);
		}
		else if(stripped_msg == "CLIENTINFO") {
			n->send_ctcp_msg(target, "CLIENTINFO", "CLIENTINFO VERSION PING TIME FINGER SOURCE USERINFO");
		}
	}
}

string bot_handler::handle_args_chronological(const string& msg, const size_t& offset) {
	string ret_msg = "";
	if(msg.length() > offset) {
		string args = msg.substr(offset, msg.length()-offset);
		vector<string> arg_tokens;
		tokenize(arg_tokens, args, ' ');
		int msg_offset = 0, word_offset = 0;
		if(arg_tokens.size() > 0) msg_offset = (unsigned int)strtoul(arg_tokens[0].c_str(), NULL, 10);
		if(arg_tokens.size() > 1) word_offset = (unsigned int)strtoul(arg_tokens[1].c_str(), NULL, 10);
		
		if(msg_offset != 0) {
			ret_msg = extract_word(msg_offset, word_offset);
		}
		if(msg_offset == 0 || msg == "") ret_msg = args;
	}
	else {
		// get longest word of last message
		vector<string> last_msg_tokens;
		tokenize(last_msg_tokens, msg_store.back(), ' ');
		for(vector<string>::iterator str_iter = last_msg_tokens.begin(); str_iter != last_msg_tokens.end(); str_iter++) {
			if(str_iter->length() > msg.length()) ret_msg = *str_iter;
		}
	}
	return ret_msg;
}

string bot_handler::extract_word(ssize_t msg_offset, ssize_t word_offset) {
	msg_offset = abs(msg_offset);
	size_t abs_word_offset = abs(word_offset);
	
	ssize_t msg_number = (ssize_t)msg_store.size() - msg_offset;
	if(msg_number >= 0 && msg_number < (int)msg_store.size()) {
		string msg = msg_store[msg_number];
		
		if(word_offset == 0) return msg;
		
		vector<string> msg_tokens;
		tokenize(msg_tokens, msg, ' ');
		
		if(word_offset > (ssize_t)msg_tokens.size()) return msg_tokens.back();
		if(abs_word_offset > msg_tokens.size()) return msg_tokens[0];
		
		ssize_t word_number = word_offset > 0 ? word_offset-1 : ((int)msg_tokens.size() + word_offset);
		return msg_tokens[word_number];
	}
	
	return "";
}

string bot_handler::strip_special_chars(const string& str) {
	//string special_chars = ",.;:!?=*^<>\"\'";
	string special_chars = ",.;:!?^\"\'";
	string new_str = "";
	for(string::const_iterator citer = str.begin(); citer != str.end(); citer++) {
		if(special_chars.find(*citer) == string::npos) new_str += *citer;
	}
	return new_str;
}

string bot_handler::get_prev_msg(const size_t& offset) {
	if(offset == 0 || offset > msg_store.size()) {
		logger::log(logger::LT_ERROR, "bot_handler.cpp", string("get_prev_msg(): invalid offset "+to_str(offset)+"!").c_str());
		return "";
	}
	return msg_store[msg_store.size() - offset];
}
