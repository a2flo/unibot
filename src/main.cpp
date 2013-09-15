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
	floor::init(argv[0],
#if !defined(WIN_UNIXENV)
				"/etc/unibot/",
#else
				"/c/unibot/",
#endif
				true);
	
	// set lua script search path
	const string lua_path = string(floor::get_absolute_path()+LUA_SCRIPT_FOLDER);
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

	// cleanup/destroy
	log_debug(">> eol");
	delete conf;
	destroy_event_handler();
	floor::destroy();
	
	return 0;
}
