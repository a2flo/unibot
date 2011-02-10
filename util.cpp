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
			case '!':
			case '#':
			case '$':
			case ';':
			case ':':
			case '=':
			case '?':
			case '@':
				result << *citer;
				break;
			default:
				if((*citer >= 'a' && *citer <= 'z') ||
				   (*citer >= 'A' && *citer <= 'Z') ||
				   (*citer >= '0' && *citer <= '9') ||
				   (*citer >= '&' && *citer <= '*') ||
				   (*citer >= ',' && *citer <= '/') ||
				   (*citer >= '[' && *citer <= '`') ||
				   (*citer >= '{' && *citer <= '~')
				   ) {
					result << *citer;
					break;
				}
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

string trim(const string& str) {
	if(str.length() == 0) return "";

	string ret = str;
	size_t pos = 0;
	while(pos < ret.length() && (ret[pos] == ' ' || ret[pos] == '\t' || ret[pos] == '\r' || ret[pos] == '\n')) {
		ret.erase(pos, 1);
	}
	
	if(ret.length() > 0) {
		pos = ret.length()-1;
		while(pos < ret.length() && (ret[pos] == ' ' || ret[pos] == '\t' || ret[pos] == '\r' || ret[pos] == '\n')) {
			ret.erase(pos, 1);
		}
	}
	return ret;
}

void set_call_path(const char* path) {
#ifndef __WINDOWS__
	const char dir_slash = '/';
#else
	const char dir_slash = '\\';
#endif
	abs_bin_path = path;
	abs_bin_path = abs_bin_path.substr(0, abs_bin_path.rfind(dir_slash)+1);
	
#ifndef __WINDOWS__
	if(abs_bin_path.size() > 0 && abs_bin_path[0] == '.') {
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

	size_t strip_pos = abs_bin_path.find("\\.\\");
	if(strip_pos != string::npos) {
		abs_bin_path.erase(strip_pos, 3);
	}
	
	bool add_bin_path = (working_dir == abs_bin_path.substr(0, abs_bin_path.length()-1)) ? false : true;
	if(!add_bin_path) abs_bin_path = working_dir + string("\\") + (add_bin_path ? abs_bin_path : "");
	else abs_bin_path += string("\\");
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

string find_and_replace(const string& str, const string& find, const string& repl) {
	// consecutive search and replace routine
	string ret = str;
	size_t pos, old_pos;
	size_t find_len = find.size();
	size_t replace_len = repl.size();
	if(find_len == 0) return ret; // replace_len might be 0 (if it's an empty string -> "")
	old_pos = 0;
	while((pos = ret.find(find, old_pos)) != string::npos) {
		ret.replace(pos, find_len, repl.c_str(), replace_len);
		old_pos = pos + replace_len;
	}
	return ret;
}

string clean_path(string path) {
	size_t pos = 0, erase_pos;

	while((pos = path.find("../", 0)) != string::npos) {
		erase_pos = path.rfind("/", pos-2);
#ifdef __WINDOWS__
		if(erase_pos == string::npos) erase_pos = path.rfind("\\", pos-2);
#endif
		if(erase_pos != string::npos) {
			path.erase(erase_pos+1, pos+2-erase_pos);
		}
	}
	
#ifdef __WINDOWS__
	// additional windows path handling
	pos = 0, erase_pos;
	while((pos = path.find("..\\", 0)) != string::npos) {
		erase_pos = path.rfind("/", pos-2);
		if(erase_pos == string::npos) erase_pos = path.rfind("\\", pos-2);
		if(erase_pos != string::npos) {
			path.erase(erase_pos+1, pos+2-erase_pos);
		}
	}
#endif
	
	
	// also remove ./
	path = find_and_replace(path, "./", "");
#ifdef __WINDOWS__
	path = find_and_replace(path, ".\\", "");
#endif
	
	return path;
}
