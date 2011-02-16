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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <SDL/SDL_atomic.h>
using namespace std;

//! unibot logging functions, use appropriately
//! note that you don't actually have to use a specific character for %_ to print the
//! correct type (the ostream operator<< is used and the %_ character is ignored - except
//! for %x and %X which will print out an integer in hex format)
#define unibot_error(...) logger::log(logger::LT_ERROR, __FILE__, __func__, __VA_ARGS__)
#define unibot_debug(...) logger::log(logger::LT_DEBUG, __FILE__, __func__, __VA_ARGS__)
#define unibot_msg(...) logger::log(logger::LT_MSG, __FILE__, __func__, __VA_ARGS__)
#define unibot_log(...) logger::log(logger::LT_NONE, __FILE__, __func__, __VA_ARGS__)

// check if atomics and sdl 1.3 are available
#ifndef _SDL_atomic_h_
#error "UniBot requires SDL 1.3 with support for atomics"
#endif

// this is only a temporary workaround to support older sdl 1.3 versions (TODO: remove at a later point)
#if defined(SDL_AtomicAdd) || defined(SDL_atomic_t)
#define AtomicFetchThenIncrement(a) SDL_AtomicAdd(a, 1)
#define atomic_t SDL_atomic_t
#else
#define AtomicFetchThenIncrement(a) SDL_AtomicFetchThenIncrement32(a.value)
typedef struct { volatile unsigned int value; } atomic_t;
#endif

class config;
class logger {
public:
	enum LOG_TYPE {
		LT_NONE,	//!< enum message with no prefix
		LT_MSG,		//!< enum simple message
		LT_ERROR,	//!< enum error message
		LT_DEBUG	//!< enum debug message
	};
	
	static void init();
	static void destroy();
	static void set_config(const config* conf);
	
	//
	static const char* type_to_str(const LOG_TYPE& type) {
		switch(type) {
			case LT_NONE: return ""; break;
			case LT_MSG: return "MSG"; break;
			case LT_ERROR: return "ERROR"; break;
			case LT_DEBUG: return "DEBUG"; break;
			default: break;
		}
		return "UNKNOWN";
	}
	
	// log entry function, this will create a buffer and insert the log msgs start info (type, file name, ...) and
	// finally call the internal log function (that does the actual logging)
	template<typename... Args> static void log(const LOG_TYPE type, const char* file, const char* func, const char* str, Args&&... args) {
		stringstream buffer;
		if(!prepare_log(buffer, type, file, func)) return;
		_log(buffer, type, str, std::forward<Args>(args)...);
	}

protected:
	logger(const logger& l);
	~logger();
	logger& operator=(const logger& l);
	
	static fstream log_file;
	static fstream msg_file;
	static atomic_t err_counter;
	static SDL_SpinLock slock;
	static const config* conf;

	//
	static bool prepare_log(stringstream& buffer, const LOG_TYPE& type, const char* file, const char* func);
	
	//! handles the log format
	//! only %x and %X are supported at the moment, in all other cases the standard ostream operator<< is used!
	template <typename T> static void handle_format(stringstream& buffer, const char& format, T value) {
		switch(format) {
			case 'x':
				buffer << "0x" << hex << value << dec;
				break;
			case 'X':
				buffer << "0x" << hex << uppercase << value << nouppercase << dec;
				break;
			default:
				buffer << value;
				break;
		}
	}
	
	// internal logging functions
	static void _log(stringstream& buffer, const LOG_TYPE& type, const char* str); // will be called in the end (when there are no more args)
	template<typename T, typename... Args> static void _log(stringstream& buffer, const LOG_TYPE& type, const char* str, T t, Args&&... args) {
		while(*str) {
			if(*str == '%' && *(++str) != '%') {
				handle_format(buffer, *str, t);
				_log(buffer, type, ++str, std::forward<Args>(args)...);
				return;
			}
			buffer << *str++;
		}
		cout << "LOG ERROR: unused extra arguments specified in: \"" << buffer.str() << "\"!" << endl;
	}

};

#endif
