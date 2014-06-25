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
#include "executable.h"

#include <limits>
#include <map>

namespace spasm {

/*
	For now `instruction` is simply a vector with some added functionality (pushing arbitrarily-sized
	data. 
*/

struct code_label_target {
	size_t offset;
	size_t target;
};

struct code_label {
	bool valid;
	size_t where;
	std::vector<code_label_target> targets;

	code_label() : valid(false) {}
};

class instruction : public std::vector<uint8_t> {
private:
	std::map<std::string, code_label> labels;

	void push_prefices (const mod_rm_specifier&);
	
	void push_instruction(size_t size, const uint8_t, const uint8_t);

	bool conditional_jump(int32_t offset, uint8_t opcode, bool address_override = false);
	bool conditional_jump(const std::string& label, uint8_t opcode);
	
	bool direct(const cpu_register& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool indirect(const sib_specifier& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool direct_simple(const cpu_register& rm,
		const uint8_t value_r8, const uint8_t value_others, bool add_index, uint8_t index, bool skip_rexw = false);

	bool indirect_nolabel(const sib_specifier& rm, size_t ptr_size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	bool indirect_simple(const sib_specifier& rm, size_t ptr_size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	bool direct_immediate(const cpu_register& reg, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, bool add_index, uint8_t index, bool skip_rexw = false);

	bool indirect_immediate(const sib_specifier& rm, size_t ptr_size, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	template<size_t size, typename T>
	bool handle_add_etc(const cpu_register& reg, T value, uint8_t index) 
	{ 
		static_assert(sizeof(T) == size, "Invalid operand size"); 
		
		if ((size == 1) && (reg.size() != 1))
			return direct_immediate(reg, &value, 1, 0x83, 0x83, false, index);

		if (reg.size() != size) return false;

		if (reg == R::al)
		{ push_back(0x04 + index * 0x8); }
		else if (reg == R::ax)
		{ push_back(0x66); push_back(0x05 + index * 0x8); }
		else if (reg == R::eax)
		{ push_back(0x05 + index * 0x8); }
		else 
			return direct_immediate(reg, &value, size, 0x80, 0x81, false, index); 

		push_value<T>(end(), value);
		return true;
	} 

public:
	
	template<typename T>
	x_function<T> compile() {
		x_function<T> function(size());
		memcpy(function.data(), &(*this)[0], size());

		uint8_t* data = static_cast<uint8_t*>(function.data());

		for (const auto& label : labels)
		{
			if (!label.second.valid)
				throw std::runtime_error("Invalid label reference.");

			for (const auto& target : label.second.targets)
			{
				*reinterpret_cast<int32_t*>(data + target.target) = 
					(int32_t)(label.second.where - target.offset);
			}
		}

		auto start = labels.find("start");
		
		if (start != labels.end())
			function.set_entry(start->second.where);
		
		function.set_executable(true);
		return function;
	}


	void label(const std::string& name) {
		code_label& target = labels[name];
		target.valid = true;
		target.where = size();
	}

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

	// ------------ mov ------------

	template<size_t size, typename T> bool mov(const cpu_register& reg, T value) 
	{ 
		static_assert(sizeof(T) == size, "Invalid operand size"); 
		if (reg.size() != size) return false;
		return direct_immediate(reg, &value, size, 0xB0, 0xB8, true, 0); 
	}
	template<size_t size, typename T> bool mov(const sib_specifier& rm, T value) 
	{ 
		static_assert(sizeof(T) == size, "Invalid operand size"); 
		if (size == 8) return false; // For now - sign extension!
		return indirect_immediate(rm, size, &value, size, 0xC6, 0xC7, 0); 
	}
	bool mov(const cpu_register& rm, const cpu_register& reg) { return direct(rm, reg, 0x88, 0x89); }
	bool mov(const sib_specifier& rm, const cpu_register& reg) { return indirect(rm, reg, 0x88, 0x89); }
	bool mov(const cpu_register& reg, const sib_specifier& rm) { return indirect(rm, reg, 0x8a, 0x8b); }

	// ------------ add, or, adc, sbb, and, sub, xor, cmp ------------

	#define DEFINE_COMMON(name, index) \
		template<size_t size, typename T> bool name(const cpu_register& reg, T value) \
		{ \
			return handle_add_etc<size, T>(reg, value, index); \
		} \
		\
		template<size_t size, typename T> bool name(const sib_specifier& rm, T value) \
		{ \
			static_assert(sizeof(T) == size, "Invalid operand size"); \
	  		return indirect_immediate(rm, size, &value, size, 0x80, 0x81, index); \
		}  \
		\
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
	
	// ------------ sal, sar, ... ------------

	#undef DEFINE_COMMON
	#define DEFINE_COMMON(name, index) \
		bool name(const cpu_register& reg) { return direct_simple(reg, 0xd0, 0xd1, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm) { return indirect_simple(rm, size, 0xd0, 0xd1, index); }  \
		bool name(const cpu_register& reg, uint8_t op) { return direct_immediate(reg, &op, 1, 0xc0, 0xc1, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm, uint8_t op) { return indirect_immediate(rm, size, &op, 1, 0xc0, 0xc1, index); }  \
		\
		bool name(const cpu_register& reg, const cpu_register& op) \
		{ \
			if (op != R::cl) return false; \
			return direct_simple(reg, 0xd2, 0xd3, false, index); \
		} \
		template<size_t size> bool name(const sib_specifier& rm, const cpu_register& op) \
		{ \
			if (op != R::cl) return false; \
			return indirect_simple(rm, size, 0xd2, 0xd3, index); \
		}  

	DEFINE_COMMON(rol, 0)
	DEFINE_COMMON(ror, 1) 
	DEFINE_COMMON(rcl, 2)
	DEFINE_COMMON(rcr, 3)
	
	DEFINE_COMMON(shl, 4)
	DEFINE_COMMON(sal, 4) // Two mnemonics for the same instruction.

	DEFINE_COMMON(shr, 5)

	// DEFINE_COMMON(shl_u, 6)
	// DEFINE_COMMON(sal_u, 6) // Seems to be invalid

	DEFINE_COMMON(sar, 7)				

	// ------------ inc, dec ------------
	
	bool inc(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 0); } 
	template<size_t size> bool inc(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 0); } 	 	

	bool dec(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 1); } 
	template<size_t size> bool dec(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 1); } 	

	// ------------ Conditional jumps (Jcc) ------------

	// These two are special:
	bool jrcxz(int32_t offset) { return conditional_jump(offset, 0xe3 ); }
	bool jecxz(int32_t offset) { return conditional_jump(offset, 0xe3, true ); }

	bool ja(int32_t offset)    { return conditional_jump(offset, 0x77 ); }
	bool ja(const std::string& label)    { return conditional_jump(label, 0x77 ); }
	bool jae(int32_t offset)   { return conditional_jump(offset, 0x73 ); }
	bool jae(const std::string& label)   { return conditional_jump(label, 0x73 ); }
	bool jb(int32_t offset)    { return conditional_jump(offset, 0x72 ); }
	bool jb(const std::string& label)    { return conditional_jump(label, 0x72 ); }
	bool jbe(int32_t offset)   { return conditional_jump(offset, 0x76 ); }
	bool jbe(const std::string& label)   { return conditional_jump(label, 0x76 ); }
	bool jc(int32_t offset)    { return conditional_jump(offset, 0x72 ); }
	bool jc(const std::string& label)    { return conditional_jump(label, 0x72 ); }
	bool je(int32_t offset)    { return conditional_jump(offset, 0x74 ); }
	bool je(const std::string& label)    { return conditional_jump(label, 0x74 ); }
	bool jg(int32_t offset)    { return conditional_jump(offset, 0x7f ); }
	bool jg(const std::string& label)    { return conditional_jump(label, 0x7f ); }
	bool jge(int32_t offset)   { return conditional_jump(offset, 0x7d ); }
	bool jge(const std::string& label)   { return conditional_jump(label, 0x7d ); }
	bool jl(int32_t offset)    { return conditional_jump(offset, 0x7c ); }
	bool jl(const std::string& label)    { return conditional_jump(label, 0x7c ); }
	bool jle(int32_t offset)   { return conditional_jump(offset, 0x7e ); }
	bool jle(const std::string& label)   { return conditional_jump(label, 0x7e ); }
	bool jna(int32_t offset)   { return conditional_jump(offset, 0x76 ); }
	bool jna(const std::string& label)   { return conditional_jump(label, 0x76 ); }
	bool jnae(int32_t offset)  { return conditional_jump(offset, 0x72 ); }
	bool jnae(const std::string& label)  { return conditional_jump(label, 0x72 ); }
	bool jnb(int32_t offset)   { return conditional_jump(offset, 0x73 ); }
	bool jnb(const std::string& label)   { return conditional_jump(label, 0x73 ); }
	bool jnbe(int32_t offset)  { return conditional_jump(offset, 0x77 ); }
	bool jnbe(const std::string& label)  { return conditional_jump(label, 0x77 ); }
	bool jnc(int32_t offset)   { return conditional_jump(offset, 0x73 ); }
	bool jnc(const std::string& label)   { return conditional_jump(label, 0x73 ); }
	bool jne(int32_t offset)   { return conditional_jump(offset, 0x75 ); }
	bool jne(const std::string& label)   { return conditional_jump(label, 0x75 ); }
	bool jng(int32_t offset)   { return conditional_jump(offset, 0x7e ); }
	bool jng(const std::string& label)   { return conditional_jump(label, 0x7e ); }
	bool jnge(int32_t offset)  { return conditional_jump(offset, 0x7c ); }
	bool jnge(const std::string& label)  { return conditional_jump(label, 0x7c ); }
	bool jnl(int32_t offset)   { return conditional_jump(offset, 0x7d ); }
	bool jnl(const std::string& label)   { return conditional_jump(label, 0x7d ); }
	bool jnle(int32_t offset)  { return conditional_jump(offset, 0x7f ); }
	bool jnle(const std::string& label)  { return conditional_jump(label, 0x7f ); }
	bool jno(int32_t offset)   { return conditional_jump(offset, 0x71 ); }
	bool jno(const std::string& label)   { return conditional_jump(label, 0x71 ); }
	bool jnp(int32_t offset)   { return conditional_jump(offset, 0x7b ); }
	bool jnp(const std::string& label)   { return conditional_jump(label, 0x7b ); }
	bool jns(int32_t offset)   { return conditional_jump(offset, 0x79 ); }
	bool jns(const std::string& label)   { return conditional_jump(label, 0x79 ); }
	bool jnz(int32_t offset)   { return conditional_jump(offset, 0x75 ); }
	bool jnz(const std::string& label)   { return conditional_jump(label, 0x75 ); }
	bool jo(int32_t offset)    { return conditional_jump(offset, 0x70 ); }
	bool jo(const std::string& label)    { return conditional_jump(label, 0x70 ); }
	bool jp(int32_t offset)    { return conditional_jump(offset, 0x7a ); }
	bool jp(const std::string& label)    { return conditional_jump(label, 0x7a ); }
	bool jpe(int32_t offset)   { return conditional_jump(offset, 0x7a ); }
	bool jpe(const std::string& label)   { return conditional_jump(label, 0x7a ); }
	bool jpo(int32_t offset)   { return conditional_jump(offset, 0x7b ); }
	bool jpo(const std::string& label)   { return conditional_jump(label, 0x7b ); }
	bool js(int32_t offset)    { return conditional_jump(offset, 0x78 ); }
	bool js(const std::string& label)    { return conditional_jump(label, 0x78 ); }
	bool jz(int32_t offset)    { return conditional_jump(offset, 0x74 ); }
	bool jz(const std::string& label)    { return conditional_jump(label, 0x74 ); }

	// ----------- nop -----------

	bool nop(size_t size = 1) 
	{
		// It doesn't really matter, but to keep things 8-byte aligned.
		
		while(size > 8)
		{
			nop(8);
			size -= 8;
		}

		switch (size)
		{
		case 1: insert(end(), {0x90}); break;
		case 2: insert(end(), {0x66, 0x90}); break;
		case 3: insert(end(), {0x0F, 0x1F, 0x00}); break;
		case 4: insert(end(), {0x0F, 0x1F, 0x40, 0x00}); break;
		case 5: insert(end(), {0x0F, 0x1F, 0x44, 0x00, 0x00}); break;
		case 6: insert(end(), {0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00}); break;
		case 7: insert(end(), {0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00}); break;
		case 8: insert(end(), {0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00}); break;
		//case 9: insert(end(), {0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00}); break;
		default:
			return false;
		}
		return true;
	}

	// ------------- push, pull --------------

	bool push(const cpu_register& rm) 
	{  
		if (rm.size() == 4 || rm.size() == 1) return false;
		return direct_simple(rm, 0x50, 0x50, true, 0, true);
	} 

	template<size_t size> bool push(const sib_specifier& rm) 
	{  
		if (size == 4 || size == 1) return false;
		return indirect_simple(rm, size, 0xff, 0xff, 6, true);
	} 	

	bool pop(const cpu_register& rm) 
	{  
		if (rm.size() == 4 || rm.size() == 1) return false;
		return direct_simple(rm, 0x58, 0x58, true, 0, true);
	} 

	template<size_t size> bool pop(const sib_specifier& rm) 
	{  
		if (size == 4 || size == 1) return false;
		return indirect_simple(rm, size, 0x8f, 0x8f, 0, true);
	} 		

	template<size_t size, typename T> 
	bool push(T value) 
	{ 
	 	static_assert(sizeof(T) == size, "Invalid operand size"); 
	 	switch (size)
	 	{
	 		case 1: push_back(0x6a); break;
			case 2: push_back(0x66); push_back(0x68);  break;
			case 4: push_back(0x68); break;
			default:
				return false;
	 	}
	 	push_value<T>(end(),value);
	 	return true;
	 }

	 // ------------- jmp --------------

	bool jmp(int32_t offset)
	{
		if ((offset <= std::numeric_limits<int8_t>::max()) &&
			(offset >= std::numeric_limits<int8_t>::min()))
		{
			push_back(0xeb);
			push_back(static_cast<int8_t>(offset));
		}
		else
		{
			push_back(0xe9);
			push_value<int32_t>(end(), offset);
		}
		return true;
	}

	bool jmp(const std::string& label)	
	{
		push_back(0xe9);
		push_value<int32_t>(end(), 0);

		code_label_target tgt;
		tgt.target = size() - 4;
		tgt.offset = size();

		labels[label].targets.push_back(tgt);

		return true;
	}

	bool jmp(const cpu_register& rm) 
	{  
		if (rm.type != register_type::r64 ) return false;
		return direct_simple(rm, 0xff, 0xff, false, 4, true);
	} 	

	bool jmp(const sib_specifier& rm) 
	{  
		if ((rm.type() != register_type::r64) && (rm.type() != register_type::rip)) return false;
		return indirect_simple(rm, 1, 0xff, 0xff, 4, true);
	} 	

	bool call(int32_t offset)
	{
		push_back(0xe8);
		push_value<int32_t>(end(), offset);
		return true;
	}

	bool call(const cpu_register& rm) 
	{  
		if (rm.type != register_type::r64 ) return false;
		return direct_simple(rm, 0xff, 0xff, false, 2, true);
	} 	

	bool call(const sib_specifier& rm) 
	{  
		if ((rm.type() != register_type::r64) &&  (rm.type() != register_type::rip)) return false;
		return indirect_simple(rm, 1, 0xff, 0xff, 2, true);
	} 

	bool ret() 
	{  
		push_back(0xc3);
		return true;
	} 

	// ----------------- imul ----------------

	bool test(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 0); } 
	template<size_t size> bool test(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 0); } 

	bool not_(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 2); } 
	template<size_t size> bool not_(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 2); } 

	bool neg(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 3); } 
	template<size_t size> bool neg(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 3); } 

	bool mul(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 4); } 
	template<size_t size> bool mul(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 4); } 	 	

	bool imul(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 5); } 
	template<size_t size> bool imul(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 5); } 	 	

	bool div(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 6); } 
	template<size_t size> bool div(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 6); } 

	bool idiv(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, 7); } 
	template<size_t size> bool idiv(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, 7); }
};


}