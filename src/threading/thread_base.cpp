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

#include "thread_base.h"
#include <chrono>

thread_base::thread_base() : thread_obj(NULL), thread_lock(), thread_status(thread_base::INIT),
thread_should_finish_flag(false), thread_delay(1) {
	this->lock(); // lock thread, so start (or unlock) must be called before the thread starts running
	thread_obj = new thread(_thread_run, this);
}

thread_base::~thread_base() {
	finish();
}

void thread_base::start() {
	if(thread_status != thread_base::INIT) {
		// something is wrong, return (thread status must be init!)
		return;
	}
	
	thread_status = thread_base::RUNNING;
	this->unlock();
}

int thread_base::_thread_run(thread_base* this_thread_obj) {
	while(true) {
		if(this_thread_obj->try_lock()) {
			this_thread_obj->run();
			this_thread_obj->unlock();
			
			// reduce system load and make other locks possible
			this_thread::sleep_for(chrono::milliseconds(this_thread_obj->get_thread_delay()));
		}
		
		if(this_thread_obj->thread_should_finish()) {
			break;
		}
	}
	this_thread_obj->set_thread_status(thread_base::FINISHED);
	
	return 0;
}

void thread_base::finish() {
	// signal thread to finish
	set_thread_should_finish();
	
	// wait a few ms
	this_thread::sleep_for(chrono::milliseconds(50));
	
	// this will block until the thread is finished
	// TODO: since the thread can't be killed and join doesn't have a timeout, start an extra thread
	// to join so we don't block forever if the thread gets unexpectedly blocked/terminated/...?
	thread_obj->join();
	
	set_thread_status(thread_base::FINISHED);
}

void thread_base::lock() {
	try {
		thread_lock.lock();
	}
	catch(system_error& sys_err) {
		cout << "unable to lock thread: " << sys_err.code() << ": " << sys_err.what() << endl;
	}
	catch(...) {
		cout << "unable to lock thread" << endl;
	}
}

bool thread_base::try_lock() {
	try {
		return thread_lock.try_lock();
	}
	catch(system_error& sys_err) {
		cout << "unable to try-lock thread: " << sys_err.code() << ": " << sys_err.what() << endl;
	}
	catch(...) {
		cout << "unable to try-lock thread" << endl;
	}
	return false;
}

void thread_base::unlock() {
	try {
		thread_lock.unlock();
	}
	catch(system_error& sys_err) {
		cout << "unable to unlock thread: " << sys_err.code() << ": " << sys_err.what() << endl;
	}
	catch(...) {
		cout << "unable to unlock thread" << endl;
	}
}

void thread_base::set_thread_status(const thread_base::THREAD_STATUS status) {
	this->lock();
	thread_status = status;
	this->unlock();
}

const thread_base::THREAD_STATUS thread_base::get_thread_status() {
	return thread_status;
}

bool thread_base::is_running() {
	// copy before use
	const THREAD_STATUS status = thread_status;
	return (status == thread_base::RUNNING || status == thread_base::INIT);
}

void thread_base::set_thread_should_finish() {
	this->lock();
	thread_should_finish_flag = true;
	this->unlock();
}

bool thread_base::thread_should_finish() {
	return thread_should_finish_flag;
}

void thread_base::set_thread_delay(const size_t delay) {
	this->lock();
	thread_delay = delay;
	this->unlock();
}

const size_t thread_base::get_thread_delay() {
	return thread_delay;
}
