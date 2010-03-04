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

#ifndef __LUA_BINDINGS_H__
#define __LUA_BINDINGS_H__

#include "platform.h"
#include "net.h"
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
F(send_kick) \
F(get_local_ip) \
F(get_server_ip) \
/* bot_states.h bindings */ \
F(get_bot_state) \
F(set_bot_state) \
F(get_users) \
/* config.h bindings */ \
F(get_config_entry) \
F(is_owner) \
/* misc/additional functions */ \
F(url_encode) \
F(get_prev_msg) \
F(strip_special_chars) \
F(handle_args_chronological)

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

class lua_bindings {
public:
	lua_bindings(net<TCP_protocol>* n, bot_handler* handler, bot_states* states, config* conf);
	~lua_bindings();
	
	__LUA_FUNCTION_BINDINGS(LUA_FUNCTION_DEFINITION);
	
protected:
	static net<TCP_protocol>* n;
	static bot_handler* handler;
	static bot_states* states;
	static config* conf;
	
};

#endif
