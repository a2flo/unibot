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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "platform.h"

class config {
public:
	config(const char* config_file);
	~config();
	
	void load_config();
	void load_config(const char* config_file);
	
	string get_bot_name();
	string get_bot_realname();
	string get_bot_password();
	vector<string> get_owner_names();
	string get_hostname();
	unsigned short int get_port();
	string get_channel();
	
	bool is_owner(string user);
	
protected:
	fstream file;
	string config_file;
	
	map<string, string> config_data;
	vector<string> owner_names;
	string bot_password;
	
	
};

#endif