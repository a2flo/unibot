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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "platform.h"

#define UNIBOT_CONFIG_VERSION 2

class invalid_config_exception : exception {};

class config {
public:
	config(const char* config_file, const char* environment, const ssize_t& argc, const char** argv);
	~config();
	
	bool load_config();
	bool load_config(const char* config_file);
	
	string get_config_entry(const string& name);
	string get_bot_name();
	string get_bot_realname();
	string get_bot_password();
	vector<string> get_owner_names();
	string get_hostname();
	unsigned short int get_port();
	string get_channel();
	const size_t get_verbosity() const;
	
	bool is_owner(string user);
	
protected:
	fstream file;
	string config_file;
	
	/* available config settings:
	 * config_version
	 * bot_name
	 * bot_realname
	 * owner_names
	 * hostname
	 * port
	 * channel
	 * verbosity
	 * argc
	 * arg_#
	 * environment
	 */
	map<string, string> config_data;
	vector<string> owner_names;
	string bot_password;
	
};

#endif
