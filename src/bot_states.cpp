/*
 *  UniBot
 *  Copyright (C) 2009 - 2013 Florian Ziesche
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General License for more details.
 *
 *  You should have received a copy of the GNU General License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "bot_states.hpp"

bot_states::bot_states(floor_irc_net* n_) : n(n_) {
	states["connected"] = false;
	states["joined"] = false;
	states["parted"] = false;
	states["op"] = false;
	states["quit"] = false;
	states["kicked"] = false;
	states["identified"] = false;
	states["silenced"] = true;
	states["quit"] = false;
	states["restart"] = true;
	
	kick_user = "";
}

bot_states::~bot_states() {
}

bool bot_states::is_connected() const {
	return states.at("connected");
}

void bot_states::set_connected(bool connected) {
	states["connected"] = connected;
}

bool bot_states::is_joined() const {
	return states.at("joined");
}

void bot_states::set_joined(bool joined) {
	states["joined"] = joined;
}

bool bot_states::is_parted() {
	if(states.at("parted")) {
		states["parted"] = false;
		return true;
	}
	return false;
}

void bot_states::set_parted(bool parted) {
	states["parted"] = parted;
}

bool bot_states::is_kicked() {
	if(states.at("kicked")) {
		states["kicked"] = false;
		return true;
	}
	return false;
}

void bot_states::set_kicked(bool kicked) {
	states["kicked"] = kicked;
}

bool bot_states::is_op() const {
	return states.at("op");
}

void bot_states::set_op(bool op) {
	states["op"] = op;
}

bool bot_states::is_quit() const {
	return states.at("quit");
}

void bot_states::set_quit(bool quit) {
	states["quit"] = quit;
}

bool bot_states::is_identified() const {
	return states.at("identified");
}

void bot_states::set_identified(bool identified) {
	states["identified"] = identified;
}

void bot_states::add_user(const string& name) {
	user_list[name] = new user_info(name, "", "", "", "no", "no", "");
	
	// request user information (WHO and ctcp VERSION)
	n->send("WHO :"+name);
	n->send_private_msg("nickserv", "acc "+name);
	n->send_ctcp_request(name, "VERSION");
}

void bot_states::update_user_info(const string& name, const string real_name, const string host, const string host_user,
								  const string registered, const string ctcp_support, const string client) {
	if(user_list.count(name) == 0) return;
	
	// only update info if string isn't empty
	if(name != "") user_list[name]->nick = name;
	if(real_name != "") user_list[name]->real_name = real_name;
	if(host != "") user_list[name]->host = host;
	if(host_user != "") user_list[name]->host_user = host_user;
	if(registered != "") user_list[name]->registered = registered;
	if(ctcp_support != "") user_list[name]->ctcp_support = ctcp_support;
	if(client != "") user_list[name]->client = client;
}

void bot_states::update_user_name(const string& from, const string& to) {
	if(user_list.count(from) > 0) {
		user_list[to] = user_list[from];
		user_list.erase(user_list.find(from));
		
		// reset registered info and request again
		user_list[to]->registered = "no";
		n->send_private_msg("nickserv", "acc "+to);
	}
}

void bot_states::delete_user(const string& name) {
	if(user_list.count(name) > 0) {
		delete user_list[name];
		user_list.erase(name);
	}
}

void bot_states::delete_all_users() {
	user_list.clear();
}

const unordered_map<string, bot_states::user_info*>& bot_states::get_users() const {
	return user_list;
}

string bot_states::get_kick_user() const {
	return kick_user;
}

void bot_states::set_kick_user(const string& kick_user_) {
	this->kick_user = kick_user_;
}

bot_states::user_info* bot_states::get_user(const string& name) const {
	if(user_list.count(name) == 0) return nullptr;
	return user_list.at(name);
}

bool bot_states::is_user(string name) const {
	return (user_list.count(name) > 0);
}

bool bot_states::is_user_registered(const string& name) const {
	bot_states::user_info* user = get_user(name);
	return (user != nullptr && user->registered == "yes");
}

bool bot_states::is_silenced() const {
	return states.at("silenced");
}

void bot_states::set_silenced(bool silenced) {
	states["silenced"] = silenced;
}

bool bot_states::is(const string& state_name) const {
	if(states.count(state_name) == 0) {
		log_error("unknown state name \"%s\"!", state_name);
		return false;
	}
	return states.at(state_name);
}

void bot_states::set(const string& state_name, bool new_state) {
	if(states.count(state_name) == 0) {
		log_error("unknown state name \"%s\"!", state_name);
		return;
	}
	states[state_name] = new_state;
}

bool bot_states::is_restart() const {
	return states.at("restart");
}

void bot_states::set_restart(bool restart) {
	states["restart"] = restart;
}
