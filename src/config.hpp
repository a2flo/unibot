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

#ifndef __UNIBOT_CONFIG_HPP__
#define __UNIBOT_CONFIG_HPP__

#include <floor/core/platform.hpp>

class config {
public:
	config(const int& argc, const char** argv);
	~config();
	
	string get_config_entry(const string& name) const;
	string get_bot_name() const;
	string get_bot_alt_add() const;
	string get_bot_realname() const;
	string get_bot_password() const;
	const unordered_set<string>& get_owner_names() const;
	string get_hostname() const;
	unsigned short int get_port() const;
	bool get_ssl() const;
	string get_channel() const;
	logger::LOG_TYPE get_verbosity() const;
	
	bool is_owner(const string& user) const;
	
protected:
	/* available config settings:
	 * bot_name
	 * bot_alt_add
	 * bot_realname
	 * owner_names
	 * hostname
	 * port
	 * ssl
	 * channel
	 * verbosity
	 * argc
	 * arg_#
	 * environment
	 * server_ping
	 * server_timeout
	 */
	unordered_map<string, string> config_data;
	unordered_set<string> owner_names;
	string bot_password;
	
};

#endif
