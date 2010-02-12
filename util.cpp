/*
 *  UniBot
 *  Copyright (C) 2009 - 2010 Florian Ziesche
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

#include "util.h"

void tokenize(vector<string>& dst, const string& src, const char delim) {	
	size_t pos = 0;
	size_t old_pos = 0;
	if(src.find(delim, pos) != string::npos) {
		while((pos = src.find(delim, old_pos)) != string::npos) {
			dst.push_back(src.substr(old_pos, pos-old_pos));
			old_pos = pos+1;
		}
		dst.push_back(src.substr(old_pos, pos-old_pos));
	}
	else dst.push_back(src);
}

string encode_url(string& url) {
	// TODO: replace '+' by %2B
	stringstream result;
	for(string::iterator citer = url.begin(); citer != url.end(); citer++) {
		switch(*citer) {
			case 'a'...'z':
			case 'A'...'Z':
			case '0'...'9':
			case '!':
			case '#':
			case '$':
			case '&'...'/':
			case ';':
			case ':':
			case '=':
			case '?':
			case '@':
			case '['...'`':
			case '{'...'~':
				result << *citer;
				break;
			default:
				result << '%' << uppercase << hex << (*citer & 0xFF);
				break;
		}
	}
	return result.str();
}

string encode_url(const char* url) {
	string url_str = string(url);
	return encode_url(url_str);
}

string trim(string str) {
	if(str.length() == 0) return "";

	size_t pos = 0;
	while(pos < str.length() && (str[pos] == ' ' || str[pos] == '\t')) {
		str.erase(pos, 1);
	}
	pos = str.length()-1;
	while(pos >= 0 && (str[pos] == ' ' || str[pos] == '\t')) {
		str.erase(pos, 1);
	}
	return str;
}

string rev(string& str) {
	reverse(str.begin(), str.end());
	return str;
}
