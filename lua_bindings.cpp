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

#include "lua_bindings.h"

//#if (!defined(__GNUC__) || defined(__clang__)) && (__has_feature(cxx_variadic_templates) || defined(__APPLE__)) // use this when compiling in c++0x mode
#if __has_feature(cxx_variadic_templates) || !defined(__clang__) || defined(__APPLE__)
#include <tr1/tuple>
using namespace tr1;
#endif
// NOTE: if you want to use clang w/o variadic template support, you have to use libstdc++ 4.2.x


/////////////////////////////////////////////////////
// helper functions

template<typename T> bool check_lua_type(lua_State* state, const size_t& arg_num) {
	throw lua_bindings_exception(string("unknown argument type, argument #"+to_str(arg_num)+", user data types are not supported yet!"));
	return false;
}

template<> bool check_lua_type<string>(lua_State* state, const size_t& arg_num) {
	return (lua_isstring(state, (int)arg_num) == 1);
}

template<> bool check_lua_type<int>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
#ifndef PLATFORM_X86
template<> bool check_lua_type<unsigned int>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
#endif
template<> bool check_lua_type<size_t>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
template<> bool check_lua_type<ssize_t>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
template<> bool check_lua_type<float>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
template<> bool check_lua_type<double>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}

template<> bool check_lua_type<bool>(lua_State* state, const size_t& arg_num) {
	return (lua_isboolean(state, (int)arg_num) == 1);
}

template<typename T> T get_lua_arg(lua_State* state, const size_t& arg_num) {
	throw lua_bindings_exception(string("unknown argument type, argument #"+to_str(arg_num)+", user data types are not supported yet!"));
	return (T)0;
}

template<> string get_lua_arg<string>(lua_State* state, const size_t& arg_num) {
	return string(lua_tostring(state, (int)arg_num));
}

template<> int get_lua_arg<int>(lua_State* state, const size_t& arg_num) {
	return int(lua_tointeger(state, (int)arg_num));
}
#ifndef PLATFORM_X86
template<> unsigned int get_lua_arg<unsigned int>(lua_State* state, const size_t& arg_num) {
	return (unsigned int)(lua_tointeger(state, (int)arg_num));
}
#endif
template<> size_t get_lua_arg<size_t>(lua_State* state, const size_t& arg_num) {
	return size_t(lua_tointeger(state, (int)arg_num));
}
template<> ssize_t get_lua_arg<ssize_t>(lua_State* state, const size_t& arg_num) {
	return ssize_t(lua_tointeger(state, (int)arg_num));
}
template<> float get_lua_arg<float>(lua_State* state, const size_t& arg_num) {
	return float(lua_tonumber(state, (int)arg_num));
}
template<> double get_lua_arg<double>(lua_State* state, const size_t& arg_num) {
	return double(lua_tonumber(state, (int)arg_num));
}

template<> bool get_lua_arg<bool>(lua_State* state, const size_t& arg_num) {
	return bool(lua_toboolean(state, (int)arg_num));
}

void check_lua_stack(lua_State* state, const size_t& expected_size) {
	if(lua_gettop(state) != (int)expected_size) {
		throw lua_bindings_exception(string("invalid lua stack size, expected "+to_str(expected_size)+"!"));
	}
}

template<size_t arg_num, typename T> T get_and_check_lua_arg(lua_State* state) {
	// get and check number of arguments
	int argc = lua_gettop(state);
	if((int)arg_num > argc) {
		throw lua_bindings_exception(string("invalid argument count "+to_str(argc)+", should be >= "+to_str(arg_num)+"!"));
	}
	
	if(!check_lua_type<T>(state, arg_num)) {
		throw lua_bindings_exception(string("invalid argument type, argument #"+to_str(arg_num)+", should be "+typeid(T).name()+"!"));
	}
	
	return get_lua_arg<T>(state, arg_num);
}

// now, where are those variadic templates ... -.-?
// note: lua argument count starts at 1, not 0!
template<typename arg1> tuple<arg1> get_lua_args(lua_State* state) {
	return make_tuple(get_and_check_lua_arg<1, arg1>(state));
}
template<typename arg1, typename arg2> tuple<arg1, arg2> get_lua_args(lua_State* state) {
	return make_tuple(get_and_check_lua_arg<1, arg1>(state), get_and_check_lua_arg<2, arg2>(state));
}
template<typename arg1, typename arg2, typename arg3> tuple<arg1, arg2, arg3> get_lua_args(lua_State* state) {
	return make_tuple(get_and_check_lua_arg<1, arg1>(state), get_and_check_lua_arg<2, arg2>(state), get_and_check_lua_arg<3, arg3>(state));
}
template<typename arg1, typename arg2, typename arg3, typename arg4> tuple<arg1, arg2, arg3, arg4> get_lua_args(lua_State* state) {
	return make_tuple(get_and_check_lua_arg<1, arg1>(state), get_and_check_lua_arg<2, arg2>(state), get_and_check_lua_arg<3, arg3>(state), get_and_check_lua_arg<4, arg4>(state));
}

#define HANDLE_LUA_BINDINGS_EXCEPTION \
catch(lua_bindings_exception& e) { \
	logger::log(logger::LT_ERROR, "lua_bindings.cpp", string("in function "+string(__func__)+": "+e.what()).c_str()); \
	return -1; \
}

/////////////////////////////////////////////////////
// lua bindings

unibot_irc_net* lua_bindings::n = NULL;
bot_handler* lua_bindings::handler = NULL;
bot_states* lua_bindings::states = NULL;
config* lua_bindings::conf = NULL;
lua* lua_bindings::l = NULL;
lua_bindings::fp_reload_scripts lua_bindings::lua_reload_scripts = NULL;

