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

#include "main.h"

int main(int argc, char* argv[]) {
	// set call path and get absolute path
	set_call_path(argv[0]);
	// set lua script search path
	setenv("LUA_PATH", (get_absolute_path()+LUA_SCRIPT_FOLDER).c_str(), 1);
	
	// config
	config* conf;
	try {
		conf = new config("/etc/unibot.conf");
	}
	catch(...) {
		exit(-1);
	}
	
	// initialize SDL
	if(SDL_Init(0) < 0) {
		logger::log(logger::LT_ERROR, "main.cpp", string(string("couldn't initialize SDL: ") + string(SDL_GetError())).c_str());
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
	
	delete bot;
	delete states;
	delete irc;
	
	SDL_Quit();

	logger::log(logger::LT_DEBUG, "main.cpp", ">> eol");
	delete conf;
	
	return 0;
}
