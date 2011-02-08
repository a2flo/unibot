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

#include "event_handler.h"

event_handler_base* event_handler = NULL;
void init_event_handler() {
	if(event_handler != NULL) {
		event_handler = new event_handler_base();
	}
}
void destroy_event_handler() {
	if(event_handler != NULL) {
		delete event_handler;
		event_handler = NULL;
	}
}

event_handler_base::event_handler_base() : thread_base() {
	this->start();
}

event_handler_base::~event_handler_base() {
}

void event_handler_base::run() {
	//
}
