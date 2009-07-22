/*
 *  UniBot
 *  Copyright (C) 2009 Florian Ziesche
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

/*! @brief platform header
 *  @author flo
 *  @todo -
 *
 *  do/include platform specific stuff here
 */

// Windows
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <io.h>
#include <direct.h>

#define snprintf _snprintf
#define ssize_t SSIZE_T
#define strtof(arg1, arg2) (float)strtod(arg1, arg2)

#if (_MSC_VER < 1400) // define vsnprintf for VC++ 7.1 and minor
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#undef strlwr
#define strlwr _strlwr
#endif // vsnprintf
#endif // (_MSC_VER < 1400)

#pragma warning(disable: 4251)

// Mac OS X
#elif __APPLE__
#include <dirent.h>

// everything else (Linux, *BSD, ...)
#else
#include <dirent.h>
#endif // WIN32


// general includes
#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_cpuinfo.h>
#include <SDL_net/SDL_net.h>
#else
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_cpuinfo.h>
#include <SDL_net.h>
#endif // __APPLE__
#include <omp.h>

// some c/c++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <locale>
#include <cmath>
#include <ctime>
#include <cstring>
#include <cstdarg>

using namespace std;

// logger
#include "log.h"
//using namespace logger;

#include "util.h"
