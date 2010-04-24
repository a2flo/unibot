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

static string abs_bin_path = "";

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
	stringstream result;
	for(string::iterator citer = url.begin(); citer != url.end(); citer++) {
		switch(*citer) {
			case 'a'...'z':
			case 'A'...'Z':
			case '0'...'9':
			case '!':
			case '#':
			case '$':
			case '&'...'*':
			case ','...'/':
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
	while(str[pos] == ' ' || str[pos] == '\t') {
		str.erase(pos, 1);
	}
	return str;
}

string rev(string& str) {
	reverse(str.begin(), str.end());
	return str;
}

void set_call_path(const char* path) {
#ifndef WIN32
	const char dir_slash = '/';
#else
	const char dir_slash = '\\';
#endif
	abs_bin_path = path;
	abs_bin_path = abs_bin_path.substr(0, abs_bin_path.rfind(dir_slash)+1);
	
#ifndef WIN32
	if(abs_bin_path[0] == '.') {
		// strip leading '.' from datapath if there is one
		abs_bin_path.erase(0, 1);
		
		char working_dir[8192];
		memset(working_dir, 0, 8192);
		getcwd(working_dir, 8192);
		
		abs_bin_path = working_dir + abs_bin_path;
	}
#else
	char working_dir[8192];
	memset(working_dir, 0, 8192);
	getcwd(working_dir, 8192);
	
	bool add_bin_path = (working_dir == abs_bin_path.substr(0, abs_bin_path.length()-1)) ? false : true;
	abs_bin_path = working_dir + string("\\") + (add_bin_path ? abs_bin_path : "");
#endif
	
#ifdef __APPLE__
	// check if path contains a 'MacOS' string (indicates that the binary is called from within an OS X .app or via complete path from the shell)
	if(abs_bin_path.find("MacOS") != string::npos) {
		// if so, add "../../../" to the path, since we have to relocate the path if the binary is inside an .app
		abs_bin_path.insert(abs_bin_path.find("MacOS")+6, "../../../");
	}
#endif
}

const string& get_absolute_path() {
	return abs_bin_path;
}

string str_to_lower(const string& str) {
	string ret;
	ret.resize(str.length());
	transform(str.begin(), str.end(), ret.begin(), ptr_fun(::tolower));
	return ret;
}

string str_to_upper(const string& str) {
	string ret;
	ret.resize(str.length());
	transform(str.begin(), str.end(), ret.begin(), ptr_fun(::toupper));
	return ret;
}
