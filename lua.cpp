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

#include "lua.h"

#define __REGISTER_FUNCTION(func_name) lua_register(__current_lua_state, #func_name, &lua_bindings::func_name);

#define BIND_CPP_FUNTIONS_TO_LUA(state) {			\
	lua_State* __current_lua_state = state;			\
	__LUA_FUNCTION_BINDINGS(__REGISTER_FUNCTION);	\
}

lua::lua(unibot_irc_net* n, bot_handler* handler, bot_states* states, config* conf) : n(n), handler(handler), states(states), conf(conf) {
	bindings = new lua_bindings(n, handler, states, conf, this, &lua::reload_scripts);
	
	reload_scripts();
}

lua::~lua() {
	for(map<string, lua_script*>::iterator script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
		delete (script_iter->second);
	}
	script_store.clear();
	
	delete bindings;
}

void lua::register_functions(lua_script* script) {
	BIND_CPP_FUNTIONS_TO_LUA(script->state);
}

void lua::reload_scripts() {
#ifdef WIN32
#error "no windows support yet"
#endif
	
	struct dirent** namelist;
	
	int n = scandir(lua_script_folder("."), &namelist, 0, alphasort);
	if(n > 0) {
		for(int j = 1; j < n; j++) {
			string fname = namelist[j]->d_name;
			if(namelist[j]->d_type != DT_DIR && fname.length() > 4) {
				string ext = fname.substr(fname.length() - 3, 3);
				if(ext == "lua") {
					reload_script(fname);
				}
			}
		}
		
		delete [] namelist;
	}
	
	logger::log(logger::LT_DEBUG, "lua.cpp", "lua scripts loaded!");
}

void lua::reload_script(const string& filename) {
	string script_filename = lua_script_folder(filename);
	if(script_store.count(script_filename) > 0) {
		// unload/delete script that was already loaded
		delete script_store[script_filename];
		script_store.erase(script_filename);
	}
	
	script_store[script_filename] = new lua_script(script_filename);
	register_functions(script_store[script_filename]);
	script_store[script_filename]->load_script();
}

void lua::check_scripts() {
	for(map<string, lua_script*>::iterator script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
		if(lua_status(script_iter->second->state) != 0) {
			logger::log(logger::LT_ERROR, "lua.cpp", string("erroneous status of script/state \""+script_iter->first+"\"!").c_str());
		}
	}
}

void lua::handle_message(const string& origin, const string& target, const string& msg) {
	// get cmd ("!whatever parameters" -> cmd: whatever)
	size_t cmd_end = msg.find(' ', 0);
	string cmd = (cmd_end != string::npos ? msg.substr(0, cmd_end) : msg);
	string parameters = (cmd_end != string::npos ? msg.substr(cmd_end+1, msg.length()-cmd_end-1) : msg);
	
	check_scripts();
	
	try {
		for(map<string, lua_script*>::iterator script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
			lua_getglobal(script_iter->second->state, "handle_message");
			if(!lua_isfunction(script_iter->second->state, -1)) {
				lua_pop(script_iter->second->state, 1);
				logger::log(logger::LT_ERROR, "lua.cpp", string("handle_message is no function in script \""+script_iter->first+string("\"!")).c_str());
				continue;
			}
			
			lua_pushstring(script_iter->second->state, origin.c_str());
			lua_pushstring(script_iter->second->state, target.c_str());
			lua_pushstring(script_iter->second->state, cmd.c_str());
			lua_pushstring(script_iter->second->state, parameters.c_str());
			int err = lua_pcall(script_iter->second->state, 4, 1, -lua_gettop(script_iter->second->state)); // error handler @index #0
			if(err > 0) {
				logger::log(logger::LT_ERROR, "lua.cpp", string(string("lua error #") + to_str(err) + string(" while running script \"")+script_iter->first+string("\"!")).c_str());
			}
		}
	}
	catch(invalidate_scripts_exception&) {
		// this breaks the scripts iteration loop (-> no more scripts are handled using the now invalidated script iterator)
	}
	catch(...) {
		logger::log(logger::LT_ERROR, "lua.cpp", string("handle_message(): unknown exception while executing lua scripts!").c_str());
	}

}

const char* lua::lua_script_folder(const string addition) {
	const string script_folder = get_absolute_path() + string(LUA_SCRIPT_FOLDER) + addition;
	return script_folder.c_str();
}
