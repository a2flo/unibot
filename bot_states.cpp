/*
 *  UniBot
 *  Copyright (C) 2009 Florian Ziesche
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

#include "bot_states.h"

bot_states::bot_states() {	
	connected = false;
	joined = false;
	parted = false;
	op = false;
	quit = false;
	kicked = false;
	identified = false;
	kick_user = "";
}

bot_states::~bot_states() {
}

bool bot_states::is_connected() {
	return connected;
}

void bot_states::set_connected(bool connected) {
	this->connected = connected;
}

bool bot_states::is_joined() {
	return joined;
}

void bot_states::set_joined(bool joined) {
	this->joined = joined;
}

bool bot_states::is_parted() {
	if(parted) {
		parted = false;
		return true;
	}
	return false;
}

void bot_states::set_parted(bool parted) {
	this->parted = parted;
}

bool bot_states::is_kicked() {
	if(kicked) {
		kicked = false;
		return true;
	}
	return false;
}

void bot_states::set_kicked(bool kicked) {
	this->kicked = kicked;
}

bool bot_states::is_op() {
	return op;
}

void bot_states::set_op(bool op) {
	this->op = op;
}

bool bot_states::is_quit() {
	return quit;
}

void bot_states::set_quit(bool quit) {
	this->quit = quit;
}

bool bot_states::is_identified() {
	return identified;
}

void bot_states::set_identified(bool identified) {
	this->identified = identified;
}

void bot_states::add_user(string name, string real_name, string host) {
	user_list[name] = pair<string, string>(real_name, host);
}

void bot_states::update_user(string name, string real_name, string host) {
	user_list[name] = pair<string, string>(real_name, host);
}

void bot_states::delete_user(string name) {
	if(user_list.count(name) > 0) {
		user_list.erase(name);
	}
}

void bot_states::update_user(string from, string to) {
	if(user_list.count(from) > 0) {
		user_list[to] = user_list[from];
		user_list.erase(user_list.find(from));
	}
}

void bot_states::delete_all_users() {
	user_list.clear();
}

map<string, pair<string, string> >* bot_states::get_users() {
	return &user_list;
}

string bot_states::get_kick_user() {
	return kick_user;
}

void bot_states::set_kick_user(string kick_user) {
	this->kick_user = kick_user;
}

pair<string, string> bot_states::get_user(string name) {
	if(user_list.count(name) == 0) return pair<string, string>("", "");
	return user_list[name];
}

bool bot_states::is_user(string name) {
	return (user_list.count(name) > 0);
}
