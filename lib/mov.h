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

namespace spasm {

instruction mov_indirect(sib_specifier rm, cpu_register reg, bool reg_to_rm);

instruction mov(cpu_register rm, cpu_register reg);
instruction mov(sib_specifier rm, cpu_register reg);

instruction mov(cpu_register reg, sib_specifier rm);

instruction mov_imm(sib_specifier rm, void* value, size_t size);
instruction mov_imm(cpu_register reg, void* value, size_t size);

template<typename T> instruction movb(cpu_register reg, T value) { return mov_imm(reg, &value, 1); }
template<typename T> instruction movw(cpu_register reg, T value) { return mov_imm(reg, &value, 2); }
template<typename T> instruction movl(cpu_register reg, T value) { return mov_imm(reg, &value, 4); }
template<typename T> instruction movq(cpu_register reg, T value) { return mov_imm(reg, &value, 8); }

template<typename T> instruction movb(sib_specifier rm, T value) { return mov_imm(rm, &value, 1); }
template<typename T> instruction movw(sib_specifier rm, T value) { return mov_imm(rm, &value, 2); }
template<typename T> instruction movl(sib_specifier rm, T value) { return mov_imm(rm, &value, 4); }

}