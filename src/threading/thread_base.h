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

#ifndef __THREAD_BASE_H__
#define __THREAD_BASE_H__

#include "platform.h"
#include <thread>
#include <mutex>

class thread_base {
public:
	thread_base();
	virtual ~thread_base();
	
	enum THREAD_STATUS {
		INVALID = -1,
		INIT = 0,
		RUNNING = 1,
		FINISHED = 2
	};
	
	// this is the main run function of the thread
	virtual void run() {}
	
	void finish();
	void lock();
	bool try_lock();
	void unlock();
	
	void set_thread_status(const thread_base::THREAD_STATUS status);
	const THREAD_STATUS get_thread_status();
	bool is_running(); // shortcut for get_thread_status() == RUNNING || INIT
	void set_thread_should_finish();
	bool thread_should_finish();
	void set_thread_delay(const size_t delay);
	const size_t get_thread_delay();
	
protected:
	thread* thread_obj;
	recursive_mutex thread_lock;
	THREAD_STATUS thread_status;
	bool thread_should_finish_flag;
	size_t thread_delay;
	
	void start();
	static int _thread_run(thread_base* this_thread_obj);
};

#endif
