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

#include "config.h"
#include "floor/floor.hpp"

config::config(const string& config_file, const string& environment, const ssize_t& argc, const char** argv) {
	// default values
	config_data["verbosity"] = size_t2string((size_t)logger::LOG_TYPE::SIMPLE_MSG);
	config_data["environment"] = environment;
	
	string binary = core::strip_path(argv[0]);
	const size_t slash_pos = binary.rfind('/');
	if(slash_pos != string::npos) binary = binary.substr(slash_pos+1, binary.length()-slash_pos-1);
	
	config_data["argc"] = ssize_t2string(argc);
	config_data["arg_0"] = core::strip_path(floor::get_absolute_path()+binary);
	for(ssize_t i = 1; i < argc; i++) {
		config_data["arg_"+ssize_t2string(i)] = argv[i];
	}
	
	if(!load_config(config_file)) {
		throw runtime_error("invalid config file");
	}
}

config::~config() {
}

bool config::load_config() {
	file.open(config_file.c_str(), fstream::in);
	if(!file.is_open()) {
		file.clear();
		log_error("couldn't open config file %s!", config_file);
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
	vector<string> lines { core::tokenize(data_str.str(), '\n') };
	bool bot_block = false;
	bool config_version_found = false;
	size_t assign_pos = 0;
	for(auto line_iter = lines.begin(); line_iter != lines.end(); line_iter++) {
		if(line_iter->find(";") == 0 || line_iter->find("#") == 0 || line_iter->find("//") == 0) continue;
		else if(line_iter->find("[unibot]") == 0) bot_block = true;
		else if(line_iter->find("[") == 0 && line_iter->find("]") != string::npos) bot_block = false;
		else if(bot_block && (assign_pos = line_iter->find("=")) != string::npos) {
			string identifier = core::trim(line_iter->substr(0, assign_pos));
			string value = core::trim(line_iter->substr(assign_pos+1, line_iter->length()-assign_pos-1));
			
			// check for config version
			if(identifier == "config_version") {
				config_version_found = true;
				if(value != size_t2string(UNIBOT_CONFIG_VERSION)) {
					log_error("invalid config version %i - current version: %i!", value, UNIBOT_CONFIG_VERSION);
					return false;
				}
			}
			
			// arg_# and argc are reserved values
			const string id_4 = identifier.size() >= 4 ? identifier.substr(0, 4) : "";
			if(id_4 == "arg_" || id_4 == "argc") {
				log_error("%s is a reserved value!", identifier);
				continue;
			}
			
			// don't store the password inside the config data ...
			if(identifier != "bot_password") {
				config_data.erase(identifier); // erase before insert, this makes the "latest" config setting "active"
				config_data.insert(pair<string, string>(identifier, value));
			}
			else bot_password = value;
			
			if(identifier == "owner_names") {
				owner_names = core::tokenize(value, ',');
				
				// trim each (array) value
				for(auto owner_iter = owner_names.begin(); owner_iter != owner_names.end(); owner_iter++) {
					*owner_iter = core::trim(*owner_iter);
				}
				
				// remove empty names
				for(auto owner_iter = owner_names.rbegin(); owner_iter != owner_names.rend(); owner_iter++) {
					if(owner_iter->length() == 0) owner_names.erase(owner_names.begin() + (owner_names.rend() - owner_iter) - 1);
				}
				
				// remove duplicates
				sort(owner_names.begin(), owner_names.end());
				owner_names.resize(unique(owner_names.begin(), owner_names.end()) - owner_names.begin());
			}
		}
	}
	
	if(!config_version_found) {
		log_error("no config version specified!");
		return false;
	}
	
	return true;
}

bool config::load_config(const string& config_file) {
	this->config_file = config_file;
	return load_config();
}

string config::get_bot_name() {
	return config_data["bot_name"];
}

string config::get_bot_alt_add() {
	return config_data["bot_alt_add"];
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
	return (unsigned short int)strtoul(config_data["port"].c_str(), nullptr, 10);
}

string config::get_channel() {
	return config_data["channel"];
}

string config::get_config_entry(const string& name) {
	if(config_data.count(name) == 0) {
		log_error("unknown config entry name \"%s\"!", name);
		return "";
	}
	return config_data[name];
}

bool config::is_owner(string user) {
	for(auto owner_iter = owner_names.begin(); owner_iter != owner_names.end(); owner_iter++) {
		if(*owner_iter == user) return true;
	}
	return false;
}

logger::LOG_TYPE config::get_verbosity() const {
	return (logger::LOG_TYPE)strtoul(config_data.find("verbosity")->second.c_str(), nullptr, 10);
}
