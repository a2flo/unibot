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

#if __has_feature(cxx_variadic_templates) || !defined(__clang__) || defined(__APPLE__)
#include <tr1/tuple>
#else
// really nasty clang++ tuple/variadic template workaround
#include "tuple/tr1/tuple"
#endif

using namespace tr1;

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
template<> bool check_lua_type<uint>(lua_State* state, const size_t& arg_num) {
	return (lua_isnumber(state, (int)arg_num) == 1);
}
template<> bool check_lua_type<size_t>(lua_State* state, const size_t& arg_num) {
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
template<> uint get_lua_arg<uint>(lua_State* state, const size_t& arg_num) {
	return uint(lua_tointeger(state, (int)arg_num));
}
template<> size_t get_lua_arg<size_t>(lua_State* state, const size_t& arg_num) {
	return size_t(lua_tointeger(state, (int)arg_num));
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

net<TCP_protocol>* lua_bindings::n = NULL;
bot_handler* lua_bindings::handler = NULL;
bot_states* lua_bindings::states = NULL;
config* lua_bindings::conf = NULL;

lua_bindings::lua_bindings(net<TCP_protocol>* n, bot_handler* handler, bot_states* states, config* conf) {
	lua_bindings::n = n;
	lua_bindings::handler = handler;
	lua_bindings::states = states;
	lua_bindings::conf = conf;
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
		map<string, pair<string, string> >* users = states->get_users();
		lua_createtable(state, (int)users->size(), (int)users->size()); // TODO: rec size?
		
		// TODO: table handling!
		// add users to table
		for(map<string, pair<string, string> >::iterator user_iter = users->begin(); user_iter != users->end(); user_iter++) {
			lua_pushstring(state, user_iter->first.c_str());
			lua_pushstring(state, user_iter->second.first.c_str());
		}
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
		lua_pushboolean(state, conf->is_owner(get<0>(args)));
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
