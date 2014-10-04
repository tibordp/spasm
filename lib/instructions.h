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

static bool valid_register(const cpu_register& reg)
{
	return (reg.type == register_type::r64) 
		|| (reg.type == register_type::r8)
		|| (reg.type == register_type::r32) 
		|| (reg.type == register_type::r16) ;
}

class instruction : public std::vector<uint8_t> {
private:
	std::vector<code_label*> labels;

	void push_prefices (const mod_rm_specifier&);
	
	void push_instruction(size_t size, const uint8_t, const uint8_t);

	bool conditional_jump(int32_t offset, uint8_t opcode, bool address_override = false);
	
	bool conditional_jump(code_label& label, uint8_t opcode);
	
	bool direct_double(const cpu_register& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool indirect_double(const sib_specifier& rm, const cpu_register& reg,
		const uint8_t value_r8, const uint8_t value_others);

	bool direct_simple(const cpu_register& rm,
		const uint8_t value_r8, const uint8_t value_others, bool add_index,
		uint8_t index, bool skip_rexw = false);

	bool indirect_nolabel(const sib_specifier& rm, size_t ptr_size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	bool indirect_simple(const sib_specifier& rm, size_t ptr_size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	bool direct_immediate(const cpu_register& reg, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, bool add_index, uint8_t index, bool skip_rexw = false);

	bool indirect_immediate(const sib_specifier& rm, size_t ptr_size, void* value, size_t size,
		const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw = false);

	bool direct_double_xmm(
		const cpu_register& rm, 
		const cpu_register& reg,
		const uint8_t opcode,
		bool add_prexix,
		const uint8_t prefix
	);

	bool indirect_double_xmm(
		const sib_specifier& rm, 
		const cpu_register& reg,
		const uint8_t opcode,
		bool add_prexix,
		const uint8_t prefix
	);

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
	x_function<T> compile(const code_label& start) {
		x_function<T> function(size());
		memcpy(function.data(), &(*this)[0], size());

		uint8_t* data = static_cast<uint8_t*>(function.data());

		for (auto label : labels)
		{
			if (!label->valid)
				throw std::runtime_error("Invalid label reference.");

			for (const auto& target : label->targets)
			{
				*reinterpret_cast<int32_t*>(data + target.target) = 
					(int32_t)(label->where - target.offset);
			}
		}

		if (start.valid)
			function.set_entry(start.where);
		else
			throw std::runtime_error("Invalid label reference."); 
		
		function.set_executable(true);
		return function;
	}

	void label(code_label& label) {
		label.valid = true;
		label.where = size();
		labels.push_back(&label);
	}

	bool align(size_t alignment = 16) 
	{
		int offset = size() % alignment;
		return (offset == 0) ? true : nop(alignment - offset);
	}

	void aligned_label(code_label& label, size_t alignment = 16) {
		align(alignment);

		label.valid = true;
		label.where = size();
		labels.push_back(&label);
	}

	template<typename T>
	void push_value(iterator whither, T what)
	{
		auto position = whither - begin();
		insert(whither, sizeof(T), 0x00);
		*reinterpret_cast<T*>(&(*this)[position]) = what;
	}

	void push_data(iterator whither, void* value, size_t size);
	void push_displacement(iterator whither, disp_size size, int32_t displacement);

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

	bool mov(const cpu_register& rm, const cpu_register& reg)  { return direct_double(rm, reg, 0x88, 0x89); }
	bool mov(const sib_specifier& rm, const cpu_register& reg) { return indirect_double(rm, reg, 0x88, 0x89); }
	bool mov(const cpu_register& reg, const sib_specifier& rm) { return indirect_double(rm, reg, 0x8a, 0x8b); }

	// ------------ add, or, adc, sbb, and, sub, xor, cmp ------------

	#define SPASM_DEF_ADD_ETC(name, index) \
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
		bool name(const cpu_register& rm, const cpu_register& reg)  { return direct_double  (rm, reg, 0x00 + 8 * index, 0x01 + 8 * index); } \
		bool name(const sib_specifier& rm, const cpu_register& reg) { return indirect_double(rm, reg, 0x00 + 8 * index, 0x01 + 8 * index); } \
		bool name(const cpu_register& reg, const sib_specifier& rm) { return indirect_double(rm, reg, 0x02 + 8 * index, 0x03 + 8 * index); } 

	SPASM_DEF_ADD_ETC(add, 0)
	SPASM_DEF_ADD_ETC(or_,  1)
	SPASM_DEF_ADD_ETC(adc, 2)
	SPASM_DEF_ADD_ETC(sbb, 3)
	SPASM_DEF_ADD_ETC(and_, 4)
	SPASM_DEF_ADD_ETC(sub, 5)
	SPASM_DEF_ADD_ETC(xor_, 6)
	SPASM_DEF_ADD_ETC(cmp, 7)

	// ------------ lea ------------

	bool lea(const sib_specifier& rm, const cpu_register& reg) 
	{ 
		if (reg.type == register_type::r8) 
			return false;
		return indirect_double(rm, reg, 0x8D, 0x8D); 
	}
	
	// ------------ sal, sar, ... ------------

	#define SPASM_DEF_ROL_ETC(name, index) \
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

	SPASM_DEF_ROL_ETC(rol, 0)
	SPASM_DEF_ROL_ETC(ror, 1) 
	SPASM_DEF_ROL_ETC(rcl, 2)
	SPASM_DEF_ROL_ETC(rcr, 3)
	
	SPASM_DEF_ROL_ETC(shl, 4)
	SPASM_DEF_ROL_ETC(sal, 4) // Two mnemonics for the same instruction.

	SPASM_DEF_ROL_ETC(shr, 5)

	// SPASM_DEF_ROL_ETC(shl_u, 6) // Unused
	// SPASM_DEF_ROL_ETC(sal_u, 6) // Unused 

	SPASM_DEF_ROL_ETC(sar, 7)				

	// ------------ inc, dec ------------
	
	bool inc(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 0); } 
	template<size_t size> bool inc(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 0); } 	 	

	bool dec(const cpu_register& rm) {  return direct_simple(rm, 0xfe, 0xff, false, 1); } 
	template<size_t size> bool dec(const sib_specifier& rm) { return indirect_simple(rm, size, 0xfe, 0xff, 1); } 	

	// ------------ Conditional jumps (Jcc) ------------

	// These two are special (they don't support the 1-byte offset):
	bool jrcxz(int32_t offset) { return conditional_jump(offset, 0xe3 ); }
	bool jecxz(int32_t offset) { return conditional_jump(offset, 0xe3, true ); }

	
	#define SPASM_COND_JUMP(op_code, name) \
		bool name(int32_t offset)    { return conditional_jump(offset, op_code ); } \
		bool name(code_label& label)    { return conditional_jump(label, op_code ); } 

	// Entries in the same line are aliases for the same instruction.

	SPASM_COND_JUMP(0x70, jo   )
	SPASM_COND_JUMP(0x71, jno  )
	SPASM_COND_JUMP(0x72, jb   ) SPASM_COND_JUMP(0x72, jc   ) SPASM_COND_JUMP(0x72, jnae )
	SPASM_COND_JUMP(0x73, jae  ) SPASM_COND_JUMP(0x73, jnb  ) SPASM_COND_JUMP(0x73, jnc  )
	SPASM_COND_JUMP(0x74, je   ) SPASM_COND_JUMP(0x74, jz   )
	SPASM_COND_JUMP(0x75, jne  ) SPASM_COND_JUMP(0x75, jnz  )
	SPASM_COND_JUMP(0x76, jna  ) SPASM_COND_JUMP(0x76, jbe  )
	SPASM_COND_JUMP(0x77, ja   ) SPASM_COND_JUMP(0x77, jnbe )
	SPASM_COND_JUMP(0x78, js   )
	SPASM_COND_JUMP(0x79, jns  )
	SPASM_COND_JUMP(0x7a, jp   ) SPASM_COND_JUMP(0x7a, jpe  )
	SPASM_COND_JUMP(0x7b, jpo  ) SPASM_COND_JUMP(0x7b, jnp  )
	SPASM_COND_JUMP(0x7c, jnge ) SPASM_COND_JUMP(0x7c, jl   )
	SPASM_COND_JUMP(0x7d, jge  ) SPASM_COND_JUMP(0x7d, jnl  )
	SPASM_COND_JUMP(0x7e, jng  ) SPASM_COND_JUMP(0x7e, jle  )
	SPASM_COND_JUMP(0x7f, jg   ) SPASM_COND_JUMP(0x7f, jnle )

	// ----------- Zero-operand instructions -----------

	bool nop(size_t size = 1);
	bool ud2() { insert(end(), {0x0F, 0x0B}); return true; }
	
	bool hlt() { push_back(0xf4); return true; }
	bool ret() { push_back(0xc3); return true; }
	
	bool clc() { push_back(0xf8); return true; } 
	bool stc() { push_back(0xf9); return true; } 
	bool cli() { push_back(0xfa); return true; } 
	bool sti() { push_back(0xfb); return true; } 
	bool cld() { push_back(0xfc); return true; } 
	bool std() { push_back(0xfd); return true; } 

	bool lock() { push_back(0xf0); return true; }

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

	// ------------- jmp, call --------------

	bool jmp(int32_t offset);
	bool jmp(code_label& label);
	bool jmp(const cpu_register& rm);
	bool jmp(const sib_specifier& rm);

	bool call(int32_t offset);
	bool call(const cpu_register& rm); 
	bool call(const sib_specifier& rm);

	// ----------------- test, not, neg, mul, imul, div, idiv ----------------

	// Test instruction is the only one from the family that accepts immediate value.

	template<size_t size, typename T> bool test(const cpu_register& reg, T value)  
	{  
		static_assert(sizeof(T) == size, "Invalid operand size");  

  		if (reg.size() != size) return false;

  		// TEST has short-form encodings for xAX
  		
		if (reg == R::al)
		{ push_back(0xA8); }
		else if (reg == R::ax)
		{ push_back(0x66); push_back(0xA9); }
		else if (reg == R::eax)
		{ push_back(0xA9); }
		else 
			return direct_immediate(reg, &value, size, 0xf6, 0xf7, false, 0); 

		push_value<T>(end(), value);
		return true;
	}  
	 
	template<size_t size, typename T> bool test(const sib_specifier& rm, T value)  
	{  
		static_assert(sizeof(T) == size, "Invalid operand size");  
  		return indirect_immediate(rm, size, &value, size, 0xf6, 0xf7, 0);  
	}   

	#define SPASM_DEF_TEST_ETC(name, index) \
		bool name(const cpu_register& rm) {  return direct_simple(rm, 0xf6, 0xf7, false, index); } \
		template<size_t size> bool name(const sib_specifier& rm) { return indirect_simple(rm, size, 0xf6, 0xf7, index); } 

	// SPASM_DEF_TEST_ETC(test, 0)   // Special handling - accepts immediate
	// SPASM_DEF_TEST_ETC(test_u, 1) // Unused.
	SPASM_DEF_TEST_ETC(not_, 2)
	SPASM_DEF_TEST_ETC(neg, 3)
	SPASM_DEF_TEST_ETC(mul, 4)
	SPASM_DEF_TEST_ETC(imul, 5)
	SPASM_DEF_TEST_ETC(div, 6)
	SPASM_DEF_TEST_ETC(idiv, 7)

	// ----------------- xchg ----------------

	bool xchg(const cpu_register& rm, const cpu_register& reg);
	bool xchg(const sib_specifier& rm, const cpu_register& reg) { return indirect_double(rm, reg, 0x86, 0x87); }
	bool xchg(const cpu_register& reg, const sib_specifier& rm) { return indirect_double(rm, reg, 0x86, 0x87); }

	// ----------------- sse instructions ----------------	

	bool movaps(const cpu_register& rm, const cpu_register& reg) { return direct_double_xmm(reg, rm, 0x28, false, 0x00); }
	bool movapd(const cpu_register& rm, const cpu_register& reg) { return direct_double_xmm(reg, rm, 0x28, true, 0x66); }

	bool movaps(const sib_specifier& rm, const cpu_register& reg) { return indirect_double_xmm(rm, reg, 0x29, false, 0x00); }
	bool movapd(const sib_specifier& rm, const cpu_register& reg) { return indirect_double_xmm(rm, reg, 0x29, true, 0x66); }	

	bool movaps(const cpu_register& reg, const sib_specifier& rm) { return indirect_double_xmm(rm, reg, 0x28, false, 0x00); }
	bool movapd(const cpu_register& reg, const sib_specifier& rm) { return indirect_double_xmm(rm, reg, 0x28, true, 0x66); }		

	#define SPASM_DEF_XMM(op_code, name) \
		bool name(const cpu_register& rm, const cpu_register& reg) { return direct_double_xmm(reg, rm, op_code, false, 0x00); }\
		bool name(const cpu_register& reg, const sib_specifier& rm) { return indirect_double_xmm(rm, reg, op_code, false, 0x00); }

	#define SPASM_DEF_XMM_PREFIX(op_code, name, prefix) \
		bool name(const cpu_register& rm, const cpu_register& reg) { return direct_double_xmm(reg, rm, op_code, true, prefix); }\
		bool name(const cpu_register& reg, const sib_specifier& rm) { return indirect_double_xmm(rm, reg, op_code, true, prefix); }	

	SPASM_DEF_XMM(0x51, sqrtps  ) SPASM_DEF_XMM_PREFIX(0x51, sqrtpd,   0x66) SPASM_DEF_XMM_PREFIX(0x51, sqrtsd,    0xF2) SPASM_DEF_XMM_PREFIX(0x51, sqrtss,    0xF3)
	SPASM_DEF_XMM(0x52, rsqrtps )                                                                                        SPASM_DEF_XMM_PREFIX(0x52, rsqrtss,   0xF3)
	SPASM_DEF_XMM(0x53, rcpps   )                                                                                        SPASM_DEF_XMM_PREFIX(0x53, rcpss,     0xF3)
	SPASM_DEF_XMM(0x54, andps   ) SPASM_DEF_XMM_PREFIX(0x54, andpd,    0x66)
	SPASM_DEF_XMM(0x55, andnps  ) SPASM_DEF_XMM_PREFIX(0x55, andnpd,   0x66)
	SPASM_DEF_XMM(0x56, orps    ) SPASM_DEF_XMM_PREFIX(0x56, orpd,     0x66)
	SPASM_DEF_XMM(0x57, xorps   ) SPASM_DEF_XMM_PREFIX(0x57, xorpd,    0x66)
	SPASM_DEF_XMM(0x58, addps   ) SPASM_DEF_XMM_PREFIX(0x58, addpd,    0x66) SPASM_DEF_XMM_PREFIX(0x58, addsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x58, addss,     0xF3)
	SPASM_DEF_XMM(0x59, mulps   ) SPASM_DEF_XMM_PREFIX(0x59, mulpd,    0x66) SPASM_DEF_XMM_PREFIX(0x59, mulsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x59, mulss,     0xF3)
	SPASM_DEF_XMM(0x5a, cvtps2pd) SPASM_DEF_XMM_PREFIX(0x5a, cvtpd2ps, 0x66) SPASM_DEF_XMM_PREFIX(0x5A, cvtsd2ss,  0xF2) SPASM_DEF_XMM_PREFIX(0x5A, cvtss2sd,  0xF3)
	SPASM_DEF_XMM(0x5b, cvtdq2ps) SPASM_DEF_XMM_PREFIX(0x5b, cvtps2dq, 0x66)                                             SPASM_DEF_XMM_PREFIX(0x5B, cvttps2dq, 0xF3)
	SPASM_DEF_XMM(0x5c, subps   ) SPASM_DEF_XMM_PREFIX(0x5c, subpd,    0x66) SPASM_DEF_XMM_PREFIX(0x5C, subsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x5C, subss,     0xF3)
	SPASM_DEF_XMM(0x5d, minps   ) SPASM_DEF_XMM_PREFIX(0x5d, minpd,    0x66) SPASM_DEF_XMM_PREFIX(0x5D, minsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x5D, minss,     0xF3)
	SPASM_DEF_XMM(0x5e, divps   ) SPASM_DEF_XMM_PREFIX(0x5e, divpd,    0x66) SPASM_DEF_XMM_PREFIX(0x5E, divsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x5E, divss,     0xF3)
	SPASM_DEF_XMM(0x5f, maxps   ) SPASM_DEF_XMM_PREFIX(0x5f, maxpd,    0x66) SPASM_DEF_XMM_PREFIX(0x5F, maxsd,     0xF2) SPASM_DEF_XMM_PREFIX(0x5F, maxss,     0xF3)

	SPASM_DEF_XMM_PREFIX(0x60, punpcklbw,  0x66)
	SPASM_DEF_XMM_PREFIX(0x61, punpcklwd,  0x66)
	SPASM_DEF_XMM_PREFIX(0x62, punpckldq,  0x66)
	SPASM_DEF_XMM_PREFIX(0x63, packsswb,   0x66)
	SPASM_DEF_XMM_PREFIX(0x64, pcmpgtb,    0x66)
	SPASM_DEF_XMM_PREFIX(0x65, pcmpgtw,    0x66)
	SPASM_DEF_XMM_PREFIX(0x66, pcmpgtd,    0x66)
	SPASM_DEF_XMM_PREFIX(0x67, packuswb,   0x66)
	SPASM_DEF_XMM_PREFIX(0x68, punpckhbw,  0x66)
	SPASM_DEF_XMM_PREFIX(0x69, punpckhwd,  0x66)
	SPASM_DEF_XMM_PREFIX(0x6a, punpckhdq,  0x66)
	SPASM_DEF_XMM_PREFIX(0x6b, packssdw,   0x66)
    SPASM_DEF_XMM_PREFIX(0x6c, punpcklqdq, 0x66)
    SPASM_DEF_XMM_PREFIX(0x6d, punpckhqdq, 0x66)
    
	SPASM_DEF_XMM_PREFIX(0x7C, haddpd,    0x66)
	SPASM_DEF_XMM_PREFIX(0x7C, haddps,    0xF2)
	SPASM_DEF_XMM_PREFIX(0x7D, hsubpd,    0x66)
	SPASM_DEF_XMM_PREFIX(0x7D, hsubps,    0xF2)

	SPASM_DEF_XMM_PREFIX(0xD0, addsubpd,  0x66) SPASM_DEF_XMM_PREFIX(0xD0, addsubps, 0xF2)
	SPASM_DEF_XMM_PREFIX(0xD1, psrlw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xD2, psrld,     0x66)
	SPASM_DEF_XMM_PREFIX(0xD3, psrlq,     0x66)
	SPASM_DEF_XMM_PREFIX(0xD4, paddq,     0x66)
	SPASM_DEF_XMM_PREFIX(0xD5, pmullw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xD8, psubusb,   0x66)
	SPASM_DEF_XMM_PREFIX(0xD9, psubusw,   0x66)
	SPASM_DEF_XMM_PREFIX(0xDA, pminub,    0x66)
	SPASM_DEF_XMM_PREFIX(0xDB, pand,      0x66)
	SPASM_DEF_XMM_PREFIX(0xDC, paddusb,   0x66)
	SPASM_DEF_XMM_PREFIX(0xDD, paddusw,   0x66)
	SPASM_DEF_XMM_PREFIX(0xDE, pmaxub,    0x66)
	SPASM_DEF_XMM_PREFIX(0xDF, pandn,     0x66)
	SPASM_DEF_XMM_PREFIX(0xE0, pavgb,     0x66)
	SPASM_DEF_XMM_PREFIX(0xE1, psraw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xE2, psrad,     0x66)
	SPASM_DEF_XMM_PREFIX(0xE3, pavgw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xE4, pmulhuw,   0x66)
	SPASM_DEF_XMM_PREFIX(0xE5, pmulhw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xE6, cvttpd2dq, 0x66) SPASM_DEF_XMM_PREFIX(0xE6, cvtpd2dq, 0xF2) SPASM_DEF_XMM_PREFIX(0xE6, cvtdq2pd, 0xF3)
	SPASM_DEF_XMM_PREFIX(0xE8, psubsb,    0x66)
	SPASM_DEF_XMM_PREFIX(0xE9, psubsw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xEA, pminsw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xEB, por,       0x66)
	SPASM_DEF_XMM_PREFIX(0xEC, paddsb,    0x66)
	SPASM_DEF_XMM_PREFIX(0xED, paddsw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xEE, pmaxsw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xEF, pxor,      0x66)
	SPASM_DEF_XMM_PREFIX(0xF1, psllw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xF2, pslld,     0x66)
	SPASM_DEF_XMM_PREFIX(0xF3, psllq,     0x66)
	SPASM_DEF_XMM_PREFIX(0xF4, pmuludq,   0x66)
	SPASM_DEF_XMM_PREFIX(0xF5, pmaddwd,   0x66)
	SPASM_DEF_XMM_PREFIX(0xF6, psadbw,    0x66)
	SPASM_DEF_XMM_PREFIX(0xF8, psubb,     0x66)
	SPASM_DEF_XMM_PREFIX(0xF9, psubw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xFA, psubd,     0x66)
	SPASM_DEF_XMM_PREFIX(0xFB, psubq,     0x66)
	SPASM_DEF_XMM_PREFIX(0xFC, paddb,     0x66)
	SPASM_DEF_XMM_PREFIX(0xFD, paddw,     0x66)
	SPASM_DEF_XMM_PREFIX(0xFE, paddd,     0x66)

};

}