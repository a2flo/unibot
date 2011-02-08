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

#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

class config;
namespace logger {
	enum LOG_TYPE {
		LT_NONE,
		LT_ERROR,
		LT_DEBUG,
		LT_MSG
	};
	
	void log(const LOG_TYPE type, const char* filename, const char* str = NULL, ...);
	void set_config(config* conf);
	
	class log_class {
	public:
		log_class();
		~log_class();
		
		void print(const LOG_TYPE type, const char* filename, const char* str = NULL, ...);
		void set_config(config* conf);
		
	protected:
		unsigned long int error_counter;
		string log_filename;
		string msg_filename;
		fstream* log_file;
		fstream* msg_file;
		
		config* conf;
		
		const char* type_to_str(LOG_TYPE type) {
			switch (type) {
				case LT_NONE: return "NONE";
				case LT_MSG: return "MSG";
				case LT_DEBUG: return "DEBUG";
				case LT_ERROR: return "ERROR";
			}
			return "UNKNOWN";
		}
		
	};

}

#endif
