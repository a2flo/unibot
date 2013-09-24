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

#include "main.hpp"

int main(int argc, char* argv[]) {
	// parse args + set data path and config file name
#if !defined(__WINDOWS__)
	string datapath ="/etc/unibot/";
#else
	string datapath ="/c/unibot/";
#endif
	string config_name = "config.xml";
	
	for(int arg_pair = 0; arg_pair < ((argc - 1) / 2); arg_pair += 2) {
		const auto arg_name = string(argv[arg_pair + 1]);
		const auto arg_value = string(argv[arg_pair + 2]);
		if(!arg_name.empty() && !arg_value.empty()) {
			if(arg_name == "--datapath") {
				datapath = arg_value;
			}
			else if(arg_name == "--config") {
				config_name = arg_value;
			}
		}
	}
	
	// init floor in console only mode
	floor::init(argv[0], datapath.c_str(), true, config_name);
	
	// set lua script search path
	const string lua_path = lua::lua_script_folder();
	if(!file_io::is_directory(lua_path)) {
		log_error("lua scripts folder (%s) does not exist or is inaccessible!");
		floor::destroy();
		return -1;
	}
	setenv("LUA_PATH", string(lua_path + "include/?.lua;" + lua_path + "?.lua").c_str(), 1);
	
	// event handler
	init_event_handler();
	
	// config
	config* conf = new config((ssize_t)argc, (const char**)argv);
	
	// initialize net
	bool restart_bot = false;
	do {
		if(restart_bot) {
			log_debug("restarting bot ...");
		}
		
		floor_irc_net* irc = new floor_irc_net(conf->get_ssl());
		if(irc->connect_to_server(conf->get_hostname(), conf->get_port())) {
			bot_states* states = new bot_states(irc);
			bot_handler* bot = new bot_handler(irc, states, conf);
			
			irc->send_connect(conf->get_bot_name(), conf->get_bot_realname());
			
			while(irc->is_running() || bot->is_running()) {
				SDL_Delay(1000); // no need to be fast here
			}
			restart_bot = states->is_restart();
			
			delete bot;
			delete states;
		}
		else {
			// connection failed, wait 5s before reconnect
			SDL_Delay(5000);
			restart_bot = true;
		}
		delete irc;
	} while(restart_bot);

	// cleanup/destroy
	log_debug(">> eol");
	delete conf;
	destroy_event_handler();
	floor::destroy();
	
	return 0;
}
