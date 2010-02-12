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
	// config
	config* conf = new config("/etc/unibot.conf");
	
	// initialize SDL
	if(SDL_Init(0) < 0) {
		logger::log(logger::LT_ERROR, "main.cpp", string(string("couldn't initialize SDL: ") + string(SDL_GetError())).c_str());
		exit(1);
	}
	
	srand((unsigned int)time(NULL));
	
	// initialize net
	net<TCP_protocol>* n = new net<TCP_protocol>(conf);
	n->connect_to_server(conf->get_hostname().c_str(), conf->get_port(), conf->get_port());
	
	bot_states* states = new bot_states();
	bot_handler* bot = new bot_handler(n, states, conf);
	
	unsigned int status = 0;
	n->send_connect(conf->get_bot_name(), conf->get_bot_realname());
	while(n->run()) {
		bot->handle();
		
		if(status == 0 && states->is_identified()) {
			status = 1;
			n->join_channel(conf->get_channel());
		}
		
		// reduce system usage ...
		SDL_Delay(1);
	}
	
	delete bot;
	delete states;
	delete n;
	
	SDL_Quit();

	delete conf;
	
	logger::log(logger::LT_DEBUG, "main.cpp", ">> eol");
	return 0;
}
