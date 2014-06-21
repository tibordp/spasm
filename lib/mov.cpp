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
	mod_rm_specifier mod_rm(reg, rm);

	if (!valid_register(reg) || (reg.type != rm.type) || !mod_rm)
		return{};

	push_prefices (result, mod_rm);
	push_instruction(result, reg,  0x88, 0x89);
	result.push_back(mod_rm.value);

	return result;
}

instruction mov_indirect(sib_specifier rm, cpu_register reg, bool reg_to_rm)
{
	instruction result;
	mod_rm_specifier mod_rm(reg, rm);
	
	if (!mod_rm || !valid_register(reg))
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
	mod_rm_specifier mod_rm(reg, reg);

	if (!valid_register(reg) || !mod_rm)
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

	switch (size)
	{
		case 1: reg = R::al; break;
		case 2: reg = R::ax; break;
		case 4: reg = R::eax; break;
		case 8: reg = R::rax; break;
		default:
			return{};
	}

	mod_rm_specifier mod_rm(reg, rm);

	if (!valid_register(reg) || !mod_rm)
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