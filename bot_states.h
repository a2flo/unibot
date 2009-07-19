/*
 *  UniBot
 *  Copyright (C) 2009 Florian Ziesche
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

class bot_states {
public:
	bot_states();
	~bot_states();
	
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
	void add_user(string name, string real_name, string host);
	void update_user(string name, string real_name, string host);
	void delete_user(string name);
	void update_user(string from, string to);
	void delete_all_users();
	map<string, pair<string, string> >* get_users();
	pair<string, string> get_user(string name);
	
protected:
	bool connected;
	bool joined;
	bool parted;
	bool op;
	bool quit;
	bool kicked;
	bool identified;
	string kick_user;
	// login name: realname, host
	map<string, pair<string, string> > user_list;
	
};

#endif
