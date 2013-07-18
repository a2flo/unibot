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

#include "task.h"

task::task(std::function<void()> op_) :
op(op_),
thread_obj(&task::run, this, [this]() {
	while(!initialized) { this_thread::yield(); }
	op();
}) {
	thread_obj.detach();
	initialized = true;
}

task::~task() {}

void task::run(task* this_task, std::function<void()> task_op) {
	task_op();
	delete this_task;
}
