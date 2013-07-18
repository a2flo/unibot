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

#ifndef __UNIBOT_TYPE_LIST_H__
#define __UNIBOT_TYPE_LIST_H__

using namespace std;

class nullptr_type {};
struct empty_type {};

// define typelists
template <typename... Args> struct type_list; // necessary for the gnu crap compiler (might be removed at a later point ...)
template <typename T, typename... Args> struct type_list<T, Args...> {
	typedef T head;
	typedef type_list<Args...> tail;
};
template <typename T> struct type_list<T> {
	typedef T head;
	typedef nullptr_type tail;
};
template <> struct type_list<> {
	typedef nullptr_type head;
	typedef nullptr_type tail;
};

// typelist namespace and functions
namespace tl {
	// typelist length definitions
	template <class tlist> struct length;
	template <> struct length<nullptr_type> {
		enum { value = 0 };
	};
	
	template <class T> struct length<type_list<T>> {
		enum { value = 1 };
	};
	
	template <class T, class... U> struct length<type_list<T, U...>> {
		enum { value = 1 + length<type_list<U...>>::value };
	};
	
	// typelist indexed access (strict)
	template <class tlist, size_t index> struct type_at;
	template <class head> struct type_at<type_list<head>, 0> {
		typedef head result;
	};
	template <class head, class... tail> struct type_at<type_list<head, tail...>, 0> {
		typedef head result;
	};
	template <class head, class... tail, size_t i> struct type_at<type_list<head, tail...>, i> {
		typedef typename type_at<type_list<tail...>, i - 1>::result result;
	};
	
	// typelist indexed access (non-strict)
	template <class tlist, size_t index, typename default_type = nullptr_type> struct type_at_non_strict {
		typedef default_type result;
	};
	
	template <class head, typename default_type> struct type_at_non_strict<type_list<head>, 0, default_type> {
		typedef head result;
	};
	
	template <class head, class... tail, typename default_type> struct type_at_non_strict<type_list<head, tail...>, 0, default_type> {
		typedef head result;
	};
	
	template <class head, class... tail, size_t i, typename default_type> struct type_at_non_strict<type_list<head, tail...>, i, default_type> {
		typedef typename type_at_non_strict<type_list<tail...>, i - 1, default_type>::result result;
	};
	
	// TODO: migrate these to c++0x/variadic type lists when needed
	/*// typelist searching
	template <class tlist, class T> struct index_of;
	template <class T> struct index_of<nullptr_type, T> {
		enum { value = -1 };
	};
	
	template <class T, class tail> struct index_of<type_list<T, tail>, T> {
		enum { value = 0 };
	};
	
	template <class head, class tail, class T> struct index_of<type_list<head, tail>, T> {
	private:
		enum { temp = index_of<tail, T>::value };
	public:
		enum { value = (temp == -1 ? -1 : 1 + temp) };
	};
	
	// typelist appending
	template <class tlist, class T> struct append;
	template <> struct append<nullptr_type, nullptr_type> {
		typedef nullptr_type result;
	};
	
	template <class T> struct append<nullptr_type, T> {
		typedef type_list<T> result;
	};
	
	template <class head, class tail> struct append<nullptr_type, type_list<head, tail>> {
		typedef type_list<head, tail> result;
	};
	
	template <class head, class tail, class T> struct append<type_list<head, tail>, T> {
		typedef type_list<head, typename append<tail, T>::result> result;
	};
	
	// typelist erasing
	template <class tlist, class T> struct erase;
	template <class T> struct erase<nullptr_type, T> {
		typedef nullptr_type result;
	};
	
	template <class T, class tail> struct erase<type_list<T, tail>, T> {
		typedef tail result;
	};
	
	template <class head, class tail, class T> struct erase<type_list<head, tail>, T> {
		typedef type_list<head, typename erase<tail, T>::result> result;
	};
	
	// typelist erase all
	template <class tlist, class T> struct erase_all;
	
	template <class T> struct erase_all<nullptr_type, T> {
		typedef nullptr_type result;
	};
	
	template <class T, class tail> struct erase_all<type_list<T, tail>, T> {
		typedef typename erase_all<tail, T>::result result;
	};
	
	template <class head, class tail, class T> struct erase_all<type_list<head, tail>, T> {
		typedef type_list<head, typename erase_all<tail, T>::result> result;
	};
	
	// typelist unique/no-duplicates
	template <class tlist> struct unique;
	
	template <> struct unique<nullptr_type> {
		typedef nullptr_type result;
	};
	
	template <class head, class tail> struct unique<type_list<head, tail>> {
	private:
		typedef typename unique<tail>::result l1;
		typedef typename erase<l1, head>::result l2;
	
	public:
		typedef type_list<head, l2> result;
	};
	
	// typelist replace
	template <class tlist, class T, class U> struct replace;
	
	template <class T, class U> struct replace<nullptr_type, T, U> {
		typedef nullptr_type result;
	};
	
	template <class T, class tail, class U> struct replace<type_list<T, tail>, T, U> {
		typedef type_list<U, tail> result;
	};
	
	template <class head, class tail, class T, class U> struct replace<type_list<head, tail>, T, U> {
		typedef type_list<head, typename replace<tail, T, U>::result> result;
	};
	
	// typelist replace all
	template <class tlist, class T, class U> struct replace_all;
	
	template <class T, class U> struct replace_all<nullptr_type, T, U> {
		typedef nullptr_type result;
	};
	
	template <class T, class tail, class U> struct replace_all<type_list<T, tail>, T, U> {
		typedef typename replace_all<tail, T, U>::result result;
	};
	
	template <class head, class tail, class T, class U> struct replace_all<type_list<head, tail>, T, U> {
		typedef type_list<head, typename replace_all<tail, T, U>::result> result;
	};*/
}

#endif
