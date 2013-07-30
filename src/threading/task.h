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

#ifndef __UNIBOT_TASK_H__
#define __UNIBOT_TASK_H__

#include "platform.h"
#include <thread>

class task {
public:
	task(std::function<void()> op);
	
	static void spawn(std::function<void()> op) {
		new task(op);
	}
	
protected:
	const std::function<void()> op;
	thread thread_obj;
	atomic<bool> initialized { false };
	
	static void run(task* this_task, std::function<void()> task_op);
	
	// prevent destruction from the outside, since we'll self-destruct
	~task();
	task(const task& tsk) = delete;
	task& operator=(const task& tsk) = delete;
	
};

#endif