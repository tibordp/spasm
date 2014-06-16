#include <iostream>
#include <cstdint>
#include <array>
#include <map>
#include <vector>
#include <bitset>
#include <stdexcept>
#include <exception>
#include <limits>

using namespace std;

enum class R : signed {
	invalid = -1,

	al = 0, cl = 1, dl = 2, bl = 3, ah = 4, ch = 5, dh = 6, bh = 7,
	spl = 12, bpl = 13, sil = 14, dil = 15,
	r8b = 16, r9b = 17, r10b = 18, r11b = 19, r12b = 20, r13b = 21, r14b = 22, r15b = 23,
	ax = 24, cx = 25, dx = 26, bx = 27, sp = 28, bp = 29, si = 30, di = 31,
	r8w = 32, r9w = 33, r10w = 34, r11w = 35, r12w = 36, r13w = 37, r14w = 38, r15w = 39,
	eax = 40, ecx = 41, edx = 42, ebx = 43, esp = 44, ebp = 45, esi = 46, edi = 47,
	r8d = 48, r9d = 49, r10d = 50, r11d = 51, r12d = 52, r13d = 53, r14d = 54, r15d = 55,
	rax = 56, rcx = 57, rdx = 58, rbx = 59, rsp = 60, rbp = 61, rsi = 62, rdi = 63,
	r8 = 64, r9 = 65, r10 = 66, r11 = 67, r12 = 68, r13 = 69, r14 = 70, r15 = 71,
	mm0 = 72, mm1 = 73, mm2 = 74, mm3 = 75, mm4 = 76, mm5 = 77, mm6 = 78, mm7 = 79,
	xmm0 = 80, xmm1 = 81, xmm2 = 82, xmm3 = 83, xmm4 = 84, xmm5 = 85, xmm6 = 86, xmm7 = 87,
	xmm8 = 88, xmm9 = 89, xmm10 = 90, xmm11 = 91, xmm12 = 92, xmm13 = 93, xmm14 = 94, xmm15 = 95,
	es = 96, cs = 97, ss = 98, ds = 99, fs = 100, gs = 101,
	cr0 = 104, cr1 = 105, cr2 = 106, cr3 = 107, cr4 = 108, cr5 = 109, cr6 = 110, cr7 = 111,
	cr8 = 112, cr9 = 113, cr10 = 114, cr11 = 115, cr12 = 116, cr13 = 117, cr14 = 118, cr15 = 119,
	dr0 = 120, dr1 = 121, dr2 = 122, dr3 = 123, dr4 = 124, dr5 = 125, dr6 = 126, dr7 = 127,
	dr8 = 128, dr9 = 129, dr10 = 130, dr11 = 131, dr12 = 132, dr13 = 133, dr14 = 134, dr15 = 135,

	// RIP is special
	rip = -2
};

