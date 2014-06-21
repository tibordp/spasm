#include "mov.h"

namespace spasm 
{

void push_prefices (instruction& output, const mod_rm_specifier& mod_rm)
{
	if (mod_rm.address_override) output.push_back(0x67);
	if (mod_rm.operand_size_override) output.push_back(0x66);
	if (mod_rm.rex) output.push_back(mod_rm.get_rex());
}

bool valid_register(const cpu_register& reg)
{
	return (reg.type == register_type::r64) 
		|| (reg.type == register_type::r8)
		|| (reg.type == register_type::r32) 
		|| (reg.type == register_type::r16) ;
}

void push_instruction(instruction& output, const cpu_register& reg, 
	const uint8_t value_r8, const uint8_t value_others)
{
	switch (reg.type)
	{
	case register_type::r8:
		output.push_back(value_r8);
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		output.push_back(value_others);
		break;
	default:
		;
	}
}

// -------------------------------------------------------------------------------------------------//

instruction mov(cpu_register rm, cpu_register reg)
{
	instruction result;
	mod_rm_specifier mod_rm;

	if (!valid_register(reg) || (reg.type != rm.type) || (!direct(mod_rm, reg, rm)))
		return{};

	push_prefices (result, mod_rm);
	push_instruction(result, reg,  0x88, 0x89);
	result.push_back(mod_rm.value);

	return result;
}

instruction mov_indirect(sib_specifier rm, cpu_register reg, bool reg_to_rm)
{
	instruction result;
	mod_rm_specifier mod_rm;
	
	if (!indirect(mod_rm, reg, rm) || !valid_register(reg))
		return{};
	
	push_prefices (result, mod_rm);
	push_instruction(result, reg,  reg_to_rm ? 0x88 : 0x8a, reg_to_rm ? 0x89 : 0x8b);
	result.push_back(mod_rm.value);

	if (mod_rm.sib) result.push_back(mod_rm.sib_value);
	result.push_displacement(result.end(), mod_rm.displacement_size, rm.displacement);

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
	instruction result;
	mod_rm_specifier mod_rm;

	if (!valid_register(reg) || !direct(mod_rm, reg, reg))
		return{};

	mod_rm.rex_r = false; // It is irrelephant

	push_prefices (result, mod_rm);
	if (reg.size() != size) return {};

	push_instruction(result, reg,  0xB0 + reg.index(), 0xB8 + reg.index());
	result.push_data(result.end(), value, size);

	return result;
}

instruction mov_imm(sib_specifier rm, void* value, size_t size)
{
	instruction result;
	cpu_register reg;
	mod_rm_specifier mod_rm;

	switch (size)
	{
		case 1: reg = R::al; break;
		case 2: reg = R::ax; break;
		case 4: reg = R::eax; break;
		case 8: reg = R::rax; break;
		default:
			return{};
	}

	if (!valid_register(reg) ||!indirect(mod_rm, reg, rm))
		return{};

	push_prefices (result, mod_rm);
	push_instruction(result, reg,  0xC6, 0xC7);

	result.push_back(mod_rm.value);
	if (mod_rm.sib) result.push_back(mod_rm.sib_value);
	result.push_displacement(result.end(), mod_rm.displacement_size, rm.displacement);
	result.push_data(result.end(), value, size);

	return result;
}

}