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

#include <limits>

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
	if (mod_rm.rex()) push_back(mod_rm.get_rex());
}

void instruction::push_instruction(size_t size, 
	const uint8_t value_r8, const uint8_t value_others)
{
	switch (size)
	{
	case 1:
		push_back(value_r8);
		break;
	case 2:
	case 4:
	case 8:
		push_back(value_others);
		break;
	default:
		;
	}
}

void instruction::push_data(iterator whither, void* value, size_t size)
{
	insert(whither, static_cast<uint8_t*>(value), static_cast<uint8_t*>(value) + size);
}

void instruction::push_displacement(iterator whither, disp_size size, int32_t displacement)
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

// -------------------------------------------------------------------------------------------------//

bool instruction::direct_double(const cpu_register& rm, const cpu_register& reg,
	const uint8_t value_r8, const uint8_t value_others)
{
	mod_rm_specifier mod_rm(reg, rm);

	if (!valid_register(reg) || (reg.type != rm.type) || !mod_rm)
		return false;

	push_prefices (mod_rm);
	push_instruction(reg.size(), value_r8, value_others);
	push_back(mod_rm.value);

	return true;
}

bool instruction::indirect_double(const sib_specifier& rm, const cpu_register& reg,
	const uint8_t value_r8, const uint8_t value_others)
{
	mod_rm_specifier mod_rm(reg, rm);
	
	if (!mod_rm || !valid_register(reg))
		return false;
	
	push_prefices (mod_rm);
	push_instruction(reg.size(), value_r8, value_others);
	push_back(mod_rm.value);

	if (mod_rm.sib) push_back(mod_rm.sib_value);
	push_displacement(end(), mod_rm.displacement_size, rm.displacement);

	if (rm.has_label())
		rm.label().targets.push_back({this->size() - 4, this->size()});

	return true;
}

bool instruction::direct_simple(const cpu_register& rm, 
	const uint8_t value_r8, const uint8_t value_others, bool add_index, uint8_t index, bool skip_rexw)
{
	mod_rm_specifier mod_rm(rm, rm.size(), index);

	if (!mod_rm) return false;
	mod_rm.rex_r = false; // It is irrelephant
	if (skip_rexw) mod_rm.rex_w = false;

	push_prefices (mod_rm);
	
	if (add_index)
		push_instruction(rm.size(),  value_r8 + rm.index(), value_others + rm.index());
	else
	{
		push_instruction(rm.size(),  value_r8, value_others);
		push_back(mod_rm.value);
	}

	return true;
}

bool instruction::indirect_nolabel(const sib_specifier& rm, size_t ptr_size,
	const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw)
{
	mod_rm_specifier mod_rm(rm, ptr_size, index);

	if (!mod_rm) return false;
	mod_rm.rex_r = false; // It is irrelephant
	if (skip_rexw) mod_rm.rex_w = false;

	push_prefices(mod_rm);
	push_instruction(ptr_size, value_r8, value_others);
	push_back(mod_rm.value);

	if (mod_rm.sib) push_back(mod_rm.sib_value);

	push_displacement(end(), mod_rm.displacement_size, rm.displacement);

	return true;
}

bool instruction::indirect_simple(const sib_specifier& rm, size_t ptr_size,
	const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw)
{
	if (!indirect_nolabel(rm, ptr_size, value_r8, value_others, index, skip_rexw)) return false;

	if (rm.has_label())
		rm.label().targets.push_back({this->size() - 4, this->size()});

	return true;
}

bool instruction::direct_immediate(const cpu_register& rm, void* value, size_t size,
	const uint8_t value_r8, const uint8_t value_others, bool add_index, uint8_t index, bool skip_rexw)
{
	bool success = direct_simple(rm, value_r8, value_others, add_index, index, skip_rexw);
	if (success)
	{
		push_data(end(), value, size);
		return true;
	}
	else
		return false;
}

bool instruction::indirect_immediate(const sib_specifier& rm, size_t ptr_size, void* value, size_t size,
	const uint8_t value_r8, const uint8_t value_others, uint8_t index, bool skip_rexw)
{
	if (!indirect_nolabel(rm, ptr_size, value_r8, value_others, index, skip_rexw)) return false;

	push_data(end(), value, size);
	
	if (rm.has_label())
		rm.label().targets.push_back({this->size() - size - 4, this->size()});

	return true;
}

bool instruction::nop(size_t size) 
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


bool instruction::conditional_jump(int32_t offset, uint8_t opcode, bool address_override)
{
	if ((offset <= std::numeric_limits<int8_t>::max()) &&
		(offset >= std::numeric_limits<int8_t>::min()))
	{
		if (address_override)
			push_back(0x67);
		push_back(opcode);
		push_back(static_cast<int8_t>(offset));
	}
	else
	{
		// jrcxz / jecxz
		if (opcode == 0xe3)
			return false;
		push_back(0x0f);
		push_back(opcode + 0x10);
		push_value<int32_t>(end(), offset);
	}
	return true;
}

bool instruction::conditional_jump(code_label& label, uint8_t opcode)
{
	if (opcode == 0xe3)
		return false;
	push_back(0x0f);
	push_back(opcode + 0x10);
	push_value<int32_t>(end(), 0);

	label.targets.push_back({this->size() - 4, this->size()});

	return true;
}

bool instruction::jmp(int32_t offset)
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

bool instruction::jmp(code_label& label)	
{
	push_back(0xe9);
	push_value<int32_t>(end(), 0);
	label.targets.push_back({size() - 4, size()});

	return true;
}

bool instruction::jmp(const cpu_register& rm) 
{  
	if (rm.type != register_type::r64 ) return false;
	return direct_simple(rm, 0xff, 0xff, false, 4, true);
} 	

bool instruction::jmp(const sib_specifier& rm) 
{  
	if ((rm.type() != register_type::r64) && (rm.type() != register_type::rip)) return false;
	return indirect_simple(rm, 1, 0xff, 0xff, 4, true);
} 	

bool instruction::call(int32_t offset)
{
	push_back(0xe8);
	push_value<int32_t>(end(), offset);
	return true;
}

bool instruction::call(const cpu_register& rm) 
{  
	if (rm.type != register_type::r64 ) return false;
	return direct_simple(rm, 0xff, 0xff, false, 2, true);
} 	

bool instruction::call(const sib_specifier& rm) 
{  
	if ((rm.type() != register_type::r64) &&  (rm.type() != register_type::rip)) return false;
	return indirect_simple(rm, 1, 0xff, 0xff, 2, true);
} 


}