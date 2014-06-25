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

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace spasm {

enum class rex_attributte : int8_t {
	no_rex, rex, rex_no_rex,
	invalid
};

enum class register_type : int8_t {
	r8, r16, r32, r64,
	mm, xmm, sreg, eee,
	rip, invalid
};

enum class disp_size : uint8_t {
	// d16?
	d0 = 0, d8 = 1, d32 = 2
};

struct cpu_register {
	register_type type;
	rex_attributte rex;
	const char* name;
	int seq;

	uint8_t index() const { return seq & 0x7; }
	bool in_range(const cpu_register& begin, const cpu_register& end) const
	{
		return (seq >= begin.seq) && (seq <= end.seq);
	}

	bool operator==(const cpu_register& lhs) const
	{
		return seq == lhs.seq;
	}

	bool operator!=(const cpu_register& lhs) const
	{
		return seq != lhs.seq;
	}

	bool valid() const { return type != register_type::invalid; }
	
	bool always_rex() const;
	bool never_rex() const;
	bool can_be_used_in_mem() const;
	size_t size() const;
};


class cpu_registers {
	static cpu_registers& instance()
	{
		static cpu_registers instance;
		return instance;
	}

	cpu_registers() = default;
	cpu_registers(const cpu_registers& other) = delete;
	cpu_registers(cpu_registers&& other) = delete;

	std::vector<cpu_register> registers;
public:
	static cpu_register insert(cpu_register which)
	{
		instance().registers.push_back(which);
		return which;
	}

	static const std::vector<cpu_register>& all()
	{
		return instance().registers;
	}
};

/* Try to avoid using these constants in static contexts due to static initialization order fiasco */
 
namespace R {
	using RT = register_type;
	using RA = rex_attributte;
	using CRS = cpu_registers;
	using CR = cpu_register;

	extern const CR al; 	extern const CR cl; 	extern const CR dl;
	extern const CR bl; 	extern const CR ah; 	extern const CR ch;
	extern const CR dh; 	extern const CR bh; 	extern const CR spl;
	extern const CR bpl; 	extern const CR sil; 	extern const CR dil;
	extern const CR r8b; 	extern const CR r9b; 	extern const CR r10b;
	extern const CR r11b; 	extern const CR r12b; 	extern const CR r13b;
	extern const CR r14b; 	extern const CR r15b; 	extern const CR ax;
	extern const CR cx; 	extern const CR dx; 	extern const CR bx;
	extern const CR sp; 	extern const CR bp; 	extern const CR si;
	extern const CR di; 	extern const CR r8w; 	extern const CR r9w;
	extern const CR r10w; 	extern const CR r11w; 	extern const CR r12w;
	extern const CR r13w; 	extern const CR r14w; 	extern const CR r15w;
	extern const CR eax; 	extern const CR ecx; 	extern const CR edx;
	extern const CR ebx; 	extern const CR esp; 	extern const CR ebp;
	extern const CR esi; 	extern const CR edi; 	extern const CR r8d;
	extern const CR r9d; 	extern const CR r10d; 	extern const CR r11d;
	extern const CR r12d; 	extern const CR r13d; 	extern const CR r14d;
	extern const CR r15d; 	extern const CR rax; 	extern const CR rcx;
	extern const CR rdx; 	extern const CR rbx; 	extern const CR rsp;
	extern const CR rbp; 	extern const CR rsi; 	extern const CR rdi;
	extern const CR r8; 	extern const CR r9; 	extern const CR r10;
	extern const CR r11; 	extern const CR r12; 	extern const CR r13;
	extern const CR r14; 	extern const CR r15; 	extern const CR mm0;
	extern const CR mm1; 	extern const CR mm2; 	extern const CR mm3;
	extern const CR mm4; 	extern const CR mm5; 	extern const CR mm6;
	extern const CR mm7; 	extern const CR xmm0; 	extern const CR xmm1;
	extern const CR xmm2; 	extern const CR xmm3; 	extern const CR xmm4;
	extern const CR xmm5; 	extern const CR xmm6; 	extern const CR xmm7;
	extern const CR xmm8; 	extern const CR xmm9; 	extern const CR xmm10;
	extern const CR xmm11; 	extern const CR xmm12; 	extern const CR xmm13;
	extern const CR xmm14; 	extern const CR xmm15; 	extern const CR es;
	extern const CR cs; 	extern const CR ss; 	extern const CR ds;
	extern const CR fs; 	extern const CR gs; 	extern const CR cr0;
	extern const CR cr1; 	extern const CR cr2; 	extern const CR cr3;
	extern const CR cr4; 	extern const CR cr5; 	extern const CR cr6;
	extern const CR cr7; 	extern const CR cr8; 	extern const CR cr9;
	extern const CR cr10; 	extern const CR cr11; 	extern const CR cr12;
	extern const CR cr13; 	extern const CR cr14; 	extern const CR cr15;
	extern const CR dr0; 	extern const CR dr1; 	extern const CR dr2;
	extern const CR dr3; 	extern const CR dr4; 	extern const CR dr5;
	extern const CR dr6; 	extern const CR dr7; 	extern const CR dr8;
	extern const CR dr9; 	extern const CR dr10; 	extern const CR dr11;
	extern const CR dr12; 	extern const CR dr13; 	extern const CR dr14;
	extern const CR dr15;   extern const CR invalid; extern const CR rip;
}

struct sib_specifier {
public:
	int multiplier;
	cpu_register scaled;
	cpu_register offset;
	int32_t displacement;

	const std::string rip_label;

	sib_specifier(int32_t displacement_) :
		multiplier(1), scaled(R::invalid), 
		offset(R::invalid), displacement(displacement_)
	{
	}
	sib_specifier(cpu_register which, int32_t displacement_ = 0) :
		multiplier(1), scaled(R::invalid), 
		offset(which), displacement(displacement_)
	{
	}
	sib_specifier(int multiplier_, cpu_register scaled_, int32_t displacement_ = 0) :
		multiplier(multiplier_), scaled(scaled_), 
		offset(R::invalid), displacement(displacement_)
	{
	}
	sib_specifier(int multiplier_, cpu_register scaled_, cpu_register offset_, int32_t displacement_ = 0) :
		multiplier(multiplier_), scaled(scaled_), 
		offset(offset_), displacement(displacement_)
	{
	}

	sib_specifier(const std::string& label) :
		multiplier(1), scaled(R::invalid),
		offset(R::rip), displacement(0),
		rip_label(label)
	{
	}

	register_type type() const {
		if (offset.valid())
			return offset.type;
		else
			return scaled.type;
	}

	size_t size() const;

	bool needs_sib() const;
	disp_size displacement_size() const;
	std::string to_string();

};

class mod_rm_specifier {
private:
	bool valid_;

public:	
	bool sib;

	// Prefices 0x40 - 0x 4f
	bool strict_rex;
	bool rex_r; bool rex_b;
	bool rex_x; bool rex_w;

	// Prefices 0x66 and 0x67
	bool operand_size_override;
	bool address_override;

	uint8_t value;
	uint8_t sib_value;

	disp_size displacement_size;

	uint8_t get_rex() const {
		return 0x40
			| (rex_w ? 0x8 : 0x00)
			| (rex_r ? 0x4 : 0x00)
			| (rex_x ? 0x2 : 0x00)
			| (rex_b ? 0x1 : 0x00);
	}

	bool rex() const {
		return strict_rex || rex_r || rex_b || rex_x || rex_w;
	}

	operator bool() { return valid_; }


	// For one-operand instructions
	mod_rm_specifier(cpu_register rm, size_t size, uint8_t index);
	mod_rm_specifier(sib_specifier rm, size_t size, uint8_t index);

	// For two-operand instructions
	mod_rm_specifier(cpu_register reg, cpu_register rm);
	mod_rm_specifier(cpu_register reg, sib_specifier rm);
};



uint8_t get_reg_rm(cpu_register reg, cpu_register rm);

}