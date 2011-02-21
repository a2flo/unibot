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

// http://jcatki.no-ip.org:8080/SDL_net/SDL_net_frame.html

#ifndef __NET_PROTOCOL_H__
#define __NET_PROTOCOL_H__

#include "platform.h"

template <class socket_type>
struct std_protocol {
	// these declarations aren't actually needed, but for the sake of understanding the "interface"
	// or getting an overview of it, i put these here
	
public:
	std_protocol() : valid(true), server_socket(NULL), client_socket(NULL), socketset(NULL) {};
	
	bool is_valid();
	bool valid_sockets();
	
	bool server_ready();
	bool client_ready();
	
	bool open_server_socket(IPaddress& server_ip);
	bool open_client_socket(IPaddress& client_ip);
	
	int server_receive(void* data, const unsigned int max_len);
	int client_receive(void* data, const unsigned int max_len);
	
	bool server_send(const char* data, const int len);
	bool client_send(const char* data, const int len);
	
protected:
	bool valid;
	socket_type* server_socket;
	socket_type* client_socket;
	SDLNet_SocketSet socketset;
};

#endif
