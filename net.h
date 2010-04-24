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

// http://jcatki.no-ip.org:8080/SDL_net/SDL_net_frame.html

#ifndef __NET_H__
#define __NET_H__

#include "platform.h"
#include "net_protocol.h"
#include "net_tcp.h"
#include "config.h"
#include "threading/thread_base.h"

typedef std_protocol<UDPsocket> UDP_protocol;

#define PACKET_MAX_LEN 65536

template <class protocol_policy> class net : public protocol_policy, public thread_base {
public:
	net(config* conf);
	virtual ~net();
	
	virtual void run();
	virtual bool connect_to_server(const char* server_name, const unsigned short int port, const unsigned short int local_port = 65535);
	
	virtual bool is_received_data();
	virtual deque<string>* get_received_data();
	virtual void clear_received_data();
	
	virtual IPaddress* get_local_ip();
	virtual IPaddress* get_server_ip();

protected:
	config* conf;
	protocol_policy protocol;
	
	IPaddress server_ip;
	IPaddress local_ip;
	
	string last_packet_remains;
	size_t received_length;
	size_t packets_per_second;
	size_t last_packet_send;
	deque<string> receive_store;
	deque<string> send_store;
	
	char receive_data[PACKET_MAX_LEN];
	virtual int receive_packet(char* data, const unsigned int max_len);
	virtual void send_packet(const char* data, const unsigned int len);
	virtual int process_packet(const string& data, const unsigned int max_len);
	
};

template <class protocol_policy> net<protocol_policy>::net(config* conf) :
thread_base(), conf(conf), last_packet_remains(""), received_length(0), packets_per_second(0), last_packet_send(0) {
	if(SDLNet_Init() == -1) {
		logger::log(logger::LT_ERROR, "net.h", string(string("couldn't initialize net: ") + string(SDLNet_GetError())).c_str());
		return;
	}
	
	logger::log(logger::LT_DEBUG, "net.h", "net initialized!");
	this->start(); // start thread
}

template <class protocol_policy> net<protocol_policy>::~net() {
	SDLNet_Quit();
	logger::log(logger::LT_DEBUG, "net.h", "net deleted!");
}

template <class protocol_policy> bool net<protocol_policy>::connect_to_server(const char* server_name, const unsigned short int port, const unsigned short int local_port) {
	lock(); // we need to lock the net class, so run() isn't called while we're connecting
	
	try {
		if(!protocol.is_valid()) throw exception();
		
		if(SDLNet_ResolveHost(&server_ip, server_name, port) == -1) {
			logger::log(logger::LT_ERROR, "net.h", string(string("SDLNet_ResolveHost(server): ") + string(SDLNet_GetError())).c_str());
			throw exception();
		}
		
		// currently useless for an irc bot ...
#if 0
		if(SDLNet_ResolveHost(&local_ip, NULL, local_port) == -1) {
			logger::log(logger::LT_ERROR, "net.h", string(string("SDLNet_ResolveHost(client): ") + string(SDLNet_GetError())).c_str());
			throw exception();
		}
#endif
		
		// create server socket
		if(!protocol.open_server_socket(server_ip)) throw exception();
		
#if 0
		// create client socket
		if(!protocol.open_client_socket(local_ip)) throw exception();
#endif
		
		// connection created - data transfer is now possible
		logger::log(logger::LT_DEBUG, "net.h", "connect_to_server(): successfully connected to server!");
	}
	catch(...) {
		logger::log(logger::LT_ERROR, "net.h", "connect_to_server(): failed to connect to server!");
		unlock();
		set_thread_should_finish(); // and quit ...
		return false;
	}
	
	unlock();
	return true;
}

