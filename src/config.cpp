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

#include "config.hpp"
#include "floor/floor.hpp"

config::config(const ssize_t& argc, const char** argv) {
	// default values
	config_data["verbosity"] = size_t2string((size_t)logger::LOG_TYPE::SIMPLE_MSG);
#if !defined(WIN_UNIXENV)
	config_data["environment"] = "unix";
#else
	config_data["environment"] = "windows";
#endif
	
	string binary = core::strip_path(argv[0]);
	const size_t slash_pos = binary.rfind('/');
	if(slash_pos != string::npos) binary = binary.substr(slash_pos+1, binary.length()-slash_pos-1);
	
	config_data["argc"] = ssize_t2string(argc);
	config_data["arg_0"] = core::strip_path(floor::get_absolute_path()+binary);
	for(ssize_t i = 1; i < argc; i++) {
		config_data["arg_"+ssize_t2string(i)] = argv[i];
	}
	
	// get config entries from the xml config doc
	const auto& config_doc = floor::get_config_doc();
	
	config_data["bot_name"] = config_doc.get<string>("config.unibot.name", "[unibot]");
	config_data["bot_realname"] = config_doc.get<string>("config.unibot.realname", "UniBot");
	config_data["bot_alt_add"] = config_doc.get<string>("config.unibot.alt_add", "_");
	config_data["password"] = "";
	bot_password = config_doc.get<string>("config.unibot.password", "");
	
	config_data["hostname"] = config_doc.get<string>("config.server.hostname", "irc.freenode.net");
	config_data["port"] = config_doc.get<string>("config.server.port", "6667");
	config_data["ssl"] = (config_doc.get<bool>("config.server.ssl", true) ? "true" : "false");
	config_data["channel"] = config_doc.get<string>("config.server.channel", "#unichannel");
	
	config_data["server_ping"] = config_doc.get<string>("config.timeouts.server_ping", "30000");
	config_data["server_timeout"] = config_doc.get<string>("config.timeouts.server_timeout", "30000");
	
	// owner names handling
	config_data["owner_names"] = config_doc.get<string>("config.owner.names", "[flo]");
	owner_names = core::tokenize(config_data["owner_names"], ',');
	for(auto& owner : owner_names) {
		// trim each (array) value
		owner = core::trim(owner);
	}
	
	// remove empty names
	for(auto owner_iter = owner_names.begin(); owner_iter != owner_names.end();) {
		if(owner_iter->length() == 0) {
			owner_iter = owner_names.erase(owner_iter);
		}
		else owner_iter++;
	}
	
	// remove duplicates
	sort(owner_names.begin(), owner_names.end());
	owner_names.resize(unique(owner_names.begin(), owner_names.end()) - owner_names.begin());
}

config::~config() {
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

bool config::get_ssl() {
	return (config_data["ssl"] == "true");
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
