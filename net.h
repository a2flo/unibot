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

typedef std_protocol<UDPsocket> UDP_protocol;

#define PACKET_MAX_LEN 4096

template <class protocol_policy> class net : public protocol_policy {
public:
	net(config* conf);
	~net();
	
	bool run();
	bool connect_to_server(const char* server_name, const unsigned short int port, const unsigned short int local_port);
	void disconnect();
	
	bool is_received_data();
	deque<string>* get_received_data();
	void clear_received_data();
	
	void send(string data);
	void send_channel_msg(string msg);
	void send_private_msg(string where, string msg);
	void send_kick(string who, string reason);
	void send_connect(string name, string real_name);
	void send_identify(string password);
	void part();
	void quit();
	void join_channel(string channel);
	
	IPaddress* get_local_ip();
	IPaddress* get_server_ip();

protected:
	protocol_policy protocol;
	config* conf;
	
	IPaddress server_ip;
	IPaddress local_ip;
	
	string last_packet_remains;
	deque<string> receive_store;
	deque<string> send_store;
	
	char receive_data[PACKET_MAX_LEN];
	int receive_packet(char* data, const unsigned int max_len);
	void send_packet(const char* data, const unsigned int len);
	int process_packet(const string& data, const unsigned int max_len);
	
};

template <class protocol_policy> net<protocol_policy>::net(config* conf) : conf(conf) {
	if(SDLNet_Init() == -1) {
		logger::log(logger::LT_ERROR, "net.h", string(string("couldn't initialize net: ") + string(SDLNet_GetError())).c_str());
		return;
	}
	
	last_packet_remains = "";
	logger::log(logger::LT_DEBUG, "net.h", "net initialized!");
}

template <class protocol_policy> net<protocol_policy>::~net() {
	SDLNet_Quit();
	logger::log(logger::LT_DEBUG, "net.h", "net deleted!");
}

template <class protocol_policy> bool net<protocol_policy>::connect_to_server(const char* server_name, const unsigned short int port, const unsigned short int local_port) {
	if(!protocol.is_valid()) return false;
	
	if(SDLNet_ResolveHost(&server_ip, server_name, port) == -1) {
		logger::log(logger::LT_ERROR, "net.h", string(string("SDLNet_ResolveHost(server): ") + string(SDLNet_GetError())).c_str());
		return false;
	}
	
	// currently useless for an irc bot ...
#if 0
	if(SDLNet_ResolveHost(&local_ip, NULL, local_port) == -1) {
		logger::log(logger::LT_ERROR, "net.h", string(string("SDLNet_ResolveHost(client): ") + string(SDLNet_GetError())).c_str());
		return false;
	}
#endif
	
	// create server socket
	if(!protocol.open_server_socket(server_ip)) return false;
	
#if 0
	// create client socket
	if(!protocol.open_client_socket(local_ip)) return false;
#endif
	
	// connection created - data transfer is now possible
	logger::log(logger::LT_DEBUG, "net.h", "connect_to_server(): successfully connected to server!");
	
	return true;
}

template <class protocol_policy> bool net<protocol_policy>::run() {
	int len = 0, used = 0;
	
	// receive data - if possible
	if(protocol.valid_sockets()) {
		if(protocol.server_ready()) {
			//cout << "server_ready" << endl;
			memset(receive_data, 0, PACKET_MAX_LEN);
			len = receive_packet(receive_data, PACKET_MAX_LEN);
			if(len <= 0) {
				// failure, end bot
				return false;
			}
			else {
				string data = receive_data;
				if(last_packet_remains.length() > 0) {
					data = last_packet_remains + data;
					len += (int)last_packet_remains.length();
					last_packet_remains = "";
				}
				//cout << "received " << len << endl;
				int loop = 0;
				while(len > 0) {
					used = process_packet(data, len);
					//cout << "used: " << used << ", rest: " << (len-used) << endl;
					
					len -= used;
					if(used == 0) {
						if(loop > 0) {
							last_packet_remains = data.substr(used, len);
						}
						else {
							// lost data ...
							logger::log(logger::LT_ERROR, "net.h", "run(): lost data - server down or disconnect?");
							len = 0;
						}
					}
					loop++;
				}
			}
		}
	}
	else return false;
	
	// send data - if possible
	if(!send_store.empty()) {
		for(deque<string>::iterator send_iter = send_store.begin(); send_iter != send_store.end(); send_iter++) {
			send_packet(send_iter->c_str(), (int)send_iter->length());
		}
		send_store.clear();
	}
	
	return true;
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
	while((pos = data.find("\n", old_pos)) != string::npos) {
		receive_store.push_back(data.substr(old_pos, pos - old_pos - 1));
		//cout << "received (" << old_pos << "/" << (pos - old_pos) << "/" << data.length() << "): " << receive_store.back() << endl;
		old_pos = pos + 1;
	}
	//cout << "end " << old_pos << endl;
	
	return (int)old_pos;
}

template <class protocol_policy> void net<protocol_policy>::send_packet(const char* data, const unsigned int len) {
	if(!protocol.server_send(data, len)) {
		logger::log(logger::LT_ERROR, "net.h", "send_packet(): coudln't send packet!");
	}
	else cout << "send (" << len << "): " << string(data).substr(0, len-1) << endl;
}

template <class protocol_policy> void net<protocol_policy>::send(string data) {
	send_store.push_back(data + "\n");
}

template <class protocol_policy> void net<protocol_policy>::send_connect(string name, string real_name) {
	send("NICK " + name);
	send("USER " + name + " 0 * " + real_name);
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

template <class protocol_policy> void net<protocol_policy>::send_private_msg(string where, string msg) {
	send("PRIVMSG " + where + " :" + msg);
}

template <class protocol_policy> void net<protocol_policy>::send_identify(string password) {
	send_private_msg("NickServ", "identify " + password);
}

template <class protocol_policy> void net<protocol_policy>::send_channel_msg(string msg) {
	send("PRIVMSG " + conf->get_channel() + " :" + msg);
}

template <class protocol_policy> void net<protocol_policy>::send_kick(string who, string reason) {
	send("KICK " + conf->get_channel() + " " + who + " :" + reason);
}

template <class protocol_policy> void net<protocol_policy>::part() {
	send("PART " + conf->get_channel() + " :EOL");
}

template <class protocol_policy> void net<protocol_policy>::quit() {
	send("QUIT :EOL");
}

template <class protocol_policy> void net<protocol_policy>::join_channel(string channel) {
	send("JOIN " + channel);
}

template <class protocol_policy> IPaddress* net<protocol_policy>::get_local_ip() {
	return &local_ip;
}

template <class protocol_policy> IPaddress* net<protocol_policy>::get_server_ip() {
	return &server_ip;
}

#endif
