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

#include "lua_unibot.hpp"
#include "floor/floor.hpp"

#define __REGISTER_FUNCTION(func_name) lua_register(__current_lua_state, #func_name, &lua_bindings::func_name);

#define BIND_CPP_FUNTIONS_TO_LUA(state) {			\
	lua_State* __current_lua_state = state;			\
	__LUA_FUNCTION_BINDINGS(__REGISTER_FUNCTION);	\
}

lua::lua(floor_irc_net* n_, bot_handler* handler_, bot_states* states_, config* conf_) :
handler(handler_), states(states_), conf(conf_) {
	bindings = new lua_bindings(n_, handler, states, conf, this, &lua::reload_scripts);
	
	reload_scripts();
}

lua::~lua() {
	for(auto script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
		delete (script_iter->second);
	}
	script_store.clear();
	
	delete bindings;
}

void lua::register_functions(lua_script* script) {
	BIND_CPP_FUNTIONS_TO_LUA(script->state);
}

void lua::reload_scripts() {
	const string main_scripts_folder = lua_script_folder();
	deque<string> folders { main_scripts_folder };
	set<string> folder_blacklist { "include" };
	
	for(;;) {
		if(folders.size() == 0) break;
		
		const auto& cur_folder = folders.front();
		const auto lua_files = core::get_file_list(cur_folder);
		for(const auto& lua_file : lua_files) {
			if(lua_file.second == file_io::FILE_TYPE::DIR &&
			   lua_file.first != "." && lua_file.first != ".." &&
			   folder_blacklist.count(lua_file.first) == 0) {
				// sub-directory
				folders.emplace_back(cur_folder + lua_file.first);
			}
			else if(lua_file.first.length() > 4 &&
					lua_file.first.substr(lua_file.first.length() - 3, 3) == "lua") {
				// lua script/file
				reload_script(lua_file.first);
			}
		}
		folders.pop_front();
	}

	log_debug("lua scripts loaded!");
}

void lua::reload_script(const string& filename) {
	string script_filename = lua_script_folder(filename);
	if(script_store.count(script_filename) > 0) {
		// unload/delete script that was already loaded
		delete script_store[script_filename];
		script_store.erase(script_filename);
	}
	
	lua_script* script = new lua_script(script_filename);
	script_store.emplace(script_filename, script);
	register_functions(script);
	script->load_script();
}

void lua::check_scripts() {
	for(auto script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
		if(lua_status(script_iter->second->state) != 0) {
			log_error("erroneous status of script/state \"%s\"!", script_iter->first);
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
		for(auto script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
			lua_getglobal(script_iter->second->state, "handle_message");
			if(!lua_isfunction(script_iter->second->state, -1)) {
				lua_pop(script_iter->second->state, 1);
				log_error("handle_message is no function in script \"%s\"!", script_iter->first);
				continue;
			}
			
			lua_pushstring(script_iter->second->state, origin.c_str());
			lua_pushstring(script_iter->second->state, target.c_str());
			lua_pushstring(script_iter->second->state, cmd.c_str());
			lua_pushstring(script_iter->second->state, parameters.c_str());
			int err = lua_pcall(script_iter->second->state, 4, 1, -lua_gettop(script_iter->second->state)); // error handler @index #0
			if(err > 0) {
				log_error("lua error #%i while running script \"%s\"!", err, script_iter->first);
			}
		}
	}
	catch(invalidate_scripts_exception e) {
		// this breaks the scripts iteration loop (-> no more scripts are handled using the now invalidated script iterator)
	}
	catch(...) {
		log_error("unknown exception while executing lua scripts!");
	}

}

string lua::lua_script_folder(const string addition) {
	return string(floor::get_data_path() + LUA_SCRIPT_FOLDER + addition);
}

const vector<string> lua::list_scripts() const {
	vector<string> script_list;
	for(auto script_iter = script_store.begin(); script_iter != script_store.end(); script_iter++) {
		script_list.push_back(script_iter->first);
	}
	return script_list;
}
