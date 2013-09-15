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

#ifndef __UNIBOT_CONFIG_HPP__
#define __UNIBOT_CONFIG_HPP__

#include "core/platform.hpp"

class config {
public:
	config(const ssize_t& argc, const char** argv);
	~config();
	
	string get_config_entry(const string& name);
	string get_bot_name();
	string get_bot_alt_add();
	string get_bot_realname();
	string get_bot_password();
	vector<string> get_owner_names();
	string get_hostname();
	unsigned short int get_port();
	string get_channel();
	logger::LOG_TYPE get_verbosity() const;
	
	bool is_owner(string user);
	
protected:
	/* available config settings:
	 * bot_name
	 * bot_alt_add
	 * bot_realname
	 * owner_names
	 * hostname
	 * port
	 * channel
	 * verbosity
	 * argc
	 * arg_#
	 * environment
	 * server_ping
	 * server_timeout
	 */
	unordered_map<string, string> config_data;
	vector<string> owner_names;
	string bot_password;
	
};

#endif
