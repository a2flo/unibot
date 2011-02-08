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

#ifndef __NET_TCP_H__
#define __NET_TCP_H__

#include "platform.h"
#include "net_protocol.h"

template<> struct std_protocol<TCPsocket> {
	std_protocol<TCPsocket>() : server_set(false), client_set(false), valid(true), server_socket(NULL), client_socket(NULL) {
		// initialize socket set
		socketset = SDLNet_AllocSocketSet(2);
		if(socketset == NULL) {
			logger::log(logger::LT_ERROR, "net_tcp.h", string(string("couldn't create socket set: ") + string(SDLNet_GetError())).c_str());
			valid = false;
		}
	}
	virtual ~std_protocol<TCPsocket>() {
		if(socketset != NULL) {
			if(server_socket != NULL) {
				SDLNet_TCP_DelSocket(socketset, server_socket);
				SDLNet_TCP_Close(server_socket);
			}
			if(client_socket != NULL) {
				SDLNet_TCP_DelSocket(socketset, client_socket);
				SDLNet_TCP_Close(client_socket);
			}
		}
	}
	
	bool is_valid() {
		if(socketset == NULL) valid = false;
		if(server_set && server_socket == NULL) valid = false;
		if(client_set && client_socket == NULL) valid = false;	
		return valid;
	}
	
	bool valid_sockets() {
		if(is_valid()) {
			int active_sockets = SDLNet_CheckSockets(socketset, 0);
			if((server_set || client_set) && (active_sockets == -1 || active_sockets > 2)) {
				logger::log(logger::LT_ERROR, "net_tcp.h", "valid_sockets(): invalid socket activity!");
				valid = false;
				return false;
			}
			return true;
		}
		logger::log(logger::LT_ERROR, "net_tcp.h", "valid_sockets(): invalid sockets!");
		return false;
	}
	
	bool open_server_socket(IPaddress& server_ip) {
		if(!is_valid()) return false;
		server_set = true;
		
		server_socket = SDLNet_TCP_Open(&server_ip);
		if(server_socket == NULL) {
			logger::log(logger::LT_ERROR, "net_tcp.h", string(string("open_server_socket(): ") + string(SDLNet_GetError())).c_str());
			return false;
		}
		
		SDLNet_TCP_AddSocket(socketset, server_socket);
		return true;
	}
	bool open_client_socket(IPaddress& client_ip) {
		if(!is_valid()) return false;
		client_set = true;
		
		client_socket = SDLNet_TCP_Open(&client_ip);
		if(client_socket == NULL) {
			logger::log(logger::LT_ERROR, "net_tcp.h", string(string("open_client_socket(): ") + string(SDLNet_GetError())).c_str());
			return false;
		}
		
		SDLNet_TCP_AddSocket(socketset, client_socket);
		return true;
	}
	
	int server_receive(void* data, const unsigned int max_len) {
		return SDLNet_TCP_Recv(server_socket, data, max_len);
	}
	
	int client_receive(void* data, const unsigned int max_len) {
		return SDLNet_TCP_Recv(client_socket, data, max_len);
	}
	
	bool server_ready() {
		return SDLNet_SocketReady(server_socket);
	}
	
	bool client_ready() {
		return SDLNet_SocketReady(client_socket);
	}
	
	bool server_send(const char* data, const int len) {
		int send_len = SDLNet_TCP_Send(server_socket, data, len);
		if(send_len != len) {
			logger::log(logger::LT_ERROR, "net_tcp.h", string(string("server_send(): invalid data send: ") + string(SDLNet_GetError())).c_str());
			return false;
		}
		return true;
	}
	
	bool client_send(const char* data, const int len) {
		int send_len = SDLNet_TCP_Send(client_socket, data, len);
		if(send_len != len) {
			logger::log(logger::LT_ERROR, "net_tcp.h", string(string("client_send(): invalid data send: ") + string(SDLNet_GetError())).c_str());
			return false;
		}
		return true;
	}
	
protected:
	bool server_set;
	bool client_set;
	bool valid;
	TCPsocket server_socket;
	TCPsocket client_socket;
	SDLNet_SocketSet socketset;
	
};

typedef std_protocol<TCPsocket> TCP_protocol;


#endif
