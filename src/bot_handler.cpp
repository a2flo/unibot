/*
 *  UniBot
 *  Copyright (C) 2009 - 2013 Florian Ziesche
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

#include "bot_handler.hpp"
#include "lua_unibot.hpp"
#include "core/core.hpp"
#include "threading/task.hpp"
#include "net/http_net.hpp"
#include <regex>

#include "floor/floor_version.hpp"

#define UNIBOT_VERSION_STRING (string("UniBot ")+FLOOR_PLATFORM+FLOOR_DEBUG_STR \
" v"+(UNIBOT_MAJOR_VERSION)+"."+(UNIBOT_MINOR_VERSION)+"."+(UNIBOT_REVISION_VERSION)+(UNIBOT_DEV_STAGE_VERSION)+"-"+size_t2string(UNIBOT_BUILD_VERSION)+\
" ("+UNIBOT_BUILD_DATE+" "+UNIBOT_BUILD_TIME+") built with "+string(FLOOR_COMPILER+FLOOR_LIBCXX))+" and floor v"+FLOOR_FULL_VERSION

#define UNIBOT_SOURCE_URL "https://github.com/a2flo/unibot"

// stores the time of when the bot was first started
// note that this has to be stored in a 64-bit uint, because it would overflow after
// 49 days when simply using SDL_GetTicks + a 32-bit uint
// -> use SDL_GetPerformanceCounter instead
// however, this way we don't know the time scale of this until runtime
// "best" case: 1000000000 ticks/s, "worst" case: 1000 tick/s
// -> overflow after: 213503 days, 213503982334 days
static unsigned long long int bot_start_time { 0 };

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
	"372",
	"433",
};

bot_handler::bot_handler(floor_irc_net* n_, bot_states* states_, config* conf_) :
thread_base(), n(n_), states(states_), conf(conf_),
server_ping_interval((unsigned int)strtoul(conf->get_config_entry("server_ping").c_str(), 0, 10)),
server_timeout((unsigned int)strtoul(conf->get_config_entry("server_timeout").c_str(), 0, 10))
{
	if(bot_start_time == 0) {
		bot_start_time = SDL_GetPerformanceCounter();
	}
	
	cur_bot_name = conf->get_bot_name();
	unsigned int cmd_size = sizeof(IRC_COMMAND_STR) / sizeof(const char*);
	for(unsigned int i = 0; i < cmd_size; i++) {
		irc_commands.emplace(IRC_COMMAND_STR[i], (bot_handler::IRC_COMMAND)(i));
	}
	
	lua_obj = new lua(n, this, states, conf);
	
	this->set_thread_delay(100); // 100ms are enough
	this->start(); // start thread
}

bot_handler::~bot_handler() {
}

void bot_handler::run() {
	const auto connection_timeout = server_ping_interval + server_timeout;
	if((SDL_GetTicks() - last_server_pong) > connection_timeout) {
		n->invalidate();
		states->set_parted(true);
		states->set_restart(true);
		set_thread_should_finish();
		log_error("server timeout - restarting ...");
		return;
	}

	// nothing to do here until a connection has been established
	if(!n->is_running()) return;
	
	//
	if(!sent_server_ping &&
	   (SDL_GetTicks() - last_server_pong) > server_ping_interval) {
		// pretend to have sent a ping, even if we're not connected yet
		// this will take care of weird server connection problems
		if(server_name != "") n->ping(server_name);
		sent_server_ping = true;
	}
	
	// if there are no messages to handle, return
	if(!n->is_received_data()) return;
	
	auto received_data = n->get_and_clear_received_data();
	for(const auto& data_vec : received_data) {
		const string data(data_vec.data(), data_vec.size());
		
		// handle the data
		// cmd_tokens:
		// [0]: sender
		// [1]: command
		// [2]: location
		// [3+]: data
		IRC_COMMAND current_cmd = parse_irc_cmd(data);
		vector<string> cmd_tokens { core::tokenize(data, ' ') };
		
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
			cmd_joined_data = data.substr(pos, data.length()-pos);
		}
		
		// any received server msg == "pong"
		last_server_pong = SDL_GetTicks();
		
		switch(current_cmd) {
			case IRC_COMMAND::NONE:
				// ignore
				break;
			case IRC_COMMAND::CMD_001:
				states->set_connected(true);
				log_debug("successfully connected to the server!");
				n->join_channel(conf->get_channel());
				
				if(cur_bot_name != conf->get_bot_name()) {
					// connect with a different name -> ghost original bot name
					n->send("PRIVMSG NickServ :GHOST " + conf->get_bot_name() + " " + conf->get_bot_password());
				}
				else {
					// everything is normal -> send ident
					n->send_identify(conf->get_bot_password());
				}
				break;
			case IRC_COMMAND::CMD_004:
				server_name = cmd_data;
				log_debug("server name is: %s", server_name);
				break;
			case IRC_COMMAND::CMD_352:
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
					log_error("CMD_352 failed");
				}
				break;
			case IRC_COMMAND::CMD_353: {
				// only handle the main channel
				if(cmd_tokens[4] != conf->get_channel()) break;
				
				if(cmd_joined_data.find(':') == string::npos) break;
				// strip ':' and last ' ' (if there is one)
				string user_str = core::trim(cmd_joined_data.substr(cmd_joined_data.find(':') + 1, cmd_joined_data.length() - cmd_joined_data.find(':') - 1));
				vector<string> users { core::tokenize(user_str, ' ') };
				states->delete_all_users();
				for(auto user_iter = users.begin(); user_iter != users.end(); user_iter++) {
					if(user_iter->length() > 0) {
						if((*user_iter)[0] == '+' || (*user_iter)[0] == '@') {
							*user_iter = user_iter->substr(1, user_iter->length() - 1);
						}
						states->add_user(*user_iter);
					}
				}
			}
				break;
			case IRC_COMMAND::PING:
				n->send("PONG " + server_name);
				log_debug("PONG!");
				break;
			case IRC_COMMAND::PONG:
				last_server_pong = SDL_GetTicks();
				sent_server_ping = false;
				break;
			case IRC_COMMAND::NOTICE:
				if(cmd_joined_data.find("You are now identified for", 0) != string::npos && cmd_joined_data.find(conf->get_bot_name(), 0) != string::npos) {
					states->set_identified(true);
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
					else if(cmd_joined_data.find("has been ghosted") != string::npos ||
							(cur_bot_name != conf->get_bot_name() &&
							 cmd_joined_data.find("is not online") != string::npos)) {
						// old session has been ghost -> reset nick to the original name and identify
						n->send_nick(conf->get_bot_name());
						n->send_identify(conf->get_bot_password());
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
			case IRC_COMMAND::JOIN:
				// only handle the main channel
				if(conf->get_channel() == cmd_tokens[2]) {
					// if the bot joined the channel, set the flag and send a "hi there ;)" message
					if(strip_user(cmd_sender) == conf->get_bot_name()) {
						log_debug("joined the channel");
						states->set_joined(true);
						if(!states->is_silenced()) {
							n->send_channel_msg(conf->get_channel(), "hi there ;)");
						}
						
						// check if bot got kicked
						if(states->is_kicked()) {
							// random kick message
							static const array<const char*, 5> kick_messages {{
								"try me!", "booyah!", "bot pwnage!", "don't mess with me!", "nice try!"
							}};
							
							// wait for 2 seconds before kicking the user
							SDL_Delay(2000);
							
							log_debug("kicking: %s", states->get_kick_user());
							n->send_kick(conf->get_channel(), states->get_kick_user(),
										 kick_messages[core::rand((unsigned int)kick_messages.size())]);
						}
					}
					// if another user joined the channel, greet him
					else {
						if(!(states->is_silenced())) {
							n->send_channel_msg(conf->get_channel(), "hey, " + strip_user(cmd_sender));
						}
						states->add_user(strip_user(cmd_sender));
					}
				}
				break;
			case IRC_COMMAND::PART:
				// only handle the main channel
				if(conf->get_channel() == cmd_tokens[2]) {
					states->delete_user(strip_user(cmd_sender));
					if(strip_user(cmd_sender) == conf->get_bot_name()) {
						states->set_parted(true);
					}
				}
				break;
			case IRC_COMMAND::QUIT:
				states->delete_user(strip_user(cmd_sender));
				break;
			case IRC_COMMAND::PRIVMSG: {
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
					log_msg("%s: %s", strip_user(cmd_sender), msg);
				}
			}
				break;
			case IRC_COMMAND::MODE:
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
			case IRC_COMMAND::KICK:
				// check if bot was kicked
				if(cmd_data == conf->get_bot_name()) {
					// check if the user who kicked the bot is the owner
					if(conf->is_owner(strip_user(cmd_sender))) {
						// if not, rejoin the channel and kick the user who kicked the bot
						n->join_channel(conf->get_channel());
						
						states->set_kick_user(strip_user(cmd_sender));
						log_debug("to get kicked: %s", states->get_kick_user());
						states->set_kicked(true);
					}
				}
				else {
					states->delete_user(cmd_data);
				}
				break;
			case IRC_COMMAND::NICK:
				states->update_user_name(strip_user(cmd_sender), cmd_location.substr(1, cmd_location.length()-1));
				break;
			case IRC_COMMAND::TOPIC:
				break;
			case IRC_COMMAND::ERROR:
				log_error("received server error - restarting ...");
				states->set_restart(true);
				quit_bot();
				set_thread_should_finish();
				break;
			case IRC_COMMAND::CMD_433:
				// "Nickname is already in use" -> set alt nick, ghost orig nick later on
				cur_bot_name += conf->get_bot_alt_add();
				n->send_nick(cur_bot_name);
				break;
			default:
				break;
		}
		
		// TODO: add lua events
		
		if(states->is_parted() && states->is_quit()) {
			n->quit();
			
			// and finally finish both threads
			set_thread_should_finish();
		}
		
		log_debug("%s", data);
	}
	n->clear_received_data();
}

bot_handler::IRC_COMMAND bot_handler::parse_irc_cmd(string cmd) {
	vector<string> cmd_tokens { core::tokenize(cmd, ' ') };
	IRC_COMMAND irc_cmd = bot_handler::IRC_COMMAND::NONE;
	
	// check for normal irc command, some commands are in the first token ...
	if(cmd_tokens.size() > 0 && irc_commands.count(cmd_tokens[0]) > 0) {
		irc_cmd = irc_commands[cmd_tokens[0]];
	}
	else if(cmd_tokens.size() > 1 && irc_commands.count(cmd_tokens[1]) > 0) {
		irc_cmd = irc_commands[cmd_tokens[1]];
	}
	else if(cmd_tokens.size() > 1 && cmd_tokens[1].length() == 3) {
		// check for numbered irc command
		unsigned int num = (unsigned int)strtoul(cmd_tokens[1].c_str(), nullptr, 10);
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
	n->part(conf->get_channel());
}

void bot_handler::handle_message(string sender, string location, string msg) {
	string origin = strip_user(sender);
	string target = (location == conf->get_bot_name() ? origin : location);
	// all bot commands have to start with '!'
	// only accept messages originating from the channel or from a user inside the channel
	if(location == conf->get_channel() || states->is_user(origin)) {
		if(msg[0] == '!') {
			msg = msg.substr(1, msg.length()-1);
		}
		
		lua_obj->handle_message(origin, target, msg);
		
		// official commands
		size_t cmd_end = msg.find(' ', 0);
		string cmd = (cmd_end != string::npos ? msg.substr(0, cmd_end) : msg);
		if(cmd == "uptime") {
			unsigned long long int uptime = SDL_GetPerformanceCounter() - bot_start_time;
			uptime /= SDL_GetPerformanceFrequency(); // to seconds
			string uptime_str = "";
			
			constexpr unsigned long long int t_day = 60ULL * 60ULL * 24ULL;
			constexpr unsigned long long int t_hour = 60ULL * 60ULL;
			constexpr unsigned long long int t_minute = 60ULL;
			
			uptime_str += ull2string(uptime / t_day) + "d ";
			uptime %= t_day;
			uptime_str += ull2string(uptime / t_hour) + "h ";
			uptime %= t_hour;
			uptime_str += ull2string(uptime / t_minute) + "m ";
			uptime %= t_minute;
			uptime_str += ull2string(uptime) + "s";
			
			n->send_private_msg(target, uptime_str);
		}
		else if((msg.length() >= 4 && msg.substr(0, 4) == "quit") ||
				(msg.length() >= 7 && msg.substr(0, 7) == "restart")) {
			bool restart = (msg.length() >= 7 && msg.substr(0, 7) == "restart");
			
			string in_bot_name = "";
			if(!restart && msg.length() > 5) in_bot_name = msg.substr(5, msg.length()-5);
			else if(restart && msg.length() > 8) in_bot_name = msg.substr(8, msg.length()-8);
			
			if(in_bot_name == "" || in_bot_name == conf->get_bot_name()) {
				if(conf->is_owner(origin)) {
					quit_bot();
					states->set_quit(true);
					states->set_restart(restart);
				}
				else {
					if(states->is_op()) {
						n->send_kick(conf->get_channel(), origin, "you're no bot owner!");
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
			log_debug("script %s reloaded!", script);
		}
		else if(cmd == "list_scripts") {
			if(conf->is_owner(origin)) {
				string script_list = "";
				vector<string> scripts = lua_obj->list_scripts();
				sort(scripts.begin(), scripts.end());
				for(auto siter = scripts.begin(); siter != scripts.end(); siter++) {
					const size_t slash_pos = siter->rfind('/')+1;
					script_list += siter->substr(slash_pos, siter->length()-slash_pos) + " ";
				}
				n->send_private_msg(target, script_list);
			}
		}
		// http request testing (only temporary for now - TODO: full implementation)
		// TODO: without the cmd http; parse the whole message, split for spaces, do a request for each part with http/https/www at the beginning
		else if(cmd == "http") {
			const string url(core::trim(msg.substr(cmd_end + 1, msg.size() - cmd_end - 1)));
			task::spawn([url, target, this]() {
				try {
					atomic<bool> received { false };
					http_net request(url, [url, target, this, &received](http_net* http_obj floor_unused,
																		 http_net::HTTP_STATUS ret_status,
																		 const string& server floor_unused,
																		 const string& data) -> bool {
						if(ret_status == http_net::HTTP_STATUS::CODE_200 && data != "timeout") {
							const regex title_rx("<[\\s]*[Tt][Ii][Tt][Ll][Ee][\\s]*>"
												 "([\\s\\S]*)" // also matches newlines (. doesn't)
												 "<[\\s]*[/][\\s]*[Tt][Ii][Tt][Ll][Ee][\\s]*>");
							smatch match;
							if(regex_search(data, match, title_rx) && match.size() >= 2) {
								// title: first match + replace newlines + trim space from the beginning and end
								string title { match.str(1) };
								core::find_and_replace(title, "\n", " ");
								core::find_and_replace(title, "\r", " ");
								title = core::trim(title);
								// TODO: html to text (i.e. &#39; to ')
								if(!received) n->send_private_msg(target, "> "+title);
							}
							else {
								if(!received) n->send_private_msg(target, "> no title found!");
							}
						}
						else {
							// unsucessful request
							const string err_msg("http request \"" + url + "\" failed: " +
												 uint2string((unsigned int)ret_status) +
												 " " + http_net::status_code_to_string(ret_status) +
												 (data != "" ? " (" : "") + data + (data != "" ? ")" : ""));
							if(!received) n->send_private_msg(target, err_msg);
							log_error("%s", err_msg);
						}
						// signal that the request has been handled
						received = true;
						return true;
					});
					
					// wait until request was successful (note: http_net will timeout after 10s per default)
					while(!received) {
						this_thread::yield();
					}
				}
				catch(floor_exception& ex) {
					log_error("http request exception: %s", ex.what());
				}
				catch(...) {
					log_error("http request exception");
				}
			});
		}
		else if(cmd == "ssl_info") {
			if(!n->is_ssl()) {
				n->send_private_msg(target, "tls/ssl is not in use!");
			}
			else {
				n->send_private_msg(target, ("using tls/ssl with cipher \""s +
											 n->get_ssl_protocol().get_protocol().get_protocol_details().get_current_cipher() +
											 "\"!"));
			}
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
			n->send_ctcp_msg(target, "PING", size_t2string(timestamp));
		}
		else if(stripped_msg == "TIME") {
			stringstream local_time;
			const time_put<char>& tmput = use_facet<time_put<char>>(loc);
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
		vector<string> arg_tokens { core::tokenize(args, ' ') };
		int msg_offset = 0, word_offset = 0;
		if(arg_tokens.size() > 0) msg_offset = (int)strtoul(arg_tokens[0].c_str(), nullptr, 10);
		if(arg_tokens.size() > 1) word_offset = (int)strtoul(arg_tokens[1].c_str(), nullptr, 10);
		
		if(msg_offset != 0) {
			ret_msg = extract_word(msg_offset, word_offset);
		}
		if(msg_offset == 0 || msg == "") ret_msg = args;
	}
	else {
		// get longest word of last message
		vector<string> last_msg_tokens { core::tokenize(msg_store.back(), ' ') };
		for(auto str_iter = last_msg_tokens.begin(); str_iter != last_msg_tokens.end(); str_iter++) {
			if(str_iter->length() > msg.length()) ret_msg = *str_iter;
		}
	}
	return ret_msg;
}

string bot_handler::extract_word(ssize_t msg_offset_, ssize_t word_offset) {
	size_t msg_offset = (size_t)abs(msg_offset_);
	size_t abs_word_offset = (size_t)abs(word_offset);
	
	ssize_t msg_number = (ssize_t)msg_store.size() - (ssize_t)msg_offset;
	if(msg_number >= 0 && msg_number < (ssize_t)msg_store.size()) {
		string msg = msg_store[(size_t)msg_number];
		
		if(word_offset == 0) return msg;
		
		vector<string> msg_tokens { core::tokenize(msg, ' ') };
		
		if(word_offset > (ssize_t)msg_tokens.size()) return msg_tokens.back();
		if(abs_word_offset > msg_tokens.size()) return msg_tokens[0];
		
		size_t word_number = (size_t)(word_offset > 0 ? word_offset-1 : ((ssize_t)msg_tokens.size() + word_offset));
		return msg_tokens[word_number];
	}
	
	return "";
}

string bot_handler::strip_special_chars(const string& str) {
	//string special_chars = ",.;:!?=*^<>\"\'";
	string special_chars = ",.;:!?^\"\'";
	string new_str = "";
	for(auto citer = str.begin(); citer != str.end(); citer++) {
		if(special_chars.find(*citer) == string::npos) new_str += *citer;
	}
	return new_str;
}

string bot_handler::get_prev_msg(const size_t& offset) {
	if(offset == 0 || offset > msg_store.size()) {
		log_error("invalid offset %i!", offset);
		return "";
	}
	return msg_store[msg_store.size() - offset];
}

const string& bot_handler::get_server_name() const {
	return server_name;
}
