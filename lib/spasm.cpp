#include <sstream>
#include <limits>

#include "spasm.h"

namespace spasm {

namespace R {
	const CR al = CRS::insert({ RT::r8, RA::no_rex, "al", 0 });
	const CR cl = CRS::insert({ RT::r8, RA::no_rex, "cl", 1 });
	const CR dl = CRS::insert({ RT::r8, RA::no_rex, "dl", 2 });
	const CR bl = CRS::insert({ RT::r8, RA::no_rex, "bl", 3 });
	const CR ah = CRS::insert({ RT::r8, RA::no_rex, "ah", 4 });
	const CR ch = CRS::insert({ RT::r8, RA::no_rex, "ch", 5 });
	const CR dh = CRS::insert({ RT::r8, RA::no_rex, "dh", 6 });
	const CR bh = CRS::insert({ RT::r8, RA::no_rex, "bh", 7 });
	const CR spl = CRS::insert({ RT::r8, RA::no_rex, "spl", 12 });
	const CR bpl = CRS::insert({ RT::r8, RA::no_rex, "bpl", 13 });
	const CR sil = CRS::insert({ RT::r8, RA::no_rex, "sil", 14 });
	const CR dil = CRS::insert({ RT::r8, RA::no_rex, "dil", 15 });
	const CR r8b = CRS::insert({ RT::r8, RA::rex, "r8b", 16 });
	const CR r9b = CRS::insert({ RT::r8, RA::rex, "r9b", 17 });
	const CR r10b = CRS::insert({ RT::r8, RA::rex, "r10b", 18 });
	const CR r11b = CRS::insert({ RT::r8, RA::rex, "r11b", 19 });
	const CR r12b = CRS::insert({ RT::r8, RA::rex, "r12b", 20 });
	const CR r13b = CRS::insert({ RT::r8, RA::rex, "r13b", 21 });
	const CR r14b = CRS::insert({ RT::r8, RA::rex, "r14b", 22 });
	const CR r15b = CRS::insert({ RT::r8, RA::rex, "r15b", 23 });
	const CR ax = CRS::insert({ RT::r16, RA::no_rex, "ax", 24 });
	const CR cx = CRS::insert({ RT::r16, RA::no_rex, "cx", 25 });
	const CR dx = CRS::insert({ RT::r16, RA::no_rex, "dx", 26 });
	const CR bx = CRS::insert({ RT::r16, RA::no_rex, "bx", 27 });
	const CR sp = CRS::insert({ RT::r16, RA::no_rex, "sp", 28 });
	const CR bp = CRS::insert({ RT::r16, RA::no_rex, "bp", 29 });
	const CR si = CRS::insert({ RT::r16, RA::no_rex, "si", 30 });
	const CR di = CRS::insert({ RT::r16, RA::no_rex, "di", 31 });
	const CR r8w = CRS::insert({ RT::r16, RA::rex, "r8w", 32 });
	const CR r9w = CRS::insert({ RT::r16, RA::rex, "r9w", 33 });
	const CR r10w = CRS::insert({ RT::r16, RA::rex, "r10w", 34 });
	const CR r11w = CRS::insert({ RT::r16, RA::rex, "r11w", 35 });
	const CR r12w = CRS::insert({ RT::r16, RA::rex, "r12w", 36 });
	const CR r13w = CRS::insert({ RT::r16, RA::rex, "r13w", 37 });
	const CR r14w = CRS::insert({ RT::r16, RA::rex, "r14w", 38 });
	const CR r15w = CRS::insert({ RT::r16, RA::rex, "r15w", 39 });
	const CR eax = CRS::insert({ RT::r32, RA::no_rex, "eax", 40 });
	const CR ecx = CRS::insert({ RT::r32, RA::no_rex, "ecx", 41 });
	const CR edx = CRS::insert({ RT::r32, RA::no_rex, "edx", 42 });
	const CR ebx = CRS::insert({ RT::r32, RA::no_rex, "ebx", 43 });
	const CR esp = CRS::insert({ RT::r32, RA::no_rex, "esp", 44 });
	const CR ebp = CRS::insert({ RT::r32, RA::no_rex, "ebp", 45 });
	const CR esi = CRS::insert({ RT::r32, RA::no_rex, "esi", 46 });
	const CR edi = CRS::insert({ RT::r32, RA::no_rex, "edi", 47 });
	const CR r8d = CRS::insert({ RT::r32, RA::rex, "r8d", 48 });
	const CR r9d = CRS::insert({ RT::r32, RA::rex, "r9d", 49 });
	const CR r10d = CRS::insert({ RT::r32, RA::rex, "r10d", 50 });
	const CR r11d = CRS::insert({ RT::r32, RA::rex, "r11d", 51 });
	const CR r12d = CRS::insert({ RT::r32, RA::rex, "r12d", 52 });
	const CR r13d = CRS::insert({ RT::r32, RA::rex, "r13d", 53 });
	const CR r14d = CRS::insert({ RT::r32, RA::rex, "r14d", 54 });
	const CR r15d = CRS::insert({ RT::r32, RA::rex, "r15d", 55 });
	const CR rax = CRS::insert({ RT::r64, RA::no_rex, "rax", 56 });
	const CR rcx = CRS::insert({ RT::r64, RA::no_rex, "rcx", 57 });
	const CR rdx = CRS::insert({ RT::r64, RA::no_rex, "rdx", 58 });
	const CR rbx = CRS::insert({ RT::r64, RA::no_rex, "rbx", 59 });
	const CR rsp = CRS::insert({ RT::r64, RA::no_rex, "rsp", 60 });
	const CR rbp = CRS::insert({ RT::r64, RA::no_rex, "rbp", 61 });
	const CR rsi = CRS::insert({ RT::r64, RA::no_rex, "rsi", 62 });
	const CR rdi = CRS::insert({ RT::r64, RA::no_rex, "rdi", 63 });
	const CR r8 = CRS::insert({ RT::r64, RA::rex, "r8", 64 });
	const CR r9 = CRS::insert({ RT::r64, RA::rex, "r9", 65 });
	const CR r10 = CRS::insert({ RT::r64, RA::rex, "r10", 66 });
	const CR r11 = CRS::insert({ RT::r64, RA::rex, "r11", 67 });
	const CR r12 = CRS::insert({ RT::r64, RA::rex, "r12", 68 });
	const CR r13 = CRS::insert({ RT::r64, RA::rex, "r13", 69 });
	const CR r14 = CRS::insert({ RT::r64, RA::rex, "r14", 70 });
	const CR r15 = CRS::insert({ RT::r64, RA::rex, "r15", 71 });
	const CR mm0 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm0", 72 });
	const CR mm1 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm1", 73 });
	const CR mm2 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm2", 74 });
	const CR mm3 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm3", 75 });
	const CR mm4 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm4", 76 });
	const CR mm5 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm5", 77 });
	const CR mm6 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm6", 78 });
	const CR mm7 = CRS::insert({ RT::mm, RA::rex_no_rex, "mm7", 79 });
	const CR xmm0 = CRS::insert({ RT::xmm, RA::no_rex, "xmm0", 80 });
	const CR xmm1 = CRS::insert({ RT::xmm, RA::no_rex, "xmm1", 81 });
	const CR xmm2 = CRS::insert({ RT::xmm, RA::no_rex, "xmm2", 82 });
	const CR xmm3 = CRS::insert({ RT::xmm, RA::no_rex, "xmm3", 83 });
	const CR xmm4 = CRS::insert({ RT::xmm, RA::no_rex, "xmm4", 84 });
	const CR xmm5 = CRS::insert({ RT::xmm, RA::no_rex, "xmm5", 85 });
	const CR xmm6 = CRS::insert({ RT::xmm, RA::no_rex, "xmm6", 86 });
	const CR xmm7 = CRS::insert({ RT::xmm, RA::no_rex, "xmm7", 87 });
	const CR xmm8 = CRS::insert({ RT::xmm, RA::rex, "xmm8", 88 });
	const CR xmm9 = CRS::insert({ RT::xmm, RA::rex, "xmm9", 89 });
	const CR xmm10 = CRS::insert({ RT::xmm, RA::rex, "xmm10", 90 });
	const CR xmm11 = CRS::insert({ RT::xmm, RA::rex, "xmm11", 91 });
	const CR xmm12 = CRS::insert({ RT::xmm, RA::rex, "xmm12", 92 });
	const CR xmm13 = CRS::insert({ RT::xmm, RA::rex, "xmm13", 93 });
	const CR xmm14 = CRS::insert({ RT::xmm, RA::rex, "xmm14", 94 });
	const CR xmm15 = CRS::insert({ RT::xmm, RA::rex, "xmm15", 95 });
	const CR es = CRS::insert({ RT::sreg, RA::rex_no_rex, "es", 96 });
	const CR cs = CRS::insert({ RT::sreg, RA::rex_no_rex, "cs", 97 });
	const CR ss = CRS::insert({ RT::sreg, RA::rex_no_rex, "ss", 98 });
	const CR ds = CRS::insert({ RT::sreg, RA::rex_no_rex, "ds", 99 });
	const CR fs = CRS::insert({ RT::sreg, RA::rex_no_rex, "fs", 100 });
	const CR gs = CRS::insert({ RT::sreg, RA::rex_no_rex, "gs", 101 });
	const CR cr0 = CRS::insert({ RT::eee, RA::no_rex, "cr0", 104 });
	const CR cr1 = CRS::insert({ RT::eee, RA::no_rex, "cr1", 105 });
	const CR cr2 = CRS::insert({ RT::eee, RA::no_rex, "cr2", 106 });
	const CR cr3 = CRS::insert({ RT::eee, RA::no_rex, "cr3", 107 });
	const CR cr4 = CRS::insert({ RT::eee, RA::no_rex, "cr4", 108 });
	const CR cr5 = CRS::insert({ RT::eee, RA::no_rex, "cr5", 109 });
	const CR cr6 = CRS::insert({ RT::eee, RA::no_rex, "cr6", 110 });
	const CR cr7 = CRS::insert({ RT::eee, RA::no_rex, "cr7", 111 });
	const CR cr8 = CRS::insert({ RT::eee, RA::rex, "cr8", 112 });
	const CR cr9 = CRS::insert({ RT::eee, RA::rex, "cr9", 113 });
	const CR cr10 = CRS::insert({ RT::eee, RA::rex, "cr10", 114 });
	const CR cr11 = CRS::insert({ RT::eee, RA::rex, "cr11", 115 });
	const CR cr12 = CRS::insert({ RT::eee, RA::rex, "cr12", 116 });
	const CR cr13 = CRS::insert({ RT::eee, RA::rex, "cr13", 117 });
	const CR cr14 = CRS::insert({ RT::eee, RA::rex, "cr14", 118 });
	const CR cr15 = CRS::insert({ RT::eee, RA::rex, "cr15", 119 });
	const CR dr0 = CRS::insert({ RT::eee, RA::no_rex, "dr0", 120 });
	const CR dr1 = CRS::insert({ RT::eee, RA::no_rex, "dr1", 121 });
	const CR dr2 = CRS::insert({ RT::eee, RA::no_rex, "dr2", 122 });
	const CR dr3 = CRS::insert({ RT::eee, RA::no_rex, "dr3", 123 });
	const CR dr4 = CRS::insert({ RT::eee, RA::no_rex, "dr4", 124 });
	const CR dr5 = CRS::insert({ RT::eee, RA::no_rex, "dr5", 125 });
	const CR dr6 = CRS::insert({ RT::eee, RA::no_rex, "dr6", 126 });
	const CR dr7 = CRS::insert({ RT::eee, RA::no_rex, "dr7", 127 });
	const CR dr8 = CRS::insert({ RT::eee, RA::rex, "dr8", 128 });
	const CR dr9 = CRS::insert({ RT::eee, RA::rex, "dr9", 129 });
	const CR dr10 = CRS::insert({ RT::eee, RA::rex, "dr10", 130 });
	const CR dr11 = CRS::insert({ RT::eee, RA::rex, "dr11", 131 });
	const CR dr12 = CRS::insert({ RT::eee, RA::rex, "dr12", 132 });
	const CR dr13 = CRS::insert({ RT::eee, RA::rex, "dr13", 133 });
	const CR dr14 = CRS::insert({ RT::eee, RA::rex, "dr14", 134 });
	const CR dr15 = CRS::insert({ RT::eee, RA::rex, "dr15", 135 });

