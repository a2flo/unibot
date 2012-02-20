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

// Windows
#if (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)) && !defined(CYGWIN)
#define WIN32_LEAN_AND_MEAN
#define __WINDOWS__
#include <windows.h>
#include <winnt.h>
#include <io.h>
#include <direct.h>

#define snprintf _snprintf
#define ssize_t SSIZE_T
#define strtof(arg1, arg2) (float)strtod(arg1, arg2)

#define __func__ __FUNCTION__

#define setenv(var_name, var_value, x) SetEnvironmentVariable(var_name, var_value)

#pragma warning(disable: 4251)

// Mac OS X
#elif __APPLE__
#include <dirent.h>

// everything else (Linux, *BSD, ...)
#else
#include <dirent.h>
#endif // windows

#ifndef __WINDOWS__
#define OS_DIR_SLASH "/"
#else
#define OS_DIR_SLASH "\\"
#endif

// general includes
#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_cpuinfo.h>
#include <SDL/SDL_main.h>
#include <SDL_net/SDL_net.h>

extern "C" {
#include <lua/lua.hpp>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#elif defined(__WINDOWS__) // windows
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_cpuinfo.h>
#include <SDL2/SDL_main.h>
#include <SDL2_net/SDL_net.h>

extern "C" {
#include <lua/lua.hpp>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#else // unix/linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_cpuinfo.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_net.h>

extern "C" {
#include <lua.hpp>
#include <lualib.h>
#include <lauxlib.h>
}
#endif // __APPLE__

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
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <typeinfo>
#include <limits>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef CYGWIN
#include <sys/wait.h>
#endif

using namespace std;

// global unibot headers
#include "type_list.h"
#include "functor.h"
#include "logger.h"
#include "util.h"

#ifndef __has_feature
#define __has_feature(x) 0
#endif

// compiler checks:
// msvc check
#if defined(_MSC_VER)
#if (_MSC_VER <= 1700)
#error "Sorry, but you need MSVC 12.0+ to compile UniBot"
#endif

// clang check
#elif defined(__clang__)
#if !__has_feature(cxx_rvalue_references) || \
	!__has_feature(cxx_auto_type) || \
	!__has_feature(cxx_variadic_templates) || \
	!__has_feature(cxx_range_for)
#error "Sorry, but you need Clang with support for 'rvalue_references', 'auto_type', 'variadic_templates' and 'range_for' to compile UniBot"
#endif

// gcc check
#elif defined(__GNUC__)
#if (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#error "Sorry, but you need GCC 4.6+ to compile UniBot"
#endif

// just fall through ...
#else
#endif
