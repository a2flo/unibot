/*
 *  UniBot
 *  Copyright (C) 2009 - 2014 Florian Ziesche
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

#ifndef __UNIBOT_EVENT_HANDLER_HPP__
#define __UNIBOT_EVENT_HANDLER_HPP__

#include <floor/core/platform.hpp>
#include <floor/threading/thread_base.hpp>

void init_event_handler();
void destroy_event_handler();

class event_handler_base : public thread_base {
public:
	event_handler_base();
	virtual ~event_handler_base();
	
	virtual void run();
	
	enum class EVENT_TYPE {
		// time base events
		EVT_MINUTELY,
		EVT_HOURLY,
		EVT_DAILY,
		EVT_WEEKLY,
		// irc events
		EVT_BOT_JOINED,
		EVT_USER_JOINED,
		EVT_USER_PARTED,
		EVT_PRIVMSG, // TODO: divide into channel/privqry?
		EVT_MODE_CHANGE,
		EVT_KICK,
		EVT_NICK_CHANGE,
		EVT_TOPIC_CHANGE,
		// internal events
		EVT___,
		// http events
		EVT_HTTP_REQUEST_DONE,
		EVT_HTTP_REQUEST_FAILED,
		// misc events
		// --
	};
	
	enum class EVENT_DURATION {
		ED_DESTROY,	// destroys the event after it's triggered
		ED_KEEP		// always keeps the event
	};
	
	// TODO: implement functor correctly
	class event_callback {
		//EVENT_TYPE type;
		//EVENT_DURATION duration;
		
		event_callback() {}
		~event_callback() {}
		
		void operator()() {
		}
	};
	
	// TODO: !! (event_callback or extra functor?)
	void add_event(const EVENT_TYPE event_type, const EVENT_DURATION event_duration, event_callback* cb);
	
protected:
	multimap<EVENT_TYPE, event_callback*> events;
	
};

extern event_handler_base* event_handler;

#endif
