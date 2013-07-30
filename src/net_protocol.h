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

#ifndef __UNIBOT_NET_PROTOCOL_H__
#define __UNIBOT_NET_PROTOCOL_H__

#include "platform.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

template <class socket_type>
struct std_protocol {
	// these declarations aren't actually needed, but for the sake of understanding the "interface"
	// or getting an overview of it, i put these here
	
public:
	bool is_valid();
	bool ready();
	
	bool open_socket(const string& address, const string& port);
	
	int receive(void* data, const unsigned int max_len);
	bool send(const char* data, const int len);
	
protected:
	bool valid { false };
	socket_type socket {};
	
};

#endif
