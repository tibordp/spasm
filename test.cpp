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

#include <utility>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <chrono>
#include <stdexcept>

#include "lib/spasm.h"
#include "lib/instructions.h"
#include "lib/executable.h"

using namespace std;
using namespace spasm;

#include "tests/tests.inc"

/*
	Testing is done with GNU AS. Basically, the test case generates both symbolic assembly code
	and corresponding hex-encoded binary code. Then the program is assembled with `as` and 
	dissassebled with `objdump`. Then the input is parsed and compared with a Python script.
	
	See test.py for details and implementation.
*/

template<typename T>
double time(T callback)
{
	using namespace chrono;
 
	/* TODO: Is it possible that the following lines somehow get reordered?
	   There are some weird results on gcc with -O3 */
	
	auto start = high_resolution_clock::now();
	callback();
	auto end = high_resolution_clock::now();
 
	return duration_cast<duration<double>>(end - start).count();
}

int main()
{
	std::vector<void (*) (ostream&, ostream&)> tests = { 
		executable_test, 
		executable_test2, 
		
		mov_test,

		test_test,
		not__test,
		neg_test,
		mul_test,
		imul_test,
		div_test,
		idiv_test,

		add_test,
		or__test,
		adc_test,
		sbb_test,
		and__test,
		sub_test,
		xor__test,
		cmp_test,

		rol_test,
		ror_test,
		rcl_test,
		rcr_test,
		shl_test,
		sal_test,
		shr_test,
		sar_test,

		inc_test,
		dec_test,
		jcc_test,

		various_test,
	};

	ofstream instructions("instructions.txt");
	ofstream bytes("bytes.txt");
	instructions << ".intel_syntax noprefix" << endl;

	try {
		for (auto test : tests) 
			cerr << "Test took " << time([&] { test(bytes, instructions); }) << "s" << endl;

	} catch (const std::runtime_error& e)
	{
		cerr << "Tests failed with [" << e.what() << "]" << endl;
		return EXIT_FAILURE;
	}
	cerr << "Tests completed." << endl;
	return EXIT_SUCCESS;
}