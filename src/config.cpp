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

#include "config.hpp"
#include <floor/floor/floor.hpp>

config::config(const int& argc, const char** argv) {
	// default values
	config_data["verbosity"] = to_string((size_t)logger::LOG_TYPE::SIMPLE_MSG);
#if !defined(WIN_UNIXENV)
	config_data["environment"] = "unix";
#else
	config_data["environment"] = "windows";
#endif
	
	string binary = core::strip_path(argv[0]);
	const size_t slash_pos = binary.rfind('/');
	if(slash_pos != string::npos) binary = binary.substr(slash_pos+1, binary.length()-slash_pos-1);
	
	config_data["argc"] = to_string(argc);
	config_data["arg_0"] = core::strip_path(floor::get_absolute_path()+binary);
	for(int i = 1; i < argc; i++) {
		config_data["arg_"+to_string(i)] = argv[i];
	}
	
	// get config entries from the json config doc
	const auto& config_doc = floor::get_config_doc();
	
	config_data["bot_name"] = config_doc.get<string>("unibot.name", "[unibot]");
	config_data["bot_realname"] = config_doc.get<string>("unibot.realname", "UniBot");
	config_data["bot_alt_add"] = config_doc.get<string>("unibot.alt_add", "_");
	config_data["password"] = "";
	bot_password = config_doc.get<string>("unibot.password", "");
	
	config_data["hostname"] = config_doc.get<string>("server.hostname", "irc.freenode.net");
	config_data["port"] = to_string(config_doc.get<uint64_t>("server.port", 7000));
	config_data["ssl"] = (config_doc.get<bool>("server.ssl", true) ? "true" : "false");
	config_data["channel"] = config_doc.get<string>("server.channel", "#unichannel");
	
	config_data["server_ping"] = to_string(config_doc.get<uint64_t>("timeouts.server_ping", 30000));
	config_data["server_timeout"] = to_string(config_doc.get<uint64_t>("timeouts.server_timeout", 30000));
	
	// owner names handling
	config_data["owner_names"] = config_doc.get<string>("owner.names", "[flo]");
	auto owner_names_vec = core::tokenize(config_data["owner_names"], ',');
	for(auto& owner : owner_names_vec) {
		// trim each (array) value
		owner = core::trim(owner);
	}
	
	// remove empty names
	for(auto owner_iter = owner_names_vec.begin(); owner_iter != owner_names_vec.end();) {
		if(owner_iter->length() == 0) {
			owner_iter = owner_names_vec.erase(owner_iter);
		}
		else owner_iter++;
	}
	
	// insert into configs owner_names set (remove duplicates)
	owner_names.insert(begin(owner_names_vec), end(owner_names_vec));
}

config::~config() {
}

string config::get_bot_name() const {
	return config_data.at("bot_name");
}

string config::get_bot_alt_add() const {
	return config_data.at("bot_alt_add");
}

string config::get_bot_realname() const {
	return config_data.at("bot_realname");
}

string config::get_bot_password() const {
	return bot_password;
}

const unordered_set<string>& config::get_owner_names() const {
	return owner_names;
}

string config::get_hostname() const {
	return config_data.at("hostname");
}

unsigned short int config::get_port() const {
	return (unsigned short int)strtoul(config_data.at("port").c_str(), nullptr, 10);
}

bool config::get_ssl() const {
	return (config_data.at("ssl") == "true");
}

string config::get_channel() const {
	return config_data.at("channel");
}

string config::get_config_entry(const string& name) const {
	if(config_data.count(name) == 0) {
		log_error("unknown config entry name \"%s\"!", name);
		return "";
	}
	return config_data.at(name);
}

bool config::is_owner(const string& user) const {
	return (owner_names.count(user) > 0);
}

logger::LOG_TYPE config::get_verbosity() const {
	return (logger::LOG_TYPE)strtoul(config_data.find("verbosity")->second.c_str(), nullptr, 10);
}
