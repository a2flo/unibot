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

#ifndef __HTTP_NET_H__
#define __HTTP_NET_H__

#include "platform.h"
#include "net.h"
#include "net_protocol.h"
#include "net_tcp.h"

// TODO: add events (for received packets and failed requests?)
template <class protocol_policy> class http_net : public net<protocol_policy> {
public:
	http_net(config* conf);
	virtual ~http_net() {} 

	void open_url(const char* url, const size_t timeout = 30);
	virtual void run();
	
protected:
	size_t request_timeout;
	string server_name;
	string server_url;
	string page_data;
	bool header_read;
	deque<string>::iterator header_end;
	
	size_t start_time;
	
	enum HTTP_STATUS_CODE {
		SC_NONE = 0,
		SC_200 = 200,
		SC_404 = 404
	};
	HTTP_STATUS_CODE status_code;
	
	enum PACKET_TYPE {
		NORMAL,
		CHUNKED
	};
	PACKET_TYPE packet_type;
	size_t header_length;
	size_t content_length;
	
	void check_header();
	void send_http_request(const char* url, const char* host);
	
	// seems like the compiler needs to know about these when using template inheritance
	using net<protocol_policy>::server_ip;
	using net<protocol_policy>::local_ip;
	using net<protocol_policy>::received_length;
	using net<protocol_policy>::receive_store;
	using net<protocol_policy>::send_store;
};

// unibot_http_net
typedef http_net<UNIBOT_NET_PROTOCOL> unibot_http_net;


template <class protocol_policy> http_net<protocol_policy>::http_net(config* conf) :
net<protocol_policy>(conf), request_timeout(30), server_name(""), server_url("/"), page_data(""), header_read(false), header_end(),
start_time(0), status_code(http_net::SC_NONE), packet_type(http_net::NORMAL), header_length(0), content_length(0) {
	this->set_thread_delay(20); // 20ms should suffice
}

template <class protocol_policy> void http_net<protocol_policy>::open_url(const char* url, const size_t timeout) {
	request_timeout = timeout;
	start_time = SDL_GetTicks();
	server_name = "";
	server_url = "/";
	
	// extract server name and server url from url
	string url_str = url;
	size_t sn_start = 0;
	size_t sn_end = url_str.length();
	
	const size_t http_pos = url_str.find("http://");
	if(http_pos != string::npos) {
		sn_start = http_pos+7;
	}
	else {
		// if http:// wasn't found, check for other ://
		if(url_str.find("://") != string::npos) {
			// if so, another protocol is requested, exit
			logger::log(logger::LT_ERROR, "http_net.h", string("open_url(): "+url_str+": invalid protocol!").c_str());
			this->set_thread_should_finish();
			return;
		}
	}
	
	// first slash
	const size_t slash_pos = url_str.find("/", sn_start);
	if(slash_pos != string::npos) {
		sn_end = slash_pos;
		server_url = url_str.substr(sn_end, url_str.length()-sn_end);
	}
	else {
		// direct/main request, use /
		server_url = "/";
	}
	
	server_name = url_str.substr(sn_start, sn_end-sn_start);
	
	// open connection
	size_t port = 80;
	const size_t colon_pos = server_name.find(":");
	if(colon_pos != string::npos) {
		port = strtoull(server_name.substr(colon_pos+1, server_name.length()-colon_pos-1).c_str(), NULL, 10);
		server_name = server_name.substr(0, colon_pos);
	}
	if(!this->connect_to_server(server_name.c_str(), port)) {
		logger::log(logger::LT_ERROR, "http_net.h", string("open_url(): couldn't connect to server "+server_name+"!").c_str());
		this->set_thread_should_finish();
		return;
	}
	
	// finally, send the request
	send_http_request(server_url.c_str(), server_name.c_str());
}

template <class protocol_policy> void http_net<protocol_policy>::send_http_request(const char* url, const char* host) {
	stringstream packet;
	packet << "GET " << url << " HTTP/1.1" << endl;
	packet << "Accept-Charset: UTF-8" << endl;
	//packet << "Connection: close" << endl;
	packet << "User-Agent: UniBot" << endl;
	packet << "Host: " << host << endl;
	packet << endl;
	
	this->send_packet(packet.str().c_str(), packet.str().length());
}

