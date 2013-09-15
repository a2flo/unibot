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

#ifndef __UNIBOT_LUA_UNIBOT_HPP__
#define __UNIBOT_LUA_UNIBOT_HPP__

#include "core/platform.hpp"
#include "net/net.hpp"
#include "net/irc_net.hpp"
#include "bot_states.hpp"
#include "bot_handler.hpp"
#include "config.hpp"
#include "core/util.hpp"
#include "lua_bindings.hpp"

#define LUA_SCRIPT_FOLDER "scripts" FLOOR_OS_DIR_SLASH
#define UNIBOT_MAX_LUA_SCRIPT_SIZE (16 * 1024 * 1024)

class lua {
public:
	lua(floor_irc_net* n, bot_handler* handler, bot_states* states, config* conf);
	~lua();
	
	void reload_scripts();
	void reload_script(const string& filename);
	const vector<string> list_scripts() const;
	
	void handle_message(const string& origin, const string& target, const string& msg);
	
	static string lua_script_folder(const string addition = "");
	
protected:
	floor_irc_net* n;
	bot_handler* handler;
	bot_states* states;
	config* conf;
	lua_bindings* bindings;
	
	struct lua_script;
	void register_functions(lua_script* script);
	void check_scripts();
	
	template<int level> static int lua_error_handler(lua_State* state) {
		// get debug info
		lua_Debug dbg;
		lua_getstack(state, level, &dbg);
		lua_getinfo(state, "nSlu", &dbg);
		
		// get error str
		const char* error_str = lua_tostring(state, -1);
		
		// print error message
		log_error("lua error: %s, in line %i\nsrc:\n%s", (error_str != nullptr ? error_str : "<unknown>"), dbg.currentline, dbg.short_src);
		return 0;
	}
	
	struct lua_script {
		lua_State* state;
		string script_filename;
		
		lua_script(const string& script_filename_) : state(luaL_newstate()), script_filename(script_filename_) {
			luaL_openlibs(state);
		}
		~lua_script() {
			lua_close(state);
		}
		
		void load_script() {
			// push level 0 error handler (level 0 is necessary, b/c we're always in level 0 while loading)
			lua_pushcfunction(state, lua_error_handler<0>);
			bool load_error = false;

			fstream script;
			script.open(script_filename.c_str(), ios::in);
			if(script.is_open()) {
				script.seekg(0, ios::end);
				const size_t script_size = (size_t)script.tellg();
				
				if(script_size > UNIBOT_MAX_LUA_SCRIPT_SIZE) {
					load_error = true;
					log_error("lua script %s is too large (size: %u, allowed: %u)!", script_filename, script_size, UNIBOT_MAX_LUA_SCRIPT_SIZE);
				}
				else {
					script.seekg(0, ios::beg);
					char* script_data = new char[script_size+1];
					script.read(script_data, script_size);
					script_data[script_size] = 0;
					
					string script_string = script_data;
#ifdef __WINDOWS__
					string script_path = get_absolute_path() + LUA_SCRIPT_FOLDER + "include\\?.lua";
					script_path = find_and_replace(script_path, "\\", "\\\\");
					script_string = "package.path = package.path .. \";" + script_path + "\"\n" + script_string;
#endif
					luaL_loadstring(state, script_string.c_str());
					delete [] script_data;
				}
			}
			else load_error = true;
			script.close();

			load_error |= (lua_pcall(state, 0, 0, -lua_gettop(state)) == 0 ? false : true); // error handler @index #0
			if(load_error) {
				log_error("error loading lua script \"%s\"!", script_filename);
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