map<R, string> r = { 
	{ R::gs, "gs" }, { R::cr12, "cr12" }, { R::edi, "edi" }, { R::edx, "edx" }, { R::cr2, "cr2" },
	{ R::r13w, "r13w" }, { R::cr0, "cr0" }, { R::cr1, "cr1" }, { R::cr6, "cr6" }, { R::cr7, "cr7" },
	{ R::cr4, "cr4" }, { R::cr5, "cr5" }, { R::cr8, "cr8" }, { R::r8b, "r8b" }, { R::r8d, "r8d" },
	{ R::r13d, "r13d" }, { R::r13b, "r13b" }, { R::r8w, "r8w" }, { R::r14, "r14" }, { R::r15, "r15" },
	{ R::r12, "r12" }, { R::r13, "r13" }, { R::r10, "r10" }, { R::di, "di" }, { R::cr14, "cr14" },
	{ R::cr15, "cr15" }, { R::cr10, "cr10" }, { R::cr11, "cr11" }, { R::dx, "dx" }, { R::cr13, "cr13" },
	{ R::ds, "ds" }, { R::dil, "dil" }, { R::r10b, "r10b" }, { R::bpl, "bpl" }, { R::r10d, "r10d" },
	{ R::xmm10, "xmm10" }, { R::xmm11, "xmm11" }, { R::xmm12, "xmm12" }, { R::xmm13, "xmm13" }, { R::xmm14, "xmm14" },
	{ R::xmm15, "xmm15" }, { R::r11w, "r11w" }, { R::r10w, "r10w" }, { R::mm5, "mm5" }, { R::mm4, "mm4" },
	{ R::mm7, "mm7" }, { R::mm6, "mm6" }, { R::mm1, "mm1" }, { R::mm0, "mm0" }, { R::mm3, "mm3" },
	{ R::mm2, "mm2" }, { R::ebp, "ebp" }, { R::ebx, "ebx" }, { R::es, "es" }, { R::r15d, "r15d" },
	{ R::dr8, "dr8" }, { R::r15b, "r15b" }, { R::esp, "esp" }, { R::dr1, "dr1" }, { R::dr0, "dr0" },
	{ R::dr3, "dr3" }, { R::dr2, "dr2" }, { R::dr5, "dr5" }, { R::dr4, "dr4" }, { R::dr7, "dr7" },
	{ R::dr6, "dr6" }, { R::r15w, "r15w" }, { R::esi, "esi" }, { R::r9b, "r9b" }, { R::cr3, "cr3" },
	{ R::bl, "bl" }, { R::bh, "bh" }, { R::xmm2, "xmm2" }, { R::xmm3, "xmm3" }, { R::xmm0, "xmm0" },
	{ R::xmm1, "xmm1" }, { R::xmm6, "xmm6" }, { R::xmm7, "xmm7" }, { R::xmm4, "xmm4" }, { R::xmm5, "xmm5" },
	{ R::xmm8, "xmm8" }, { R::xmm9, "xmm9" }, { R::bx, "bx" }, { R::ecx, "ecx" }, { R::dr10, "dr10" },
	{ R::dl, "dl" }, { R::r12w, "r12w" }, { R::r9d, "r9d" }, { R::r8, "r8" }, { R::rdx, "rdx" },
	{ R::dr15, "dr15" }, { R::dr14, "dr14" }, { R::r12b, "r12b" }, { R::dr11, "dr11" }, { R::r12d, "r12d" },
	{ R::dr13, "dr13" }, { R::dr12, "dr12" }, { R::bp, "bp" }, { R::rdi, "rdi" }, { R::ch, "ch" },
	{ R::cl, "cl" }, { R::cx, "cx" }, { R::cs, "cs" }, { R::rcx, "rcx" }, { R::rsi, "rsi" },
	{ R::eax, "eax" }, { R::rsp, "rsp" }, { R::r14d, "r14d" }, { R::r14b, "r14b" }, { R::rbx, "rbx" },
	{ R::rbp, "rbp" }, { R::r14w, "r14w" }, { R::ah, "ah" }, { R::al, "al" }, { R::r9, "r9" },
	{ R::spl, "spl" }, { R::ax, "ax" }, { R::rax, "rax" }, { R::dh, "dh" }, { R::r11b, "r11b" },
	{ R::r11, "r11" }, { R::r11d, "r11d" }, { R::cr9, "cr9" }, { R::sil, "sil" }, { R::dr9, "dr9" },
	{ R::fs, "fs" }, { R::r9w, "r9w" }, { R::ss, "ss" }, { R::sp, "sp" }, { R::si, "si" },
	{ R::rip, "rip" }
};

enum class rex_attribute : int8_t {
	no_rex = 1 ,
	rex = 2,
	rex_no_rex = 3
};

enum class register_type : int8_t {
	invalid = -1,

	r8 = 0, r16 = 1, r32 = 2, r64 = 3,
	mm = 4, xmm = 5, sreg = 6, eee = 7,	
};

static const array<uint8_t, 136> reg_attrs = {
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,
	9, 9, 9, 9, 9, 9, 9, 9,
	10, 10, 10, 10, 10, 10, 10, 10,
	13, 13, 13, 13, 13, 13, 13, 13,
	14, 14, 14, 14, 14, 14, 14, 14,
	19, 19, 19, 19, 19, 19, 19, 19,
	21, 21, 21, 21, 21, 21, 21, 21,
	22, 22, 22, 22, 22, 22, 22, 22,
	27, 27, 27, 27, 27, 27, 0, 0,
	29, 29, 29, 29, 29, 29, 29, 29,
	30, 30, 30, 30, 30, 30, 30, 30,
	29, 29, 29, 29, 29, 29, 29, 29,
	30, 30
};

template<typename T>
inline uint8_t to_u8(T what)
{
	return static_cast<uint8_t>(what);
}

