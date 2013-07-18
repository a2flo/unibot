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

#ifndef __UNIBOT_LOGGER_H__
#define __UNIBOT_LOGGER_H__

#include "cpp_headers.h"
using namespace std;

//! unibot logging functions, use appropriately
//! note that you don't actually have to use a specific character for %_ to print the
//! correct type (the ostream operator<< is used and the %_ character is ignored - except
//! for %x and %X which will print out an integer in hex format)
#define unibot_error(...) logger::log(logger::LOG_TYPE::ERROR_MSG, __FILE__, __func__, __VA_ARGS__)
#define unibot_debug(...) logger::log(logger::LOG_TYPE::DEBUG_MSG, __FILE__, __func__, __VA_ARGS__)
#define unibot_msg(...) logger::log(logger::LOG_TYPE::SIMPLE_MSG, __FILE__, __func__, __VA_ARGS__)
#define unibot_log(...) logger::log(logger::LOG_TYPE::NONE, __FILE__, __func__, __VA_ARGS__)

class config;
class logger {
public:
	enum class LOG_TYPE : size_t {
		NONE,		//!< enum message with no prefix
		SIMPLE_MSG,	//!< enum simple message
		ERROR_MSG,	//!< enum error message
		DEBUG_MSG	//!< enum debug message
	};
	
	static void init();
	static void destroy();
	static void set_config(const config* conf);
	
	//
	static const char* type_to_str(const LOG_TYPE& type) {
		switch(type) {
			case LOG_TYPE::NONE: return "";
			case LOG_TYPE::SIMPLE_MSG: return "[ MSG ]";
			case LOG_TYPE::ERROR_MSG: return "[ERROR]";
			case LOG_TYPE::DEBUG_MSG: return "[DEBUG]";
		}
		assert(false && "invalid log type");
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
	logger(const logger& l) = delete;
	~logger() = delete;
	logger& operator=(const logger& l) = delete;
	
	//
	static bool prepare_log(stringstream& buffer, const LOG_TYPE& type, const char* file, const char* func);
	
	//! handles the log format
	//! only %x and %X are supported at the moment, in all other cases the standard ostream operator<< is used!
	template <bool is_enum_flag, typename U> struct enum_helper_type {
		typedef U type;
	};
	template <typename U> struct enum_helper_type<true, U> {
		typedef typename underlying_type<U>::type type;
	};
	template <typename T> static void handle_format(stringstream& buffer, const char& format, T value) {
		typedef typename conditional<is_enum<T>::value,
									 typename enum_helper_type<is_enum<T>::value, T>::type,
									 typename conditional<is_pointer<T>::value &&
														  !is_same<T, char*>::value &&
														  !is_same<T, const char*>::value &&
														  !is_same<T, unsigned char*>::value &&
														  !is_same<T, const unsigned char*>::value,
														  size_t,
														  T>::type>::type print_type;
		
		switch(format) {
			case 'x':
				buffer << hex << "0x" << (print_type)value << dec;
				break;
			case 'X':
				buffer << hex << uppercase << "0x" << (print_type)value << nouppercase << dec;
				break;
			default:
				buffer << (print_type)value;
				break;
		}
	}
	
	// internal logging functions
	static void _log(stringstream& buffer, const LOG_TYPE& type, const char* str); // will be called in the end (when there are no more args)
	template<typename T, typename... Args> static void _log(stringstream& buffer, const LOG_TYPE& type, const char* str, T value, Args&&... args) {
		while(*str) {
			if(*str == '%' && *(++str) != '%') {
				handle_format(buffer, *str, value);
				_log(buffer, type, ++str, std::forward<Args>(args)...);
				return;
			}
			buffer << *str++;
		}
		cout << "LOG ERROR: unused extra arguments specified in: \"" << buffer.str() << "\"!" << endl;
	}
	
};

#endif