template <class protocol_policy> void http_net<protocol_policy>::run() {
	net<protocol_policy>::run();
	
	if(this->is_received_data()) {
		// first, try to get the header
		if(!header_read) {
			header_length = 0;
			for(deque<string>::iterator line = receive_store.begin(); line != receive_store.end(); line++) {
				header_length += line->size() + 2; // +2 == CRLF
				// check for empty line
				if(line->length() == 0) {
					header_read = true;
					header_end = line;
					check_header();
					
					// remove header from receive store
					header_end++;
					receive_store.erase(receive_store.begin(), header_end);
					break;
				}
			}
		}
		// if header was found previously, try to find the message end
		else {
			bool packet_complete = false;
			if(packet_type == http_net::NORMAL && content_length == (received_length - header_length)) {
				packet_complete = true;
				for(deque<string>::iterator line = receive_store.begin(); line != receive_store.end(); line++) {
					page_data += *line + '\n';
				}
			}
			else if(packet_type == http_net::CHUNKED) {
				// note: this iterates over the receive store twice, once to check if all data was received and sizes are correct and
				// a second time to write the chunk data to page_data
				for(deque<string>::iterator line = receive_store.begin(); line != receive_store.end(); line++) {
					// get chunk length
					size_t chunk_len = strtoull(line->c_str(), NULL, 16);
					if(chunk_len == 0 && line->length() == 1) {
						if(packet_complete) break; // second run is complete, break
						packet_complete = true;
						
						// packet complete, start again, add data to page_data this time
						line = receive_store.begin();
						chunk_len = strtoull(line->c_str(), NULL, 16);
					}
					
					size_t chunk_received_len = 0;
					while(++line != receive_store.end()) {
						// append chunk data
						if(packet_complete) page_data += *line + '\n';
						chunk_received_len += line->size();
						
						// check if complete chunk was received
						if(chunk_len == chunk_received_len) break;
						chunk_received_len++; // newline
					}
					
					if(line == receive_store.end()) break;
				}
			}
			
			if(packet_complete) {
				// TODO: add http-packet-received event
				
				// we're done here, clear and finish
				this->clear_received_data();
				this->set_thread_should_finish();
			}
		}
	}
	
	if((start_time + request_timeout*1000) < SDL_GetTicks()) {
		logger::log(logger::LT_ERROR, "http_net.h", string("run(): timeout for "+server_name+server_url+" request!").c_str());
		this->set_thread_should_finish();
	}
}

template <class protocol_policy> void http_net<protocol_policy>::check_header() {
	deque<string>::iterator line = receive_store.begin();
	
	// first line contains status code
	const size_t space_1 = line->find(" ")+1;
	const size_t space_2 = line->find(" ", space_1);
	status_code = (HTTP_STATUS_CODE)strtoul(line->substr(space_1, space_2-space_1).c_str(), NULL, 10);
	if(status_code != SC_200) {
		logger::log(logger::LT_ERROR, "http_net.h", string("check_header(): "+server_name+server_url+": received status code "+
														   to_str(status_code)+"!").c_str());
		this->set_thread_should_finish();
		return;
	}
	
	// continue ...
	for(line++; line != header_end; line++) {
		string line_str = str_to_lower(*line);
		if(line_str.find("transfer-encoding:") == 0) {
			if(line_str.find("chunked") != string::npos) {
				packet_type = http_net::CHUNKED;
			}
		}
		else if(line_str.find("content-length:") == 0) {
			// ignore content length if a chunked transfer-encoding was already specified (rfc2616 4.4.3)
			if(packet_type != http_net::CHUNKED) {
				packet_type = http_net::NORMAL;
				
				const size_t cl_space = line_str.find(" ")+1;
				size_t non_digit = line_str.find_first_not_of("0123456789", cl_space);
				if(non_digit == string::npos) non_digit = line_str.length();
				content_length = strtoull(line_str.substr(cl_space, non_digit-cl_space).c_str(), NULL, 10);
			}
		}
	}
}

#endif
