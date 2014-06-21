#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#include "lib/spasm.h"
#include "lib/mov.h"

using namespace std;
using namespace spasm;

#include "tests/mov_test.h"

/*
	Testing is done with GNU AS. Basically, the test case generates both symbolic assembly code
	and corresponding hex-encoded binary code. Then the program is assembled with `as` and 
	dissassebled with `objdump`. Then the input is parsed and compared with a Python script.
	
	See test.py for details and implementation.
*/

int main()
{
	std::vector<void (*) (ofstream&, ofstream&)> tests = { 
		mov_test
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