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

#ifndef __UNIBOT_UTIL_H__
#define __UNIBOT_UTIL_H__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
#include <unistd.h>
#else
#include <windows.h>
#include <winnt.h>
#include <io.h>
#include <direct.h>
#undef getcwd
#define getcwd _getcwd

#ifndef __WINDOWS__
#define __WINDOWS__
#endif

#endif
using namespace std;

template<int i> struct int2type {
	enum { value = i };
};

template<typename T> struct type2type {
	typedef T orig_type;
};

template<typename T> string to_str(const T& var) {
	stringstream buffer;
	buffer.precision(7);
	buffer.setf(ios::fixed, ios::floatfield);
	buffer << var;
	return buffer.str();
}

extern string find_and_replace(const string& str, const string& find, const string& repl);
extern string trim(const string& str);
extern void tokenize(vector<string>& dst, const string& src, const char delim);
extern string encode_url(string& url);
extern string encode_url(const char* url);

extern string str_to_lower(const string& str);
extern string str_to_upper(const string& str);

extern void set_call_path(const char* path);
extern const string& get_absolute_path();
extern string clean_path(string path);

#endif