	const CR invalid = CRS::insert({ RT::invalid, RA::invalid, "", -1 });
	const CR rip = CRS::insert({ RT::rip, RA::invalid, "rip", -2 });
}

/*
	get_reg_rm - returns the REG+R/M (without MOD) for given
	two registers.
*/
uint8_t get_reg_rm(cpu_register reg, cpu_register rm)
{
	return (reg.index() << 3) | rm.index();
}

/*
	can_be_used_in_mem - returns true iff the register can be used for
	memory access (eg. `mov rax, [rax]` is valid but ÿmov rax, [xmm0]`
	is not)
*/

bool cpu_register::can_be_used_in_mem() const {
	return in_range(R::rax, R::rdi) ||
		in_range(R::r8, R::r15) ||
		in_range(R::eax, R::edi) ||
		in_range(R::r8d, R::r15d);
}

/*
	is_always_rex - returns true iff the register requires that
	at least one of the REX prefices be used (the new spl-dil 
	registers)
*/
bool cpu_register::always_rex() const
{
	return in_range(R::spl, R::dil);
}

/*
	is_never_rex - returns true iff the register requires that
	no REX prefix be used (ah-bh)
*/
bool cpu_register::never_rex() const
{
	return in_range(R::ah, R::bh);
}

// TODO:
// enum class register_type : int8_t {
// 	r8, r16, r32, r64,
// 	mm, xmm, sreg, eee,
// 	rip, invalid
// };

size_t cpu_register::size() const
{
	switch (type) 
	{
		case register_type::r8: return 1;
		case register_type::r16: return 2;
		case register_type::r32: return 4;
		case register_type::rip:
		case register_type::r64: return 8;
		default:
			return -1;
	}
}

/*
	needs_sib - returns true iff the Mod/R+M specification
	needs an additional SIB byte.
*/
bool sib_specifier::needs_sib() const
{
	return  (scaled.valid()) ||
			(offset.index() == 4);
}

disp_size sib_specifier::displacement_size() const
{
	if (displacement == 0)
		return disp_size::d0;
	if ((displacement <= std::numeric_limits<int8_t>::max()) &&
		(displacement >= std::numeric_limits<int8_t>::min()))
		return disp_size::d8;
	else
		return disp_size::d32;
}

std::string sib_specifier::to_string()
{
	std::stringstream ss;
	ss << "[";
	
	if (scaled.valid())
	{
		ss << multiplier << "*";
		ss << scaled.name;
		if (offset.valid())
			ss << "+";
	}

	if (offset.valid())
	{
		ss << offset.name;
		if (displacement != 0)
			ss << "+";
	}

	if (displacement != 0)
		ss << "0x" << std::hex << displacement;

	ss << "]";

	return ss.str();
}


bool direct(mod_rm_specifier& result, cpu_register reg, cpu_register rm)
{
	if (!reg.valid() || !rm.valid())
		return false;

	// Actual value is the easy part...
	result.value = 0xC0 | get_reg_rm(reg, rm);	// mod = 3
	
	//...then come the prefices.
	result.sib = false;
	result.rex_x = false;
	result.displacement_size = disp_size::d0;
	
	result.address_override = false;
	result.operand_size_override = (reg.type == register_type::r16)
		&& (rm.type == register_type::r16);

	result.rex_r = (reg.rex == rex_attributte::rex);
	result.rex_b = (rm.rex == rex_attributte::rex);

	result.rex_w = (reg.type == register_type::r64)
		|| (rm.type == register_type::r64);

	result.rex = reg.always_rex() || rm.always_rex()
		|| result.rex_r || result.rex_b || result.rex_w;

	if (result.rex && (reg.never_rex() || rm.never_rex()))
		return false; //throw runtime_error("Cannot use an operand that requires a REX prefix.");

	return true;
}

bool indirect(mod_rm_specifier& result, cpu_register reg, sib_specifier rm)
{
	result.sib = rm.needs_sib();

	const bool appropriate_set =
		(rm.scaled.can_be_used_in_mem() || !rm.scaled.valid()) &&
		(rm.offset.can_be_used_in_mem() || !rm.offset.valid() || (rm.offset == R::rip)) &&
		reg.valid();

	if (!appropriate_set)
		return false; //throw runtime_error("This register cannot be used for memory access.");

	if (rm.scaled == R::invalid && rm.offset == R::invalid)
	{
		// For now no displacement-only
		return false;
	}

	if (rm.offset == R::rip)
	{
		if (rm.scaled.valid() || rm.multiplier != 1)
			return false; //throw runtime_error("RIP-relative addressing can be used with displacement only.");

		// RIP-relative can only have 32-bit displacement
		result.displacement_size = disp_size::d32;
		result.value = (reg.index() << 3) | 0x05;
	}
	else if (!result.sib)
	{
		result.displacement_size = rm.displacement_size();

		if ((rm.offset.index() == 5) && (result.displacement_size == disp_size::d0))
			result.displacement_size = disp_size::d8;		

		uint8_t mod = static_cast<uint8_t>(result.displacement_size);
		result.value = (mod << 6) | get_reg_rm(reg, rm.offset);
	}
	else
	{
		if (rm.scaled == R::esp || rm.scaled == R::rsp)
			return false; // throw runtime_error("RSP/ESP cannot be used as a scaled register.");

		// If we want to access just the RSP (eg. mov [rsp + 0xdead], eax), we
		// cannot do it without the SIB byte. In that case the scaled index is
		// 4 - empty (the same as RSP).
		if ((rm.scaled == R::invalid) && (rm.offset.index() == 4))
		{
			if (rm.multiplier != 1)
				return false;

			rm.scaled = R::rsp;
		}

		result.displacement_size = rm.displacement_size();

		// We cannot have registers rbp, ebp, r13, r13d with zero displacement
		if ((rm.offset.index() == 5) && (result.displacement_size == disp_size::d0))
			result.displacement_size = disp_size::d8;

		uint8_t ss;

		switch (rm.multiplier)
		{
		case 1: ss = 0x00; break;
		case 2: ss = 0x40; break;
		case 4: ss = 0x80; break;
		case 8: ss = 0xC0; break;
		default: return false; //  throw runtime_error("Invalid multiplier!");
		}

		result.sib_value = ss | (rm.scaled.index() << 3) | rm.offset.index();

		uint8_t mod = static_cast<uint8_t>(result.displacement_size);

		// Index 4 is used for SIB
		result.value = (mod << 6) | (reg.index() << 3) | 0x04;
	}

	// We calcualte the appropriate prefices

	if (rm.scaled.valid())
	{
		if (rm.scaled.type != rm.offset.type)
			return false; //throw runtime_error("SIB registers must be of the same type.");
		result.address_override = (rm.scaled.type == register_type::r32);
	}
	else	
		result.address_override = (rm.offset.type == register_type::r32);

	result.operand_size_override = (reg.type == register_type::r16);

	result.rex_w = (reg.type == register_type::r64);
	result.rex_x = (rm.scaled.rex == rex_attributte::rex);
	result.rex_r = (reg.rex == rex_attributte::rex);
	result.rex_b = (rm.offset.rex == rex_attributte::rex);

	result.rex = reg.always_rex()
		|| result.rex_w || result.rex_b
		|| result.rex_x || result.rex_r;

	if (result.rex && reg.never_rex())
		return false; //throw runtime_error("Cannot use an operand that requires a REX prefix.");

	return true;
}

}