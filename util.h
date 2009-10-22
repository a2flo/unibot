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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

template<typename T> string to_str(const T& var) {
	stringstream buffer;
	buffer.precision(7);
	buffer.setf(ios::fixed, ios::floatfield);
	buffer << var;
	return buffer.str();
}

string trim(string str);
void tokenize(vector<string>& dst, const string& src, const char delim);
string encode_url(string& url);
string encode_url(const char* url);
string rev(string& str);

#endif
