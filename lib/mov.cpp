#include "mov.h"

void push_prefices (instruction& output, const mod_rm_specifier& mod_rm)
{
	if (mod_rm.address_override) output.push_back(0x67);
	if (mod_rm.operand_size_override) output.push_back(0x66);
	if (mod_rm.rex) output.push_back(mod_rm.get_rex());
}

instruction mov_indirect(sib_specifier rm, cpu_register reg, bool reg_to_rm)
{
	// Incomplete
	mod_rm_specifier mod_rm;
	if (!indirect(mod_rm, reg, rm))
		return{};

	instruction result;

	push_prefices (result, mod_rm);

	switch (reg.type)
	{
	case register_type::r8:
		result.push_back(reg_to_rm ? 0x88 : 0x8a);
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		result.push_back(reg_to_rm ? 0x89 : 0x8b);
		break;
	default:
		return{};
	}

	result.push_back(mod_rm.value);
	if (mod_rm.sib) result.push_back(mod_rm.sib_value);

	result.push_displacement(result.end(), mod_rm.displacement_size, rm.displacement);
	return result;
}

instruction mov(cpu_register rm, cpu_register reg)
{
	// Incomplete
	mod_rm_specifier mod_rm;
	if ((reg.type != rm.type) || (!direct(mod_rm, reg, rm)))
		return{};

	instruction result;
	
	push_prefices (result, mod_rm);

	switch (reg.type)
	{
	case register_type::r8:
		result.push_back(0x88);
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		result.push_back(0x89);
		break;
	default:
		return{};
	}

	result.push_back(mod_rm.value);
	return result;
}

instruction mov(sib_specifier rm, cpu_register reg)
{
	return mov_indirect(rm, reg, true);
}

instruction mov(cpu_register reg, sib_specifier rm)
{
	return mov_indirect(rm, reg, false);
}

instruction mov_imm(cpu_register reg, void* value, size_t size)
{
	mod_rm_specifier mod_rm;
	if (!direct(mod_rm, reg, reg))
		return{};

	mod_rm.rex_r = false; // It is irrelephant
	instruction result;

	push_prefices (result, mod_rm);

	if (reg.size() != size)
		return {};

	switch (reg.type)
	{
	case register_type::r8:
		result.push_back(0xB0 + reg.index());
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		result.push_back(0xB8 + reg.index());
		break;
	default:
		return{};
	}

	// result.push_back(mod_rm.value);
	result.push_data(result.end(), value, size);

	return result;
}

instruction mov_imm(sib_specifier rm, void* value, size_t size)
{
	cpu_register reg;

	switch (size)
	{
		case 1: reg = R::al; break;
		case 2: reg = R::ax; break;
		case 4: reg = R::eax; break;
		case 8: reg = R::rax; break;
		default:
			return{};
	}

	mod_rm_specifier mod_rm;

	if (!indirect(mod_rm, reg, rm))
		return{};

	instruction result;

	push_prefices (result, mod_rm);

	switch (reg.type)
	{
	case register_type::r8:
		result.push_back(0xC6);
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		result.push_back(0xC7);
		break;
	default:
		return{};
	}

	result.push_back(mod_rm.value);
	if (mod_rm.sib) result.push_back(mod_rm.sib_value);
	result.push_displacement(result.end(), mod_rm.displacement_size, rm.displacement);
	result.push_data(result.end(), value, size);

	return result;
}
