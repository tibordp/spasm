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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "lib/spasm.h"
#include "lib/mov.h"
#include "lib/executable.h"

using namespace std;
using namespace spasm;

#include "tests/mov_test.h"
#include "tests/executable_test.h"

/*
	Testing is done with GNU AS. Basically, the test case generates both symbolic assembly code
	and corresponding hex-encoded binary code. Then the program is assembled with `as` and 
	dissassebled with `objdump`. Then the input is parsed and compared with a Python script.
	
	See test.py for details and implementation.
*/

int main()
{
	std::vector<void (*) (ofstream&, ofstream&)> tests = { 
		executable_test, //mov_test, 
	};

	ofstream instructions("instructions.txt");
	ofstream bytes("bytes.txt");

	instructions << ".intel_syntax noprefix" << endl;

	try {
		for (auto test : tests) 
			test(bytes, instructions);
	} catch (const std::exception& e)
	{
		cerr << "Tests failed with [" << e.what() << "]" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}