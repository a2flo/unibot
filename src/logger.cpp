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

#if defined(__APPLE__) || defined(WIN_UNIXENV)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

static ofstream* log_file = new ofstream(UNIBOT_LOG_FILENAME);
static ofstream* msg_file = new ofstream(UNIBOT_MSG_FILENAME);
static atomic<unsigned int> err_counter { 0 };
static mutex output_lock;
static const config* conf { nullptr };

void logger::init() {
	if(!log_file->is_open()) {
		cout << "LOG ERROR: couldn't open log file!" << endl;
	}
	if(!msg_file->is_open()) {
		cout << "LOG ERROR: couldn't open msg-log file!" << endl;
	}
}

void logger::destroy() {
	log_file->close();
	delete log_file;
	log_file = nullptr;
	msg_file->close();
	delete msg_file;
	msg_file = nullptr;
}

bool logger::prepare_log(stringstream& buffer, const LOG_TYPE& type, const char* file, const char* func) {
	// check verbosity level and leave or continue accordingly
	if(conf != nullptr && conf->get_verbosity() < type) {
		return false;
	}
	
	if(type != logger::LOG_TYPE::NONE) {
		switch(type) {
			case LOG_TYPE::ERROR_MSG:
				buffer << "\033[31m";
				break;
			case LOG_TYPE::DEBUG_MSG:
				buffer << "\033[32m";
				break;
			case LOG_TYPE::SIMPLE_MSG:
				buffer << "\033[34m";
				break;
			case LOG_TYPE::NONE: break;
		}
		buffer << logger::type_to_str(type);
		switch(type) {
			case LOG_TYPE::ERROR_MSG:
			case LOG_TYPE::DEBUG_MSG:
			case LOG_TYPE::SIMPLE_MSG:
				buffer << "\033[m";
				break;
			case LOG_TYPE::NONE: break;
		}
		if(type == LOG_TYPE::ERROR_MSG) buffer << " #" << err_counter++ << ":";
		buffer << " ";
		// prettify file string (aka strip path)
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
			cout.flush();
		}
		buffer << *str++;
	}
	buffer << endl;
	
	// finally: output
	while(!output_lock.try_lock()) {
		this_thread::yield();
	}
	
	string bstr(buffer.str());
	cout << bstr;
	cout.flush();
	
	// log channel messages separately
	ofstream* ofile = (type == LOG_TYPE::SIMPLE_MSG ? msg_file : log_file);
	const char* ofilename = (type == LOG_TYPE::SIMPLE_MSG ? UNIBOT_MSG_FILENAME : UNIBOT_LOG_FILENAME);
	
	// this will close the file and reopen it in append mode
	if(ofile->is_open()) {
		ofile->close();
		ofile->clear();
	}
	
	ofile->open(ofilename, fstream::app | fstream::out);
	
	if(bstr[0] != 0x1B) {
		*ofile << bstr;
	}
	else {
		bstr.erase(0, 5);
		bstr.erase(7, 3);
		*ofile << bstr;
	}
	ofile->flush();
	ofile->close();
	ofile->clear();
	output_lock.unlock();
}

void logger::set_config(const config* conf_) {
	conf = conf_;
}
