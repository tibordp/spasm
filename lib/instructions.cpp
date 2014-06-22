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

#include "instructions.h"

namespace spasm 
{

bool valid_register(const cpu_register& reg)
{
	return (reg.type == register_type::r64) 
		|| (reg.type == register_type::r8)
		|| (reg.type == register_type::r32) 
		|| (reg.type == register_type::r16) ;
}

void instruction::push_prefices (const mod_rm_specifier& mod_rm)
{
	if (mod_rm.address_override) push_back(0x67);
	if (mod_rm.operand_size_override) push_back(0x66);
	if (mod_rm.rex) push_back(mod_rm.get_rex());
}

void instruction::push_instruction(const cpu_register& reg, 
	const uint8_t value_r8, const uint8_t value_others)
{
	switch (reg.type)
	{
	case register_type::r8:
		push_back(value_r8);
		break;
	case register_type::r16:
	case register_type::r32:
	case register_type::r64:
		push_back(value_others);
		break;
	default:
		;
	}
}

// -------------------------------------------------------------------------------------------------//

bool instruction::direct(const cpu_register& rm, const cpu_register& reg,
	const uint8_t value_r8, const uint8_t value_others)
{
	mod_rm_specifier mod_rm(reg, rm);

	if (!valid_register(reg) || (reg.type != rm.type) || !mod_rm)
		return false;

	push_prefices (mod_rm);
	push_instruction(reg, value_r8, value_others);
	push_back(mod_rm.value);

	return true;
}

bool instruction::indirect(const sib_specifier& rm, const cpu_register& reg,
	const uint8_t value_r8, const uint8_t value_others)
{
	mod_rm_specifier mod_rm(reg, rm);
	
	if (!mod_rm || !valid_register(reg))
		return false;
	
	push_prefices (mod_rm);
	push_instruction(reg, value_r8, value_others);
	push_back(mod_rm.value);

	if (mod_rm.sib) push_back(mod_rm.sib_value);
	push_displacement(end(), mod_rm.displacement_size, rm.displacement);

	return true;
}

bool instruction::direct_simple(const cpu_register& rm, 
	const uint8_t value_r8, const uint8_t value_others, bool add_index, int offset)
{
	cpu_register reg;
	switch (rm.size())
	{
		case 1: reg = R::al; break;
		case 2: reg = R::ax; break;
		case 4: reg = R::eax; break;
		case 8: reg = R::rax; break;
		default:
			return false;
	}

	mod_rm_specifier mod_rm(reg, rm);

	if (!valid_register(rm) || !mod_rm)
		return false;

	mod_rm.rex_r = false; // It is irrelephant

	push_prefices (mod_rm);
	
	if (add_index)
		push_instruction(rm,  value_r8 + rm.index(), value_others + rm.index());
	else
	{
		push_instruction(rm,  value_r8, value_others);
		push_back(mod_rm.value + 8 * offset);
	}

	return true;
}

bool instruction::indirect_simple(const sib_specifier& rm, size_t ptr_size,
	const uint8_t value_r8, const uint8_t value_others, int offset)
{
	cpu_register reg;
	switch (ptr_size)
	{
		case 1: reg = R::al; break;
		case 2: reg = R::ax; break;
		case 4: reg = R::eax; break;
		case 8: reg = R::rax; break;
		default:
			return false;
	}

	mod_rm_specifier mod_rm(reg, rm);

	mod_rm.rex_r = false; // It is irrelephant

	if (!valid_register(reg) || !mod_rm )
		return false;

	push_prefices (mod_rm);
	push_instruction(reg,  value_r8, value_others);
	
	push_back(mod_rm.value + 8 * offset);

	if (mod_rm.sib) push_back(mod_rm.sib_value);
	push_displacement(end(), mod_rm.displacement_size, rm.displacement);

	return true;
}

bool instruction::direct_immediate(const cpu_register& rm, void* value, size_t size,
	const uint8_t value_r8, const uint8_t value_others, bool add_index, int offset)
{
	bool success = direct_simple(rm, value_r8, value_others, add_index, offset);
	if (success)
	{
		push_data(end(), value, size);
		return true;
	}
	else
		return false;
}

bool instruction::indirect_immediate(const sib_specifier& rm, size_t ptr_size, void* value, size_t size,
	const uint8_t value_r8, const uint8_t value_others, int offset)
{
	bool success = indirect_simple(rm, ptr_size, value_r8, value_others, offset);
	if (success)
	{
		push_data(end(), value, size);
		return true;
	}
	else
		return false;
}


}