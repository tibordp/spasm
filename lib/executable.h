/*
    spasm - x86-64 assembler / JIT support library
    Copyright (C) 2014  Tibor Djurica Potpara <tibor.djurica@ojdip.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#define NOMINMAX

#include <utility>

#include "spasm.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

namespace spasm {

template<typename> class x_function;

template<typename R, typename... ArgTypes>
class x_function<R(ArgTypes...)>
{
	bool executable_;
	size_t size_;
	void * data_;
	void * entry_point_;
public:

	void* data() {
		return data_;
	}

	void set_entry(size_t offset)
	{
		entry_point_ = static_cast<char*>(data_) + offset;
	}

	void set_executable(bool executable)
	{
		if (executable_ != executable)
		{
			#ifdef _WIN32
			DWORD old_protect;
			VirtualProtect(
				data_, size_,
				executable ? PAGE_EXECUTE_READ : PAGE_READWRITE, 
				&old_protect
			);
			#else
			mprotect(data_, size_,
				PROT_READ | (executable ? PROT_EXEC : PROT_WRITE));
			#endif
			executable_ = executable;
		}
	}

	x_function(size_t size) :
		executable_(false),
		size_(size)
	{
		if (size == 0) { data_ = nullptr; return; }
		#ifdef _WIN32
		data_ = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
		#else
		data_ = mmap(NULL, size,
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		#endif
		entry_point_ = data_;
	}

	x_function(void* data, size_t size) :
		x_function(size)
	{
		memcpy(data_, data, size);
		entry_point_ = data_;
		set_executable(true);
	}

	~x_function()
	{
		#ifdef _WIN32
		VirtualFree(data_, 0, MEM_RELEASE);
		#else
		munmap(data_, size_);
		#endif
	}

	void swap(x_function& other)
	{
		using std::swap;
		swap(executable_, other.executable_);
		swap(size_, other.size_);
		swap(data_, other.data_);
		swap(entry_point_, other.entry_point_);
	}

	/*
		By copying the x_memory object we are actually making a copy. 
		While it would probably be more efficient to just keep a 
		reference count, it may be helpful to allow the users to 
		unprotect & change the machine code.
	*/

	x_function() : x_function(0) {}
	x_function(const x_function& other) : x_function()
	{
		x_function copy(other.size_);
		memcpy(copy.data_, other.data_, other.size_);
		
		// Recalculate the entry point offset
		copy.entry_point_ = static_cast<char*>(copy.data_) + 
			(static_cast<char*>(other.entry_point_) - static_cast<char*>(other.data_));

		copy.set_executable(other.executable_);

		swap(copy);
	}

	x_function(x_function&& other) : x_function() { swap(other); }
	x_function& operator=(x_function other)
	{
		swap(other);
		return *this;
	}

	/*----------------------------------------------------------------- */

	void* data() const { return data_; }
	size_t size() const { return size_; }
	bool executable() const { return executable_; } 

	template<typename... RArgTypes>
	R operator()(RArgTypes&&... args) const
	{
		return reinterpret_cast<R(*)(ArgTypes...)>(entry_point_)(
			std::forward<RArgTypes>(args)...);
	}
};

}