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

#ifndef __BOT_HANDLER_H__
#define __BOT_HANDLER_H__

#include "platform.h"
#include "net.h"
#include "bot_states.h"
#include "config.h"

class bot_handler {
public:
	bot_handler(net<TCP_protocol>* n, bot_states* states, config* conf);
	~bot_handler();

	enum IRC_COMMAND {
		NONE,
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
		CMD_353,
		CMD_372
	};
	static const char* IRC_COMMAND_STR[];
	
	void handle();
	void quit_bot();
	
protected:
	net<TCP_protocol>* n;
	map<string, IRC_COMMAND> irc_commands;
	bot_states* states;
	config* conf;
	locale loc;
	
	string servername;
	unsigned long int start_time;
	
	deque<string> msg_store;
	unsigned int keep_msg_count;
	
	IRC_COMMAND parse_irc_cmd(string cmd);
	string strip_user(string str);
	string strip_user_realname(string str);
	string strip_user_host(string str);
	void handle_message(string sender, string location, string msg);
	
	string handle_args_chronological(string msg, unsigned int offset);
	string extract_word(int msg_offset, int word_offset);
	string strip_special_chars(string str);
	
};

#endif
