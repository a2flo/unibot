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

#ifndef __LUA_H__
#define __LUA_H__

#include "platform.h"
#include "net.h"
#include "bot_states.h"
#include "bot_handler.h"
#include "config.h"
#include "util.h"
#include "lua_bindings.h"

#define LUA_SCRIPT_FOLDER "scripts/"

class lua {
public:
	lua(net<TCP_protocol>* n, bot_handler* handler, bot_states* states, config* conf);
	~lua();
	
	void reload_scripts();
	void reload_script(const string& filename);
	
	void handle_message(const string& origin, const string& target, const string& msg);
	
protected:
	net<TCP_protocol>* n;
	bot_handler* handler;
	bot_states* states;
	config* conf;
	lua_bindings* bindings;
	
	struct lua_script;
	void register_functions(lua_script* script);
	void check_scripts();
	const char* lua_script_folder(const string addition = "");
	
	template<int level> static int lua_error_handler(lua_State* state) {
		// get debug info
		lua_Debug dbg;
		lua_getstack(state, level, &dbg);
		lua_getinfo(state, "nSlu", &dbg);
		
		// get error str
		string error_str = lua_tostring(state, -1);
		
		// print error message
		logger::log(logger::LT_ERROR, "lua.h", string("lua error: "+error_str+", in line "+to_str(dbg.currentline)+"\nsrc:\n"+dbg.short_src).c_str());
		return 0;
	}
	
	struct lua_script {
		lua_State* state;
		string script_filename;
		
		lua_script(const string& script_filename) : state(luaL_newstate()), script_filename(script_filename) {
			luaL_openlibs(state);
		}
		~lua_script() {
			lua_close(state);
		}
		
		void load_script() {
			// push level 0 error handler (level 0 is necessary, b/c we're always in level 0 while loading)
			lua_pushcfunction(state, lua_error_handler<0>);
			bool load_error = false;
			load_error |= luaL_loadfile(state, script_filename.c_str());
			load_error |= lua_pcall(state, 0, 0, -lua_gettop(state)); // error handler @index #0
			if(load_error) {
				logger::log(logger::LT_ERROR, "lua.h", string("error loading lua script \""+script_filename+"\"!").c_str());
				lua_pop(state, 1);
			}
			
			// pop level 0 error handler and push level 1 error handler
			lua_pop(state, 1);
			lua_pushcfunction(state, lua_error_handler<1>);
		}
	};
	// <filename, pointer to script object>
	map<string, lua_script*> script_store;
	
};

#endif
