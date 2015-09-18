/*
 *  UniBot
 *  Copyright (C) 2009 - 2014 Florian Ziesche
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

#ifndef __UNIBOT_BOT_HANDLER_HPP__
#define __UNIBOT_BOT_HANDLER_HPP__

#include <floor/net/net.hpp>
#include <floor/net/irc_net.hpp>
#include <floor/core/platform.hpp>
#include "event_handler.hpp"
#include "bot_states.hpp"
#include "config.hpp"
#include "build_version.hpp"
#include <floor/threading/thread_base.hpp>

#define UNIBOT_MAJOR_VERSION "0"
#define UNIBOT_MINOR_VERSION "5"
#define UNIBOT_REVISION_VERSION "1"
#define UNIBOT_DEV_STAGE_VERSION "d5"
#define UNIBOT_BUILD_TIME __TIME__
#define UNIBOT_BUILD_DATE __DATE__

class lua;
class bot_handler : public thread_base {
public:
	bot_handler(floor_irc_net* n, bot_states* states, config* conf);
	~bot_handler();

	enum class IRC_COMMAND : unsigned int {
		NONE = 0u,
		ADMIN,
		AWAY,
		CONNECT,
		DIE,
		ERROR,
		INFO,
		NVITE,
		SON,
		JOIN,
		KICK,
		KILL,
		LINKS,
		LIST,
		LUSERS,
		MODE,
		MOTD,
		NAMES,
		NICK,
		NOTICE,
		OPER,
		PART,
		PASS,
		PING,
		PONG,
		PRIVMSG,
		QUIT,
		REHASH,
		RESTART,
		SERVICE,
		SERVLIST,
		SERVER,
		SQUERY,
		SQUIT,
		STATS,
		SUMMON,
		TIME,
		TOPIC,
		TRACE,
		USER,
		USERHOST,
		USERS,
		VERSION,
		WALLOPS,
		WHO,
		WHOIS,
		WHOWAS,
		CMD_001,
		CMD_002,
		CMD_003,
		CMD_004,
		CMD_005,
		CMD_352,
		CMD_353,
		CMD_372,
		CMD_433,
	};
	static const char* IRC_COMMAND_STR[];
	
	void run();
	void quit_bot();
	
	string get_prev_msg(const size_t& offset);
	string handle_args_chronological(const string& msg, const size_t& offset);
	string strip_special_chars(const string& str);
	
	const string& get_server_name() const;
	const unsigned int& get_last_server_pong() const;
	
protected:
	floor_irc_net* n;
	unordered_map<string, IRC_COMMAND> irc_commands;
	bot_states* states;
	config* conf;
	locale loc;
	
	lua* lua_obj;
	
	string server_name { "" };
	unsigned int last_server_pong { SDL_GetTicks() };
	bool sent_server_ping { false };
	
	const unsigned int server_ping_interval;
	const unsigned int server_timeout;
	
	string cur_bot_name { "" };
	
	deque<string> msg_store;
	const size_t keep_msg_count { std::numeric_limits<size_t>::max() };
	
	IRC_COMMAND parse_irc_cmd(string cmd);
	string strip_user(string str);
	string strip_user_realname(string str);
	string strip_user_host(string str);
	void handle_message(string sender, string location, string msg);
	
	string extract_word(ssize_t msg_offset, ssize_t word_offset);
	
};

#endif
