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

#ifndef __LUA_BINDINGS_H__
#define __LUA_BINDINGS_H__

#include "platform.h"
#include "net.h"
#include "irc_net.h"
#include "bot_states.h"
#include "bot_handler.h"
#include "config.h"
#include "util.h"

#define __LUA_FUNCTION_BINDINGS(F) \
/* net.h bindings */ \
F(send) \
F(send_channel_msg) \
F(send_private_msg) \
F(send_action_msg) \
F(send_ctcp_request) \
F(send_kick) \
F(get_local_ip) \
F(get_server_ip) \
F(join_channel) \
/* bot_states.h bindings */ \
F(get_bot_state) \
F(set_bot_state) \
F(get_users) \
F(is_registered) \
/* config.h bindings */ \
F(get_config_entry) \
F(is_owner) \
/* misc/additional functions */ \
F(url_encode) \
F(get_prev_msg) \
F(strip_special_chars) \
F(handle_args_chronological) \
F(reload_scripts) \
F(crand) \
F(get_os) \
F(execute_command)

#define LUA_FUNCTION_DEFINITION(func_name) static int func_name(lua_State* state);

class lua_bindings_exception : public exception {
protected:
	string error_str;
public:
	lua_bindings_exception(const string& error_str) : error_str(error_str) {}
	~lua_bindings_exception() throw() {}
    virtual const char* what() const throw () {
		return error_str.c_str();
	}
};
class invalidate_scripts_exception : exception {};

class lua;
class lua_bindings {
public:
	typedef void (lua::*fp_reload_scripts)();
	
	lua_bindings(unibot_irc_net* n, bot_handler* handler, bot_states* states, config* conf, lua* l, fp_reload_scripts lua_reload_scripts);
	~lua_bindings();
	
	__LUA_FUNCTION_BINDINGS(LUA_FUNCTION_DEFINITION);
	
protected:
	static unibot_irc_net* n;
	static bot_handler* handler;
	static bot_states* states;
	static config* conf;
	
	static lua* l;
	static fp_reload_scripts lua_reload_scripts;
	
};

#endif
