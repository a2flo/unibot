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

thread_base::thread_base() {
	thread_status = thread_base::INIT;
	thread_should_finish_flag = false;
	thread_delay = 1;
	
	thread_lock = SDL_CreateMutex();
	this->lock(); // lock thread, so start (or unlock) must be called before the thread starts running
	thread = SDL_CreateThread(&thread_base::_thread_run, this);
}

thread_base::~thread_base() {
	finish();
	SDL_DestroyMutex(thread_lock);
}

void thread_base::start() {
	if(thread_status != thread_base::INIT) {
		// somethin is wrong, return (thread status must be init!)
		return;
	}
	
	thread_status = thread_base::RUNNING;
	this->unlock();
}

int thread_base::_thread_run(void* data) {
	thread_base* this_thread = (thread_base*)data;
	
	while(true) {
		this_thread->lock();
		this_thread->run();
		this_thread->unlock();
		
		// reduce system load and make other locks possible
		SDL_Delay((Uint32)this_thread->get_thread_delay());
		
		if(this_thread->thread_should_finish()) {
			break;
		}
	}
	this_thread->set_thread_status(thread_base::FINISHED);
	
	return 0;
}

void thread_base::finish() {
	// signal thread to finish
	set_thread_should_finish();
	
	// wait a few ms
	SDL_Delay(50);
	
	// if thread status is still running (or != finished), kill the thread
	if(get_thread_status() != thread_base::FINISHED) {
		SDL_KillThread(thread);
	}
	else {
		// if it's finished, wait (this is presumably better than kill)
		SDL_WaitThread(thread, NULL);
	}
	
	set_thread_status(thread_base::FINISHED);
}

void thread_base::lock() {
	if(SDL_mutexP(thread_lock) == -1) {
		cout << "ERROR: couldn't lock mutex!" << endl;
	}
}

void thread_base::unlock() {
	if(SDL_mutexV(thread_lock) == -1) {
		cout << "ERROR: couldn't unlock mutex!" << endl;
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
