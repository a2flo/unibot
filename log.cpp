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

#include "log.h"

namespace logger {
	log_class log_obj;
	
	log_class::log_class() : error_counter(0), log_filename("log.txt"), msg_filename("msg.txt") {
		log_file = new fstream();
		log_file->open(log_filename.c_str(), fstream::out);
		
		msg_file = new fstream();
		msg_file->open(msg_filename.c_str(), fstream::out);
	}
	
	log_class::~log_class() {
		log_file->close();
		delete log_file;
		
		msg_file->close();
		delete msg_file;
	}
	
	void log_class::print(const LOG_TYPE type, const char* filename, const char* str, ...) {
		// TODO: write a variadic template function to replace vsnprintf and replace "char* ostr" with a stringstream -> variable msg size
		
		// log channel messages separately
		if(type == LT_MSG) {
			if(msg_file->is_open()) {
				msg_file->close();
				msg_file->clear();
			}
			
			msg_file->open(msg_filename.c_str(), fstream::app | fstream::out);
			
			*msg_file << str << endl;
			
			msg_file->close();
			msg_file->clear();
		}
		// standard log
		else {
			// this will close the file and reopen it in append mode
			if(log_file->is_open()) {
				log_file->close();
				log_file->clear();
			}
			log_file->open(log_filename.c_str(), fstream::app | fstream::out);
			
			// TODO: write this!
			cout << type_to_str(type) << " (" << filename << "): " << str << endl;
			*log_file << type_to_str(type) << " (" << filename << "): " << str << endl;
			
			log_file->close();
			log_file->clear();
		}
	}
	
	void log(const LOG_TYPE type, const char* filename, const char* str, ...) {
		log_obj.print(type, filename, str);
	}
}
