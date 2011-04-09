/*
 *  UniBot
 *  Copyright (C) 2009 - 2011 Florian Ziesche
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
#include "event_handler.h"
#include "net.h"
#include "irc_net.h"
#include "bot_states.h"
#include "config.h"
#include "build_version.h"
#include "threading/thread_base.h"

#define UNIBOT_MAJOR_VERSION "0"
#define UNIBOT_MINOR_VERSION "3"
#define UNIBOT_REVISION_VERSION "0d4"
#define UNIBOT_BUILD_TIME __TIME__
#define UNIBOT_BUILD_DATE __DATE__

#if defined(_MSC_VER)
#define UNIBOT_COMPILER "VC++ "+to_str(_MSC_VER)
#elif (defined(__GNUC__) && !defined(__llvm__) && !defined(__clang__))
#define UNIBOT_COMPILER "GCC "+to_str(__VERSION__)
#elif (defined(__GNUC__) && defined(__llvm__) && !defined(__clang__))
#define UNIBOT_COMPILER "LLVM-GCC "+to_str(__VERSION__)
#elif defined(__clang__)
#define UNIBOT_COMPILER "Clang "+to_str(__clang_version__)
#else
#define UNIBOT_COMPILER "unknown compiler"
#endif

#define UNIBOT_LIBCXX_PREFIX " and "
#if defined(_LIBCPP_VERSION)
#define UNIBOT_LIBCXX UNIBOT_LIBCXX_PREFIX+"libc++ "+to_str(_LIBCPP_VERSION)
#elif defined(__GLIBCXX__)
#define UNIBOT_LIBCXX UNIBOT_LIBCXX_PREFIX+"libstdc++ "+to_str(__GLIBCXX__)
#else
#define UNIBOT_LIBCXX ""
#endif

#define UNIBOT_VERSION_STRING ("UniBot "+to_str(sizeof(void*) == 4 ? "x86" : (sizeof(void*) == 8 ? "x64" : "unknown"))+" v"+to_str(UNIBOT_MAJOR_VERSION)+"."+ \
								to_str(UNIBOT_MINOR_VERSION)+"."+to_str(UNIBOT_REVISION_VERSION)+"-"+to_str(UNIBOT_BUILD_VERSION)+" ("+UNIBOT_BUILD_DATE+" "+ \
								UNIBOT_BUILD_TIME+") built with "+UNIBOT_COMPILER+UNIBOT_LIBCXX)

#define UNIBOT_SOURCE_URL "http://www.assembla.com/spaces/unibot"

class lua;
class bot_handler : public thread_base {
public:
	bot_handler(unibot_irc_net* n, bot_states* states, config* conf);
	~bot_handler();

	enum IRC_COMMAND {
		CMD_NONE,
		CMD_ADMIN,
		CMD_AWAY,
		CMD_CONNECT,
		CMD_DIE,
		CMD_ERROR, // damn you, msvc!
		CMD_INFO,
		CMD_NVITE,
		CMD_SON,
		CMD_JOIN,
		CMD_KICK,
		CMD_KILL,
		CMD_LINKS,
		CMD_LIST,
		CMD_LUSERS,
		CMD_MODE,
		CMD_MOTD,
		CMD_NAMES,
		CMD_NICK,
		CMD_NOTICE,
		CMD_OPER,
		CMD_PART,
		CMD_PASS,
		CMD_PING,
		CMD_PONG,
		CMD_PRIVMSG,
		CMD_QUIT,
		CMD_REHASH,
		CMD_RESTART,
		CMD_SERVICE,
		CMD_SERVLIST,
		CMD_SERVER,
		CMD_SQUERY,
		CMD_SQUIT,
		CMD_STATS,
		CMD_SUMMON,
		CMD_TIME,
		CMD_TOPIC,
		CMD_TRACE,
		CMD_USER,
		CMD_USERHOST,
		CMD_USERS,
		CMD_VERSION,
		CMD_WALLOPS,
		CMD_WHO,
		CMD_WHOIS,
		CMD_WHOWAS,
		CMD_001,
		CMD_002,
		CMD_003,
		CMD_004,
		CMD_005,
		CMD_352,
		CMD_353,
		CMD_372
	};
	static const char* IRC_COMMAND_STR[];
	
	void run();
	void quit_bot();
	
	string get_prev_msg(const size_t& offset);
	string handle_args_chronological(const string& msg, const size_t& offset);
	string strip_special_chars(const string& str);
	
protected:
	unibot_irc_net* n;
	map<string, IRC_COMMAND> irc_commands;
	bot_states* states;
	config* conf;
	locale loc;
	
	lua* lua_obj;
	
	string servername;
	unsigned long int start_time;
	
	deque<string> msg_store;
	size_t keep_msg_count;
	
	IRC_COMMAND parse_irc_cmd(string cmd);
	string strip_user(string str);
	string strip_user_realname(string str);
	string strip_user_host(string str);
	void handle_message(string sender, string location, string msg);
	
	string extract_word(ssize_t msg_offset, ssize_t word_offset);
	
};

#endif