template <class protocol_policy> void net<protocol_policy>::run() {
	int len = 0, used = 0;
	
	// receive data - if possible
	try {
		if(protocol.valid_sockets()) {
			if(protocol.server_ready()) {
				//cout << "server_ready" << endl;
				memset(receive_data, 0, PACKET_MAX_LEN);
				len = receive_packet(receive_data, PACKET_MAX_LEN);
				if(len <= 0) {
					// failure, end bot
					throw exception();
				}
				else {
					string data = receive_data;
					if(last_packet_remains.length() > 0) {
						data = last_packet_remains + data;
						len += (int)last_packet_remains.length();
						last_packet_remains = "";
					}
					
					used = process_packet(data, len);
					
					len -= used;
					if(used == 0 || len > 0) {
						last_packet_remains = data.substr(used, len);
					}
				}
			}
		}
		else throw exception();
	}
	catch(...) {
		// something is wrong, finsh and return
		logger::log(logger::LT_ERROR, "net.h", "run(): unknown net error, exiting ...");
		set_thread_should_finish();
		return;
	}
	
	// send data - if possible
	if(!send_store.empty()) {
		deque<string>::iterator send_end = send_store.end();
		if(packets_per_second != 0 && last_packet_send > SDL_GetTicks()-1000) {
			// wait
		}
		else {
			if(packets_per_second != 0 && send_store.size() > packets_per_second) {
				send_end = send_store.begin()+packets_per_second;
			}
			
			for(deque<string>::iterator send_iter = send_store.begin(); send_iter != send_end; send_iter++) {
				send_packet(send_iter->c_str(), (int)send_iter->length());
			}
			if(packets_per_second != 0) last_packet_send = SDL_GetTicks();
			
			send_store.erase(send_store.begin(), send_end);
		}
	}
}

template <class protocol_policy> int net<protocol_policy>::receive_packet(char* data, const unsigned int max_len) {
	if(!protocol.is_valid()) {
		logger::log(logger::LT_ERROR, "net.h", "receive_packet(): invalid protocol and/or sockets!");
		return -1;
	}
	
	// receive the package
	int len = protocol.server_receive(data, max_len);
	// received packet length is equal or less than zero, return -1
	if(len <= 0) {
		logger::log(logger::LT_ERROR, "net.h", "receive_packet(): invalid data received!");
		return -1;
	}
	
	return len;
}

template <class protocol_policy> int net<protocol_policy>::process_packet(const string& data, const unsigned int max_len) {
	size_t old_pos = 0, pos = 0;
	size_t lb_offset = 1;
	const size_t len = data.length();
	for(;;) {
		// handle \n and \r\n newlines
		if((pos = data.find("\r", old_pos)) == string::npos) {
			if((pos = data.find("\n", old_pos)) == string::npos) {
				break;
			}
			else lb_offset = 1;
		}
		else {
			if(pos+1 >= len) {
				// \n not received yet
				break;
			}
			if(data[pos+1] != '\n') {
				// \r must be followed by \n!
				break;
			}
			pos++;
			lb_offset = 2;
		}
		
		receive_store.push_back(data.substr(old_pos, pos - old_pos - lb_offset + 1));
		//cout << "received (" << old_pos << "/" << (pos - old_pos) << "/" << data.length() << "): " << receive_store.back() << endl;
		old_pos = pos + 1;
	}
	//cout << ":: end " << old_pos << endl;
	
	received_length += old_pos;
	return (int)old_pos;
}

template <class protocol_policy> void net<protocol_policy>::send_packet(const char* data, const unsigned int len) {
	if(!protocol.server_send(data, len)) {
		logger::log(logger::LT_ERROR, "net.h", "send_packet(): couldn't send packet!");
	}
	else if(conf->get_verbosity() >= logger::LT_DEBUG) {
		cout << "send (" << len << "): " << string(data).substr(0, len-1) << endl;
	}
}

template <class protocol_policy> bool net<protocol_policy>::is_received_data() {
	return !receive_store.empty();
}

template <class protocol_policy> deque<string>* net<protocol_policy>::get_received_data() {
	return &receive_store;
}

template <class protocol_policy> void net<protocol_policy>::clear_received_data() {
	receive_store.clear();
}

template <class protocol_policy> IPaddress* net<protocol_policy>::get_local_ip() {
	return &local_ip;
}

template <class protocol_policy> IPaddress* net<protocol_policy>::get_server_ip() {
	return &server_ip;
}

#endif