lua_bindings::lua_bindings(unibot_irc_net* n, bot_handler* handler, bot_states* states, config* conf, lua* l, fp_reload_scripts lua_reload_scripts) {
	lua_bindings::n = n;
	lua_bindings::handler = handler;
	lua_bindings::states = states;
	lua_bindings::conf = conf;
	
	lua_bindings::l = l;
	lua_bindings::lua_reload_scripts = lua_reload_scripts;
}

lua_bindings::~lua_bindings() {
}

int lua_bindings::send(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		n->send(get<0>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::send_channel_msg(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		n->send_channel_msg(get<0>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::send_private_msg(lua_State* state) {
	try {
		tuple<string, string> args = get_lua_args<string, string>(state);
		n->send_private_msg(get<0>(args), get<1>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::send_action_msg(lua_State* state) {
	try {
		tuple<string, string> args = get_lua_args<string, string>(state);
		n->send_action_msg(get<0>(args), get<1>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::send_ctcp_request(lua_State* state) {
	try {
		tuple<string, string> args = get_lua_args<string, string>(state);
		n->send_ctcp_request(get<0>(args), get<1>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::send_kick(lua_State* state) {
	try {
		tuple<string, string> args = get_lua_args<string, string>(state);
		n->send_kick(get<0>(args), get<1>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::get_local_ip(lua_State* state) {
	try {
		check_lua_stack(state, 0);
		IPaddress* lip = n->get_local_ip();
		lua_pushinteger(state, lip->host);
		lua_pushinteger(state, lip->port);
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 2;
}

int lua_bindings::get_server_ip(lua_State* state) {
	try {
		check_lua_stack(state, 0);
		IPaddress* sip = n->get_server_ip();
		lua_pushinteger(state, sip->host);
		lua_pushinteger(state, sip->port);
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 2;
}

int lua_bindings::join_channel(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		n->join_channel(get<0>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::get_bot_state(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		lua_pushboolean(state, states->is(get<0>(args)));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::set_bot_state(lua_State* state) {
	try {
		tuple<string, bool> args = get_lua_args<string, bool>(state);
		states->set(get<0>(args), get<1>(args));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 0;
}

int lua_bindings::get_users(lua_State* state) {
	try {
		check_lua_stack(state, 0);
		map<string, bot_states::user_info*>* users = states->get_users();
		
		// create user table and add all users to it
		// table format: { nickname = { realname, host }, ... }
		lua_createtable(state, (int)users->size(), 0);
		for(map<string, bot_states::user_info*>::iterator user_iter = users->begin(); user_iter != users->end(); user_iter++) {
			// key
			lua_pushstring(state, user_iter->first.c_str());
			
			// value (-> new table, containing two strings)
			lua_createtable(state, 2, 0);
			lua_pushstring(state, user_iter->second->nick.c_str());
			lua_rawseti(state, -2, 1);
			lua_pushstring(state, user_iter->second->real_name.c_str());
			lua_rawseti(state, -2, 2);
			lua_pushstring(state, user_iter->second->host.c_str());
			lua_rawseti(state, -2, 3);
			lua_pushstring(state, user_iter->second->host_user.c_str());
			lua_rawseti(state, -2, 4);
			lua_pushstring(state, user_iter->second->registered.c_str());
			lua_rawseti(state, -2, 5);
			lua_pushstring(state, user_iter->second->ctcp_support.c_str());
			lua_rawseti(state, -2, 6);
			lua_pushstring(state, user_iter->second->client.c_str());
			lua_rawseti(state, -2, 7);
			
			// add key + value table to users table
			lua_settable(state, -3);
		}
		
		// push size of users table
		lua_pushinteger(state, (int)users->size());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 2;
}

int lua_bindings::is_registered(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		
		// check if user exists and is registered/identified
		lua_pushboolean(state, states->is_user_registered(get<0>(args)));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::get_config_entry(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		lua_pushstring(state, conf->get_config_entry(get<0>(args)).c_str());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::is_owner(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		
		// check if user exists and is registered/identified and check if the nick has an config owner entry
		lua_pushboolean(state, conf->is_owner(get<0>(args)) && states->is_user_registered(get<0>(args)));
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::url_encode(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		lua_pushstring(state, encode_url(get<0>(args)).c_str());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::get_prev_msg(lua_State* state) {
	try {
		tuple<size_t> args = get_lua_args<size_t>(state);
		lua_pushstring(state, handler->get_prev_msg(get<0>(args)).c_str());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::strip_special_chars(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		lua_pushstring(state, handler->strip_special_chars(get<0>(args)).c_str());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::handle_args_chronological(lua_State* state) {
	try {
		tuple<string> args = get_lua_args<string>(state);
		lua_pushstring(state, handler->handle_args_chronological(get<0>(args), 0).c_str());
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}

int lua_bindings::reload_scripts(lua_State* state) {
	try {
		check_lua_stack(state, 0);
		
		// generate an error in the current state to abort script execution
		lua_error(state);
	}
	catch(...) {
		(l->*lua_reload_scripts)();
		
		// since we are currently in a script iteration loop and the scripts store was renewed/reloaded,
		// all iterators to the old script store are now invalidated (-> break loop)
		throw invalidate_scripts_exception();
	}
	return 0;
}

int lua_bindings::crand(lua_State* state) {
	try {
		tuple<ssize_t> args = get_lua_args<ssize_t>(state);
		lua_pushinteger(state, get<0>(args) != 0 ? 1 + (rand() % std::labs(get<0>(args))) : 0);
	}
	HANDLE_LUA_BINDINGS_EXCEPTION;
	return 1;
}
