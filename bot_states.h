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

#ifndef __BOT_STATES_H__
#define __BOT_STATES_H__

#include "platform.h"
#include "net.h"

class bot_states {
public:
	bot_states(net<TCP_protocol>* n);
	~bot_states();
	
	struct user_info {
		string nick;
		string real_name;
		string host;
		string host_user;
		
		string ctcp_support; // == CLIENTINFO, don't rely on this, most clients don't support ctcp CLIENTINFO,
							 // but react on ctcp VERSION and maybe others as well
		string client;
		
		user_info() : nick(""), real_name(""), host(""), host_user(""), ctcp_support(""), client("") {}
		user_info(const string& nick_, const string& real_name_, const string& host_, const string& host_user_, const string& ctcp_support_, const string& client_) :
		nick(nick_), real_name(real_name_), host(host_), host_user(host_user_), ctcp_support(ctcp_support_), client(client_) {}
		~user_info() {}
	};
	
	bool is_connected();
	void set_connected(bool connected);
	bool is_joined();
	void set_joined(bool joined);
	bool is_parted();
	void set_parted(bool parted);
	bool is_op();
	void set_op(bool op);
	bool is_quit();
	void set_quit(bool quit);
	bool is_kicked();
	void set_kicked(bool kicked);
	string get_kick_user();
	void set_kick_user(string kick_user);
	bool is_identified();
	void set_identified(bool identified);
	void add_user(string name);
	void update_user_info(string name, string real_name = "", string host = "", string host_user = "", string ctcp_support = "", string client = ""); // "" == don't update
	void update_user_name(string from, string to);
	void delete_user(string name);
	void delete_all_users();
	map<string, user_info*>* get_users();
	bot_states::user_info* get_user(string name);
	bool is_user(string name);
	bool is_silenced();
	void set_silenced(bool silenced);
	
	bool is(const string& state_name);
	void set(const string& state_name, bool new_state);
	
protected:
	net<TCP_protocol>* n;
	
	/* available bot states:
	 * connected
	 * joined
	 * parted
	 * op
	 * quit
	 * kicked
	 * identified
	 * silenced
	 */
	// state name, state
	map<string, bool> states;
	
	string kick_user;
	// login name, user_info
	map<string, user_info*> user_list;
	
};

#endif
