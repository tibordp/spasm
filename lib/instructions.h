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

#include "spasm.h"

namespace spasm {

/*
	For now `instruction` is simply a vector with some added functionality (pushing arbitrarily-sized
	data. 
*/
class label {
	size_t jump_offset;

};

class instruction : public std::vector<uint8_t> {
private:
	void push_prefices (const mod_rm_specifier&);
	void push_instruction(const cpu_register&, const uint8_t, const uint8_t);

public:
	template<typename T>
	void push_value(iterator whither, T what)
	{
		auto position = whither - begin();
		insert(whither, sizeof(T), 0x00);
		*reinterpret_cast<T*>(&(*this)[position]) = what;
	}

	void push_data(iterator whither, void* value, size_t size)
	{
		insert(whither, static_cast<uint8_t*>(value), static_cast<uint8_t*>(value) + size);
	}

	void push_displacement(iterator whither, disp_size size, int32_t displacement)
	{
		switch (size)
		{
		case disp_size::d8:
			push_value(whither, static_cast<int8_t>(displacement));
			break;
		case disp_size::d32:
			push_value(whither, displacement);
			break;
		default:
			break;
		}
	}

	bool direct(const cpu_register& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool indirect(const sib_specifier& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool direct_simple(const cpu_register& rm,
		const uint8_t value_r8, const uint8_t value_others, bool add_index, int offset = 0);

	bool indirect_simple(const sib_specifier& rm, size_t ptr_size,
		const uint8_t value_r8, const uint8_t value_others, int offset);

	bool direct_immediate(const cpu_register& reg, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, bool add_index, int offset = 0);

	bool indirect_immediate(const sib_specifier& rm, size_t ptr_size, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, int offset = 0);

	// ------------ mov ------------

	template<size_t size, typename T> 
	bool mov(const cpu_register& reg, T value) { \
		static_assert(sizeof(T) == size, "Invalid operand size"); \
		return direct_immediate(reg, &value, size, 0xB0, 0xB8, true); }
	template<size_t size, typename T> 
	bool mov(const sib_specifier& rm, T value) { \
		static_assert(sizeof(T) == size, "Invalid operand size"); \
		return indirect_immediate(rm, size, &value, size, 0xC6, 0xC7); }
	bool mov(const cpu_register& rm, const cpu_register& reg) { return direct(rm, reg, 0x88, 0x89); }
	bool mov(const sib_specifier& rm, const cpu_register& reg) { return indirect(rm, reg, 0x88, 0x89); }
	bool mov(const cpu_register& reg, const sib_specifier& rm) { return indirect(rm, reg, 0x8a, 0x8b); }

	// ------------ add, or, adc, sbb, and, sub, xor, cmp ------------

	#define DEFINE_COMMON(name, index) \
		template<size_t size, typename T> \
				bool name(const cpu_register& reg, T value) { \
			static_assert(sizeof(T) == size, "Invalid operand size"); \
			return direct_immediate(reg, &value, size, 0x80, 0x81, false, index); } \
		template<size_t size, typename T> \
		bool name(const sib_specifier& rm, T value) { \
			static_assert(sizeof(T) == size, "Invalid operand size"); \
			return indirect_immediate(rm, size, &value, size, 0x80, 0x81, index); }  \
		bool name(const cpu_register& rm, const cpu_register& reg) {  return direct(rm, reg, 0x00 + 8 * index, 0x01 + 8 * index); } \
		bool name(const sib_specifier& rm, const cpu_register& reg) { return indirect(rm, reg, 0x00 + 8 * index, 0x01 + 8 * index); } \
		bool name(const cpu_register& reg, const sib_specifier& rm) { return indirect(rm, reg, 0x02 + 8 * index, 0x03 + 8 * index); } 

	DEFINE_COMMON(add, 0)
	DEFINE_COMMON(or_,  1) /* or_, and_, xor_ undercoreless ones are reserved*/
	DEFINE_COMMON(adc, 2)
	DEFINE_COMMON(sbb, 3)
	DEFINE_COMMON(and_, 4)
	DEFINE_COMMON(sub, 5)
	DEFINE_COMMON(xor_, 6)
	DEFINE_COMMON(cmp, 7)

	// ------------ lea ------------

	bool lea(const sib_specifier& rm, const cpu_register& reg) { 
		if (reg.type == register_type::r8) return false;
		return indirect(rm, reg, 0x8D, 0x8D); }
	
	// ------------ sal, sar ------------
	#undef DEFINE_COMMON
	#define DEFINE_COMMON(name, index) \
		bool name(const cpu_register& reg) { return direct_simple(reg, 0xd0, 0xd1, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm) { return indirect_simple(rm, size, 0xd0, 0xd1, index); }  \
		bool name(const cpu_register& reg, uint8_t op) { return direct_immediate(reg, &op, 1, 0xc0, 0xc1, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm, uint8_t op) { return indirect_immediate(rm, size, &op, 1, 0xc0, 0xc1, index); }  \
		bool name(const cpu_register& reg, const cpu_register& op) { \
			if (op != R::cl) return false; \
			return direct_simple(reg, 0xd2, 0xd3, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm, const cpu_register& op) { \
			if (op != R::cl) return false; \
			return indirect_simple(rm, size, 0xd2, 0xd3, index); }  

	DEFINE_COMMON(rol, 0)
	DEFINE_COMMON(ror, 1) 
	DEFINE_COMMON(rcl, 2)
	DEFINE_COMMON(rcr, 3)
	
	DEFINE_COMMON(shl, 4)
	DEFINE_COMMON(sal, 4)

	DEFINE_COMMON(shr, 5)

	// DEFINE_COMMON(shl_u, 6)
	// DEFINE_COMMON(sal_u, 6) // Seems to be invalid

	DEFINE_COMMON(sar, 7)				

	// ------------ inc, dec ------------
	
	bool inc(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 0); } 
	template<size_t size> bool inc(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 0); } 	 	

	bool dec(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 1); } 
	template<size_t size> bool dec(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 1); } 	


};


}