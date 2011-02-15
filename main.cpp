/*
 *  UniBot
 *  Copyright (C) 2009 - 2011 Florian Ziesche
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

#include "main.h"

int main(int argc, char* argv[]) {
	// set call path and get absolute path
	set_call_path(argv[0]);
	// set lua script search path
	const string lua_path = string(get_absolute_path()+LUA_SCRIPT_FOLDER);
	setenv("LUA_PATH", string(lua_path + "include/?.lua;" + lua_path + "?.lua").c_str(), 1);

	// event handler
	init_event_handler();
	
	// config
	config* conf;
	try {
#ifndef __WINDOWS__
		conf = new config("/etc/unibot.conf", (ssize_t)argc, (const char**)argv);
#else
		conf = new config("C:/unibot.conf", (ssize_t)argc, (const char**)argv);
#endif
	}
	catch(...) {
		exit(-1);
	}
	
	// initialize SDL
	if(SDL_Init(0) < 0) {
		unibot_error("couldn't initialize SDL: %s", SDL_GetError());
		exit(-1);
	}
	
	srand((unsigned int)time(NULL));
	
	// initialize net
	unibot_irc_net* irc = new unibot_irc_net(conf);
	irc->connect_to_server(conf->get_hostname().c_str(), conf->get_port());
	
	bot_states* states = new bot_states(irc);
	bot_handler* bot = new bot_handler(irc, states, conf);
	
	irc->send_connect(conf->get_bot_name(), conf->get_bot_realname());
	
	while(irc->is_running() || bot->is_running()) {
		SDL_Delay(100); // no need to be fast here
	}
	bool restart = states->is_restart();
	
	delete bot;
	delete states;
	delete irc;
	
	SDL_Quit();

	unibot_debug(">> eol");
	delete conf;
	
	destroy_event_handler();
	
#ifndef __WINDOWS__
	if(restart) {
		cout << "restarting bot ..." << endl;
		
		// get binary name
		string binary = clean_path(argv[0]);
		const size_t slash_pos = binary.rfind('/');
		if(slash_pos != string::npos) binary = binary.substr(slash_pos+1, binary.length()-slash_pos-1);
		
#ifdef __APPLE__
		string restart_cmd = "killall "+binary+" 2&>/dev/null && sleep 1 && "+string(argv[0]);
		system(restart_cmd.c_str());
#else
		system(string("killall "+binary).c_str());
		system("sleep 1");
		system(string(argv[0]).c_str());
#endif
	}
#endif
	
	return 0;
}
