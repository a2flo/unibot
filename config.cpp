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

#include "config.h"

config::config(const char* config_file, const ssize_t& argc, const char** argv) {
	// default values
	config_data["verbosity"] = to_str(logger::LT_MSG);
	
	config_data["argc"] = to_str(argc);
	config_data["arg_0"] = clean_path(get_absolute_path()+string(argv[0]));
	for(ssize_t i = 1; i < argc; i++) {
		config_data["arg_"+to_str(i)] = argv[i];
	}
	
	logger::set_config(this);
	
	if(!load_config(config_file)) {
		throw invalid_config_exception();
	}
}

config::~config() {
}

bool config::load_config() {
	file.open(config_file.c_str(), fstream::in);
	if(!file.is_open()) {
		file.clear();
		logger::log(logger::LT_ERROR, "config.cpp", string(string("load_config(): couldn't open config file ")+config_file+string("!")).c_str());
		return false;
	}
	
	// get file data
	stringstream data_str;
	file.seekg(0, ios::end);
	unsigned int size = (unsigned int)file.tellg();
	file.seekg(0, ios::beg);
	char* data = new char[size+1];
	memset(data, 0, size+1);
	file.read(data, size);
	file.seekg(0, ios::beg);
	file.seekp(0, ios::beg);
	file.clear();
	data_str << data;
	delete [] data;
	
	// data read, close file
	file.close();
	
	// parse config file
	vector<string> lines;
	tokenize(lines, data_str.str(), '\n');
	bool bot_block = false;
	bool config_version_found = false;
	size_t assign_pos = 0;
	for(vector<string>::iterator line_iter = lines.begin(); line_iter != lines.end(); line_iter++) {
		if(line_iter->find(";") == 0 || line_iter->find("#") == 0 || line_iter->find("//") == 0) continue;
		else if(line_iter->find("[unibot]") == 0) bot_block = true;
		else if(line_iter->find("[") == 0 && line_iter->find("]") != string::npos) bot_block = false;
		else if(bot_block && (assign_pos = line_iter->find("=")) != string::npos) {
			string identifier = trim(line_iter->substr(0, assign_pos));
			string value = trim(line_iter->substr(assign_pos+1, line_iter->length()-assign_pos-1));
			
			// check for config version
			if(identifier == "config_version") {
				config_version_found = true;
				if(value != to_str(UNIBOT_CONFIG_VERSION)) {
					logger::log(logger::LT_ERROR, "config.cpp", string("load_config(): invalid config version "+value+" - current version: "+to_str(UNIBOT_CONFIG_VERSION)+"!").c_str());
					return false;
				}
			}
			
			// arg_* and argc are reserved values
			if(identifier.substr(0, 4) == "arg_" || identifier.substr(0, 4) == "argc") {
				logger::log(logger::LT_ERROR, "config.cpp", string("load_config(): "+identifier+" is a reserved value!").c_str());
				continue;
			}
			
			// don't store the password inside the config data ...
			if(identifier != "bot_password") {
				config_data.erase(identifier); // erase before insert, this makes the "latest" config setting "active"
				config_data.insert(pair<string, string>(identifier, value));
			}
			else bot_password = value;
			
			if(identifier == "owner_names") {
				tokenize(owner_names, value, ',');
				
				// trim each (array) value
				for(vector<string>::iterator owner_iter = owner_names.begin(); owner_iter != owner_names.end(); owner_iter++) {
					*owner_iter = trim(*owner_iter);
				}
				
				// remove empty names
				for(vector<string>::reverse_iterator owner_iter = owner_names.rbegin(); owner_iter != owner_names.rend(); owner_iter++) {
					if(owner_iter->length() == 0) owner_names.erase(owner_names.begin() + (owner_names.rend() - owner_iter) - 1);
				}
				
				// remove duplicates
				sort(owner_names.begin(), owner_names.end());
				owner_names.resize(unique(owner_names.begin(), owner_names.end()) - owner_names.begin());
			}
		}
	}
	
	if(!config_version_found) {
		logger::log(logger::LT_ERROR, "config.cpp", string("load_config(): no config version specified!").c_str());
		return false;
	}
	
	return true;
}

bool config::load_config(const char* config_file) {
	this->config_file = config_file;
	return load_config();
}

string config::get_bot_name() {
	return config_data["bot_name"];
}

string config::get_bot_realname() {
	return config_data["bot_realname"];
}

string config::get_bot_password() {
	return bot_password;
}

vector<string> config::get_owner_names() {
	return owner_names;
}

string config::get_hostname() {
	return config_data["hostname"];
}

unsigned short int config::get_port() {
	return (unsigned short int)strtoul(config_data["port"].c_str(), NULL, 10);
}

string config::get_channel() {
	return config_data["channel"];
}

string config::get_config_entry(const string& name) {
	if(config_data.count(name) == 0) {
		logger::log(logger::LT_ERROR, "config.cpp", string("get_config_entry(): unknown config entry name \""+name+"\"!").c_str());
		return "";
	}
	return config_data[name];
}

bool config::is_owner(string user) {
	for(vector<string>::iterator owner_iter = owner_names.begin(); owner_iter != owner_names.end(); owner_iter++) {
		if(*owner_iter == user) return true;
	}
	return false;
}

size_t config::get_verbosity() {
	return strtoul(config_data["verbosity"].c_str(), NULL, 10);
}
