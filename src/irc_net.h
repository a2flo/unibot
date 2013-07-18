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

#ifndef __UNIBOT_IRC_NET_H__
#define __UNIBOT_IRC_NET_H__

#include "platform.h"
#include "net.h"
#include "net_protocol.h"
#include "net_tcp.h"
#include "config.h"

template <class protocol_policy> class irc_net : public net<protocol_policy> {
public:
	irc_net(config* conf);
	virtual ~irc_net() {} 
	
	void send(string data);
	void send_channel_msg(string msg);
	void send_private_msg(string where, string msg);
	void send_action_msg(string where, string msg);
	void send_ctcp_msg(string where, string type, string msg);
	void send_ctcp_request(string where, string type);
	void send_kick(string who, string reason);
	void send_connect(string name, string real_name);
	void send_identify(string password);
	void part();
	void quit();
	void join_channel(string channel);
	
protected:
	// seems like the compiler needs to know about these when using template inheritance
	using net<protocol_policy>::conf;
	using net<protocol_policy>::server_ip;
	using net<protocol_policy>::local_ip;
	using net<protocol_policy>::receive_store;
	using net<protocol_policy>::send_store;
	using net<protocol_policy>::packets_per_second;
};

// unibot_irc_net
typedef irc_net<UNIBOT_NET_PROTOCOL> unibot_irc_net;


template <class protocol_policy> irc_net<protocol_policy>::irc_net(config* conf) :
net<protocol_policy>(conf) {
	packets_per_second = 5;
	this->set_thread_delay(20); // 20ms should suffice
}

template <class protocol_policy> void irc_net<protocol_policy>::send(string data) {
	if(data.find("\n") != string::npos) {
		// treat \n as new msg, split string and send each line as new msg (with the same type -> string till first ':')
		size_t colon_pos = data.find(":");
		string msg_type = data.substr(0, colon_pos+1);
		
		size_t old_newline_pos = colon_pos;
		size_t newline_pos = 0;
		this->lock(); // lock before modifying send store
		while((newline_pos = data.find("\n", old_newline_pos+1)) != string::npos) {
			string msg = data.substr(old_newline_pos+1, newline_pos-old_newline_pos-1);
			send_store.push_back(msg_type + msg + "\n");
			old_newline_pos = newline_pos;
		}
		this->unlock();
	}
	// just send the msg
	else {
		this->lock(); // lock before modifying send store
		send_store.push_back(data + "\n");
		this->unlock();
	}
}

template <class protocol_policy> void irc_net<protocol_policy>::send_connect(string name, string real_name) {
	send("NICK " + name);
	send("USER " + name + " 0 * :" + real_name);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_private_msg(string where, string msg) {
	send("PRIVMSG " + where + " :" + msg);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_action_msg(string where, string msg) {
	send("PRIVMSG " + where + " :" + (char)0x01 + "ACTION " + msg + (char)0x01);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_ctcp_msg(string where, string type, string msg) {
	send("NOTICE " + where + " :" + (char)0x01 + type + " " + msg + (char)0x01);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_ctcp_request(string where, string type) {
	send("PRIVMSG " + where + " :" + (char)0x01 + type + (char)0x01);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_identify(string password) {
	send_private_msg("NickServ", "identify " + password);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_channel_msg(string msg) {
	send("PRIVMSG " + conf->get_channel() + " :" + msg);
}

template <class protocol_policy> void irc_net<protocol_policy>::send_kick(string who, string reason) {
	send("KICK " + conf->get_channel() + " " + who + " :" + reason);
}

template <class protocol_policy> void irc_net<protocol_policy>::part() {
	send("PART " + conf->get_channel() + " :EOL");
}

template <class protocol_policy> void irc_net<protocol_policy>::quit() {
	send("QUIT :EOL");
}

template <class protocol_policy> void irc_net<protocol_policy>::join_channel(string channel) {
	send("JOIN " + channel);
}

#endif
