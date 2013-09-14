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
	// init floor in console only mode
	floor::init(argv[0], "", true);
	
	// set lua script search path
	const string lua_path = string(floor::get_absolute_path()+LUA_SCRIPT_FOLDER);
	setenv("LUA_PATH", string(lua_path + "include/?.lua;" + lua_path + "?.lua").c_str(), 1);
	
	// event handler
	init_event_handler();
	
	// config
	config* conf = nullptr;
	const string conf_name = (argc > 1 ? argv[1] : "unibot.conf");
	const array<array<string, 2>, 2> config_filenames {{
		{{ "/etc/"+conf_name, "unix" }},
		{{ "C:/"+conf_name, "windows" }}
	}};
	for(size_t i = 0; i < config_filenames.size(); i++) {
		try {
			conf = new config(config_filenames[i][0], config_filenames[i][1], (ssize_t)argc, (const char**)argv);
			break;
		}
		catch(...) {
			conf = nullptr;
			continue;
		}
	}
	if(conf == nullptr) {
		cout << "ERROR: couldn't open the unibot config file!" << endl;
		exit(-1);
	}
	
	// initialize SDL
	if(SDL_Init(0) < 0) {
		log_error("couldn't initialize SDL: %s", SDL_GetError());
		exit(-1);
	}
	
	srand((unsigned int)time(nullptr));
	
	// initialize net
	bool restart_bot = false;
	do {
		if(restart_bot) {
			log_debug("restarting bot ...");
		}
		
		floor_irc_net* irc = new floor_irc_net();
		if(irc->connect_to_server(conf->get_hostname().c_str(), conf->get_port())) {
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
	
	SDL_Quit();

	log_debug(">> eol");
	delete conf;
	
	destroy_event_handler();
	
	return 0;
}
