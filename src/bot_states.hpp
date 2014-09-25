/*
 *  UniBot
 *  Copyright (C) 2009 - 2014 Florian Ziesche
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

#ifndef __UNIBOT_BOT_STATES_HPP__
#define __UNIBOT_BOT_STATES_HPP__

#include <floor/net/net.hpp>
#include <floor/net/irc_net.hpp>
#include <floor/core/platform.hpp>

class bot_states {
public:
	bot_states(floor_irc_net* n);
	~bot_states();
	
	struct user_info {
		string nick;
		string real_name;
		string host;
		string host_user;
		string registered { "no" };
		
		string ctcp_support { "no" }; // == CLIENTINFO, don't rely on this, most clients don't support ctcp CLIENTINFO,
									  // but react on ctcp VERSION and maybe others as well
		string client;
		
		user_info() {}
		user_info(const string& nick_, const string& real_name_, const string& host_, const string& host_user_,
				  const string& registered_, const string& ctcp_support_, const string& client_) :
		nick(nick_), real_name(real_name_), host(host_), host_user(host_user_), registered(registered_),
		ctcp_support(ctcp_support_), client(client_) {}
	};
	
	bool is_connected() const;
	void set_connected(bool connected);
	bool is_joined() const;
	void set_joined(bool joined);
	bool is_parted();
	void set_parted(bool parted);
	bool is_op() const;
	void set_op(bool op);
	bool is_quit() const;
	void set_quit(bool quit);
	bool is_kicked();
	void set_kicked(bool kicked);
	string get_kick_user() const;
	void set_kick_user(const string& kick_user);
	bool is_identified() const;
	void set_identified(bool identified);
	void add_user(const string& name);
	void update_user_info(const string& name, const string real_name = "", const string host = "",
						  const string host_user = "", const string registered = "", const string ctcp_support = "",
						  const string client = ""); // "" == don't update
	void update_user_name(const string& from, const string& to);
	void delete_user(const string& name);
	void delete_all_users();
	const unordered_map<string, user_info*>& get_users() const;
	bot_states::user_info* get_user(const string& name) const;
	bool is_user(string name) const;
	bool is_user_registered(const string& name) const;
	bool is_silenced() const;
	void set_silenced(bool silenced);
	bool is_restart() const;
	void set_restart(bool restart);
	
	bool is(const string& state_name) const;
	void set(const string& state_name, bool new_state);
	
protected:
	floor_irc_net* n;
	
	/* available bot states:
	 * connected
	 * joined
	 * parted
	 * op
	 * quit
	 * kicked
	 * identified
	 * silenced
	 * restart
	 */
	// state name, state
	unordered_map<string, bool> states;
	
	string kick_user;
	// login name, user_info
	unordered_map<string, user_info*> user_list;
	
};

#endif