/*
	index_of - returns the index (0 - 7) of a given register. This is
	the index used for example for the REG field. 
*/
inline uint8_t index_of(R reg)
{
	const uint8_t mask = 0x07; // 00000111
	return to_u8(reg) & mask;
}

/*
	get_reg_rm - returns the REG+R/M (without MOD) for given
	two registers.
*/
inline uint8_t get_reg_rm(R reg, R rm)
{
	return (index_of(reg) << 3) | index_of(rm);
}

/*
	can_be_used_in_mem - returns true iff the register can be used for
	memory access (eg. `mov rax, [rax]` is valid but ÿmov rax, [xmm0]`
	is not)
*/
inline bool can_be_used_in_mem(R reg) {
	return (reg >= R::rax && reg <= R::rdi) ||
		(reg >= R::r8 && reg <= R::r15) ||
		(reg >= R::eax && reg <= R::edi) ||
		(reg >= R::r8d && reg <= R::r15d);
}

struct register_attribute {
	rex_attribute rex;
	register_type type;

	explicit register_attribute(uint8_t value) {
		rex  = static_cast<rex_attribute>(value & 0x3);
		type = static_cast<register_type>((value >> 2) & 0x7);
	}

	explicit register_attribute(R reg) 
	{
		// We reserve the negative values for pseudo-registers, such as RIP
		if (reg > R::invalid)
			*this = register_attribute(reg_attrs[to_u8(reg)]);
		else
		{
			rex = rex_attribute::no_rex;
			type = register_type::invalid;
		}
	}
};

/*
	is_always_rex - returns true iff the register requires that
	at least one of the REX prefices be used (the new spl-dil 
	registers)
*/
inline bool is_always_rex(R reg)
{
	return (reg >= R::spl) && (reg <= R::dil);
}

/*
	is_never_rex - returns true iff the register requires that
	no REX prefix be used (ah-bh)
*/
inline bool is_never_rex(R reg)
{
	return (reg >= R::ah) && (reg <= R::bh);
}

enum class disp_size : uint8_t {
	// d16?
	d0 = 0, d8 = 1, d32 = 2
};

uint8_t get_ss(unsigned long multiplier)
{
	switch (multiplier)
	{
		case 1: return 0x00;
		case 2: return 0x40;
		case 4: return 0x80;
		case 8: return 0xC0;
		default: throw runtime_error("Invalid multiplier!");
	}
}

struct M {
public:
	int multiplier;
	R scaled;
	R offset;
	int32_t displacement;

	explicit M(int32_t displacement_) :
		scaled(R::invalid), multiplier(1),
		offset(R::invalid), displacement(displacement_)
	{
	}
	explicit M(R which, int32_t displacement_ = 0) : 
		scaled(R::invalid), multiplier(1),
		offset(which), displacement(displacement_)
	{
	}
	explicit M(uint8_t multiplier_,  R scaled_, int32_t displacement_ = 0) : 
		scaled(scaled_), multiplier(multiplier_), 
		offset(R::invalid), displacement(displacement_) 
	{
	}
	explicit M(uint8_t multiplier_, R scaled_, R offset_, int32_t displacement_ = 0) :
		scaled(scaled_), multiplier(multiplier_), 
		offset(offset_), displacement(displacement_)
	{
	}

	bool needs_sib() const
	{
		// Incomplete
		return (multiplier != 1) || 
			(scaled != R::invalid) || 
			(index_of(offset) == 4);
	}

	disp_size displacement_size() const
	{
		if (displacement == 0)
			return disp_size::d0;
		if ((displacement <= numeric_limits<int8_t>::max()) &&
			(displacement >= numeric_limits<int8_t>::min()))
			return disp_size::d8;
		else
			return disp_size::d32;
	}
};

struct mod_rm {
	bool sib;

	bool rex;
	bool rex_r;
	bool rex_b;
	bool rex_x;
	bool rex_w;

	uint8_t value;
	uint8_t sib_value;

	disp_size displacement_size;

	uint8_t get_rex() {
		return 0x40
			| (rex_w ? 0x8 : 0x00)
			| (rex_r ? 0x4 : 0x00)
			| (rex_x ? 0x2 : 0x00)
			| (rex_b ? 0x1 : 0x00);
	}

