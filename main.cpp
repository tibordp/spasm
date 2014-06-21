#include <iostream>
#include <cstdint>
#include <array>
#include <iomanip>
#include <map>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include <stdexcept>
#include <exception>
#include <limits>
#include <fstream>

#include "spasm.h"
#include "mov.h"

using namespace std;

bool indirect_test()
{
	ofstream instructions("instructions.txt");
	ofstream bytes("bytes.txt");

	instructions << ".intel_syntax noprefix" << endl;

	for (auto i : cpu_registers::all())
	{
		if (i.type > register_type::r64)
			continue;

		for (auto j : cpu_registers::all())
			for (auto k : cpu_registers::all())
				for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
				{
					auto X = mov(i, { multiplier, j, k });

					if (!X.empty())
					{
						instructions << "mov " << i.name << ", " << sib_specifier({ multiplier, j, k }).to_string();
						instructions << endl;

						bool first = true;

						for (int i : X)
						{
							if (first)
								first = false;
							else
								bytes << " ";
							bytes << setfill('0') << setw(2) << hex << i;
						}

						bytes << endl;
					}
				}
	}

	return true;
}

int main()
{
	auto tests = { [] { return indirect_test(); } };
	for (auto test : tests)
		if (!test())
		{
			cerr << "Test failed";
			return 1;
		}

	return 0;
}