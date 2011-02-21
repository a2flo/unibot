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

#include "logger.h"
#include "config.h"

#define UNIBOT_LOG_FILENAME "log.txt"
#define UNIBOT_MSG_FILENAME "msg.txt"

fstream logger::log_file(UNIBOT_LOG_FILENAME, fstream::out);
fstream logger::msg_file(UNIBOT_MSG_FILENAME, fstream::out);
atomic_t logger::err_counter;
SDL_SpinLock logger::slock;
const config* logger::conf = NULL;

void logger::init() {
	logger::err_counter.value = 0;
	if(!log_file.is_open()) {
		cout << "LOG ERROR: couldn't open log file!" << endl;
	}
	if(!msg_file.is_open()) {
		cout << "LOG ERROR: couldn't open msg-log file!" << endl;
	}
}

void logger::destroy() {
	log_file.close();
	msg_file.close();
}

bool logger::prepare_log(stringstream& buffer, const LOG_TYPE& type, const char* file, const char* func) {
	// check verbosity level and leave or continue accordingly
	if(conf != NULL && conf->get_verbosity() < (size_t)type) {
		return false;
	}
	
	if(type != logger::LT_NONE && type != logger::LT_MSG) {
		buffer << logger::type_to_str(type);
		if(type == logger::LT_ERROR) buffer << " #" << AtomicFetchThenIncrement(&err_counter);
		buffer << ": ";
		/* prettify file string (aka strip path) */
		string file_str = file;
		size_t slash_pos = string::npos;
		if((slash_pos = file_str.rfind("/")) == string::npos) slash_pos = file_str.rfind("\\");
		file_str = (slash_pos != string::npos ? file_str.substr(slash_pos+1, file_str.size()-slash_pos-1) : file_str);
		buffer << file_str;
		buffer << ": " << func << "(): ";
	}
	
	return true;
}

void logger::_log(stringstream& buffer, const LOG_TYPE& type, const char* str) {
	// this is the final log function
	while(*str) {
		if(*str == '%' && *(++str) != '%') {
			cout << "LOG ERROR: invalid log format, missing arguments!" << endl;
		}
		buffer << *str++;
	}
	buffer << endl;
	
	// finally: output
	SDL_AtomicLock(&slock);
	
	cout << buffer.str();
	
	// log channel messages separately
	fstream& ofile = (type == LT_MSG ? msg_file : log_file);
	const char* ofilename = (type == LT_MSG ? UNIBOT_MSG_FILENAME : UNIBOT_LOG_FILENAME);
	
	// this will close the file and reopen it in append mode
	if(ofile.is_open()) {
		ofile.close();
		ofile.clear();
	}
	
	ofile.open(ofilename, fstream::app | fstream::out);
	
	ofile << buffer.str();
	ofile.flush();
	
	ofile.close();
	ofile.clear();
	
	SDL_AtomicUnlock(&slock);
}

void logger::set_config(const config* conf) {
	logger::conf = conf;
}