	mod_rm(R reg, R rm)
		: sib(false), rex_x(false), 
		  displacement_size(disp_size::d0)
	{
		value = 0xC0 | get_reg_rm(reg, rm);	// mod = 3
		
		register_attribute reg_attrs(reg);
		register_attribute rm_attrs(rm);

		rex_r = (reg_attrs.rex == rex_attribute::rex);
		rex_b = (rm_attrs.rex == rex_attribute::rex);

		rex_w = (reg_attrs.type == register_type::r64)
			|| (rm_attrs.type == register_type::r64);

		rex = is_always_rex(reg) || is_always_rex(rm)
			|| rex_r || rex_b || rex_w;

		if (rex && (is_never_rex(reg) || is_never_rex(rm)))
			throw runtime_error("Cannot use an operand that requires a REX prefix.");		
	}

	mod_rm(R reg, M rm)
	{
		sib = rm.needs_sib();

		const bool appropriate_set = 
			(can_be_used_in_mem(rm.scaled) || (rm.scaled == R::invalid)) &&
			(can_be_used_in_mem(rm.offset) || (rm.offset == R::invalid) || (rm.offset == R::rip));

		if (!appropriate_set)
			throw runtime_error("This register cannot be used for memory access.");

		if (rm.offset == R::rip)
		{
			if (rm.scaled != R::invalid || rm.multiplier != 1)
				throw runtime_error("RIP-relative addressing can be used with displacement only.");
			
			// RIP-relative can only have 32-bit displacement
			displacement_size = disp_size::d32;
			value = (index_of(reg) << 3) | 0x05;
		}
		else if (!sib)
		{
			displacement_size = rm.displacement_size();

			uint8_t mod = to_u8(displacement_size);
			value = (mod << 6) | get_reg_rm(reg, rm.offset);
		}
		else
		{
			if (rm.scaled == R::esp || rm.scaled == R::rsp)
				throw runtime_error("RSP/ESP cannot be used as a scaled register.");

			// If we want to access just the RSP (eg. mov [rsp + 0xdead], eax), we
			// cannot do it without the SIB byte. In that case the scaled index is
			// 4 - empty (the same as RSP).
			if (rm.scaled == R::invalid && index_of(rm.offset) == 4)
				rm.scaled = R::rsp;

			displacement_size = rm.displacement_size();

			if (index_of(rm.offset) == 5)  // rbp, ebp, r13, r13d
			{
				// We cannot have these registers with zero displacement
				if (displacement_size == disp_size::d0)
					displacement_size = disp_size::d8;
			}

			sib_value = get_ss(rm.multiplier) | (index_of(rm.scaled) << 3) | index_of(rm.offset);
			
			uint8_t mod = to_u8(displacement_size);
			
			// Index 4 is used for SIB
			value = (mod << 6) | (index_of(reg) << 3) | 0x04; 	
		}

		// We calcualte the appropriate REX flag
		register_attribute reg_attrs(reg);
		register_attribute scaled_attrs(rm.scaled);
		register_attribute offset_attrs(rm.offset);

		if ((rm.scaled != R::invalid) && (scaled_attrs.type != offset_attrs.type))
			throw runtime_error("SIB registers must be of the same type.");

		rex_w = (reg_attrs.type == register_type::r64);

		rex_x = (scaled_attrs.rex == rex_attribute::rex);
		rex_r = (reg_attrs.rex == rex_attribute::rex);
		rex_b = (offset_attrs.rex == rex_attribute::rex);

		rex = is_always_rex(reg)
			|| rex_w || rex_b
			|| rex_x || rex_r;

		if (rex && is_never_rex(reg))
			throw runtime_error("Cannot use an operand that requires a REX prefix.");
	}
};

void print(mod_rm mrm)
{
	cout << hex;
	if (mrm.rex)
		cout << "REX: " << (int)mrm.get_rex() << ", ";
	cout << "value: " << (int)mrm.value;
	if (mrm.sib)
		cout << " " <<  (int)mrm.sib_value;
}


int main() 
{
	for (auto i : r)
		for (auto j : r)
			for (auto k : r)
		{
			try {
				auto m = mod_rm(i.first, M(1, j.first, k.first));
				//cout << i.second << ", [" << j.second << k.second << "] ";  print(m);
				//cout << endl;
			}
			catch (...)
			{

			}
			
		}
			
	cout << "Done!";
	
	

	system("pause");
};